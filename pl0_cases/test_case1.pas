const a = 1, b = 2, c = 3, d = 'd', e = 'e';
var x, y, z: integer;

begin
    case (a+b) of
        3: write("Case1 success");
        4: write("Case2 failed", (a+b))
    end;
    x := 10;
    y := 20;
    z := 30;
    case (x*y+z) of
        10  : write("Case2 failed.", (x*y+z));
        20  : write("Case2 failed.", (x*y+z));
        'a':  write("Case2 failed.", (x*y+z));
        230 : write("Case2 success");
        321: write("Case2 failed.", (x*y+z))
    end;
end.
