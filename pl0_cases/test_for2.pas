const c = 1;
var a, b, i: integer;

begin
    a := 0;
    b := 0;
    for i:= 0 to 10 do
    begin
    end;
    if i = 10 then
        write("for-2-1 success.")
    else
        write("for-2-1 failed.", i);

    i := 100;
    for i := 2 to 1 do
    begin
    end;
    if i = 100 then
        write("for-2-2 success.")
    else
        write("for-2-2 failed.", i);

    for i := 1 downto 100 do
    begin
    end;
    if i = 100 then
        write("for-2-3 success.")
    else
        write("for-2-3 failed.", i);
end.

