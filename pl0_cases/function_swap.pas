var
    a, b: integer;

procedure swap(var a, b: integer);
var
    t: integer;
begin
    t := a;
    a := b;
    b := t;
end;

begin
    read(a);
    read(b);
    swap(a, b);
    write(a);
    write(b);
end.
