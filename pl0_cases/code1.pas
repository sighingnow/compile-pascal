const a = 10, b = 'b';
var c, d: integer;
    e, f: array[100] of char;

procedure proc(var x: integer; y, z: char);
begin
    read(x, y, z);
    x := y + z;
end;

procedure noparam;
begin
    read(c, c, c);
    c := c + c + c + c + c;
end;

function func(var x: char; y, z: char): integer;
begin
    x := y + z;
    func := (x + y) + (z + x) + (y + z) + x + y + z;
    if x = y then
        func := func + 1
    else
        func := func + 2
    ;
    x := -x * y;
end;

begin
    read(c, d);
    c := b;

end.

