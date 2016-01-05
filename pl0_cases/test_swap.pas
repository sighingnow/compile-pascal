var
    a, b, x, y: integer;

procedure swap(var a, b: integer);
var
    t: integer;
begin
    t := a;
    a := b;
    b := t;
end;

begin
    x := 133;
    y := 233;
    a := x;
    b := y;
    swap(a, b);
    if a = y then
        write("swap 1 right.")
    else
        write("swap 1 failed.", a)
    ;
    if b = x then
        write("swap 2 right.")
    else
        write("swap 2 failed.", b)
    ;
end.
