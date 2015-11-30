const a = 10, b = 'b';
var c, d: integer;
    e, f: array[100] of char;
    h: char;

procedure proc(var x: integer; y, z: char);
    var k, h, j: integer;
    function func(var x: char; y, z: char): integer;
        var xx, yy, zz: char;
        procedure noparam;
        begin
            read(c, c, c);
            c := c + c + c + c + c;
        end;
    begin
        noparam;
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
    read(x, y, z);
    x := y + z;
    x := func(x, y, z);
end;

begin
    proc(c, d);
    c := b;
    h := c;
end.

