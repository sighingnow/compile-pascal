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
        write("array operation right.")
    else
        write("array operation failed.", a)
    ;
    if b = x then
        write("array operation right.")
    else
        write("array operation failed.", b)
    ;
end.
