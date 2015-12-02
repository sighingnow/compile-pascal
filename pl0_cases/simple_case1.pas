const a = 1, b = 2, c = 3, d = 'd', e = 'e';
var x, y, z: integer;

begin
    read(x, y, z);
    case (a+b) of
        3: write(x+y+z)
    end;
    case a of
        1  : write(x); 
        2  : write(y);
        'a': begin 
            write(z);
        end
    end;
end.
