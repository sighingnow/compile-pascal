var a: integer;

function fib(n: integer): integer;
begin
    if n < 2 then
        if n = 0 then
            fib := 0
        else
            fib := 1
    else
        fib := fib(n-1) + fib(n-2)
end;

begin
    if fib(10) = 55 then
        write("fibonacci 10 success.")
    else
        write("fibonacci 10 failed.", fib(10))
    ;
    if fib(20) = 6765 then
        write("fibonacci 20 success.")
    else
        write("fibonacci 20 failed.", fib(20))
    ;
    if fib(30) = 832040 then
        write("fibonacci 50 success.")
    else
        write("fibonacci 50 failed.", fib(30))
    ;
end.