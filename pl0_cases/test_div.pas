const a = 1, b = 2, c = 3, d = 4;
var x, y, z, i: integer;

function p1(var a: integer): integer;
begin
    for i := 1 to 10 do
        a := a * i;
    p1 := a;
end;

procedure p2(a: integer; var b: integer);
begin
    for i := 1 to 10 do begin
        a := a / i;
    end;
    b := a
end;

begin
    x := 1;
    p2(p1(x), y);
    if x = 3628800 then
        write("fibonacci-1 10 success.")
    else
        write("fibonacci-1 10 failed.", p1(x))
    ;
    if y = 1 then
        write("fibonacci-1 10 success.")
    else
        write("fibonacci-1 10 failed.", y)
    ;
end.