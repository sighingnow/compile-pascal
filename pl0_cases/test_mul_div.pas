var a, b, c: integer;

begin
    a := 101;
    b := 22;
    if a * b = 2222 then
        write("multiply success.")
    else
        write("multiply failed.", a * b);
    if a / b = 4 then
        write("division success.")
    else
        write("division failed.", a * b);
    if a % b = 13 then
        write("module success.")
    else
        write("module failed.", a * b);
end.


