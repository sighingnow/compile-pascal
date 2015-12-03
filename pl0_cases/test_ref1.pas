var x, y: integer;

procedure p(var t: integer);
begin
    t := 100;
end;

begin
    y := 100;
    p(x);
    if x = y then 
        write("reference argument success.")
    else
        write("reference argument failed.", x);
end.

