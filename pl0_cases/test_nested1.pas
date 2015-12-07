var a, b, c: integer;

procedure p1;
    procedure p2;
    begin
        write("a in p2 is: ", a);
        a := a - 1;
        p1;
    end;
begin
    write("a in p1 is: ", a);
    if a > 1 then
        p2
end;

begin
    a := 10;
    p1;
end.
