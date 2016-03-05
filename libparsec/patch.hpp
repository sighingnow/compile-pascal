#ifndef __PATCH_HPP__
#define __PATCH_HPP__

#include <string>
#include <sstream>

namespace patch {

template <typename T>
inline std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

}

#ifdef ENABLE_PATCH
    #define to_string patch::to_string
#else
    #define to_string std::to_string
#endif

#endif /* __PATCH_HPP__ */
