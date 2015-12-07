var x, y: integer;

procedure p(var t: integer);
begin
    t := 100;
end;

begin
    y := 100;
    p(x);
    if x = y then 
        write("reference argument 1 success.")
    else
        write("reference argument 2 failed.", x);
end.

