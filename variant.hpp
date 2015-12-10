#include <utility>
#include <typeinfo>
#include <type_traits>
#include <iostream>
#include <string>
#include "patch.hpp"

// static maximum size
template<size_t arg, size_t ... args>
struct static_max;
template<size_t arg>
struct static_max<arg> {
    static const size_t value = arg;
};
template <size_t fst, size_t snd, size_t ... args>
struct static_max<fst, snd, args...> {
    static const size_t value = fst >= snd ? static_max<fst, args...>::value : static_max<snd, args...>::value;
};

// destory, copy and move
template<typename... Ts>
struct variant_helper;
template<typename F, typename... Ts>
struct variant_helper<F, Ts...> {
    inline static void destroy(size_t id, void * data)  {
        if (id == typeid(F).hash_code()) {
            reinterpret_cast<F*>(data)->~F();
        }
        variant_helper<Ts...>::destroy(id, data);
    }
    inline static void move(size_t old_t, void * old_v, void * new_v) {
        if (old_t == typeid(F).hash_code()) {
            new (new_v) F(std::move(*reinterpret_cast<F*>(old_v)));
        }
        variant_helper<Ts...>::move(old_t, old_v, new_v);       
    }
    inline static void copy(size_t old_t, const void * old_v, void * new_v) {
        if (old_t == typeid(F).hash_code()) {
            new (new_v) F(*reinterpret_cast<const F*>(old_v));
        }
        variant_helper<Ts...>::copy(old_t, old_v, new_v);       
    }   
};
template<> struct variant_helper<>  {
    inline static void destroy(size_t, void *) { }
    inline static void move(size_t, void *, void *) { }
    inline static void copy(size_t, const void *, void *) { }
};

// variant type.
template<typename... Ts>
struct variant {
private: 
    using data_t = typename std::aligned_union<static_max<sizeof(Ts)...>::value, Ts...>::type;
    using helper_t = variant_helper<Ts...>;

    data_t data;
    size_t type_id;
public: 
    variant() : type_id(typeid(void).hash_code()) {   }
    variant(const variant<Ts...> & old) : type_id(old.type_id) {
        helper_t::copy(old.type_id, &old.data, &data);
    }
    variant(variant<Ts...>&& old) : type_id(old.type_id) {
        helper_t::move(old.type_id, &old.data, &data);
    }

    // move and copy assignment operators..
    variant<Ts...> & operator = (const variant<Ts...> & old) {
        this->type_id = old.type_id;
        helper_t::copy(old.type_id, &old.data, &(this->data));
        return *this;
    }
    variant<Ts...> & operator = (variant<Ts...> && old) {
        std::swap(type_id, old.type_id);
        std::swap(data, old.data);
        return *this;
    }

    // validate data type.
    template<typename T>
    bool is() {
        return this->type_id == typeid(T).hash_code();
    }
    bool is(size_t type_id = typeid(void).hash_code()) {
        return this->type_id == type_id;
    }
    bool validate() {
        return type_id != typeid(void).hash_code();
    }

    // set and get value from variant type.
    template<typename T, typename... Args>
    void set(Args&&... args) {
        // destroy the current contents, then create new one.    
        helper_t::destroy(type_id, &data);      
        new (&data) T(std::forward<Args>(args)...);
        type_id = typeid(T).hash_code();
    }
    template<typename T>
    T& get() {
        // type cast.
        if (this->is<T>()) {
            return *reinterpret_cast<T*>(&data);
        }
        throw std::bad_cast();
    }

    ~variant() {
        helper_t::destroy(type_id, &data);
    }  
};

