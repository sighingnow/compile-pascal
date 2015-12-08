function sum(n: integer): integer;
begin
    if n = 1 then
        sum := 1
    else
        sum := n + sum(n-1)
end;

begin
    if sum(10) = 55 then
        write("fibonacci 10 success.")
    else
        write("fibonacci 10 failed.", sum(10));
    if sum(100) = 5050 then
        write("fibonacci 20 success.")
    else
        write("fibonacci 20 failed.", sum(100));
    if sum(1000) = 500500 then
        write("fibonacci 50 success.")
    else
        write("fibonacci 50 failed.", sum(1000));
end.
