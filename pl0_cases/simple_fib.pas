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
    read(a);
    write(fib(a));
end.