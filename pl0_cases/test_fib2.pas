var x, y, z: integer;

function fib(n: integer): integer;
var a, b, c, i: integer;
begin
    a := 0;
    b := 1;
    if n > 1 then begin
        for i := 2 to n do begin
            c := a + b;
            a := b;
            b := c;
        end;
        fib := c
    end
    else begin
        if n = 0 then
            fib := a
        else
            fib := b
    end
end;

begin
    if fib(10) = 55 then
        write("fibonacci 10 success.")
    else
        write("fibonacci 10 failed.", fib(10));
    if fib(20) = 6765 then
        write("fibonacci 20 success.")
    else
        write("fibonacci 20 failed.", fib(20));
    if fib(30) = 832040 then
        write("fibonacci 50 success.")
    else
        write("fibonacci 50 failed.", fib(30));
end.
