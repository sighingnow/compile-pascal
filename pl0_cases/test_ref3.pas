const x=20;
var y, z, a: integer;

function xel(var arg1, arg2: integer): integer;
begin
    z := 2;
    arg1 := arg1 + arg2 + x
end;

begin
    y := 10;
    z := 9;
    a := xel(y, z);
    if y = 32 then
        write("reference argument 3 success.")
    else
        write("reference argument 3 failed.", y)
    ;
end.
