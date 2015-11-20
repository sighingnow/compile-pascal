var
    a, b: integer;

procedure swap(var a, b: integer);
var
    t: integer;
begin
    t := a; a := b; b := t;
end;

function mod(a, b: integer): integer;
begin
    mod := a-a/b*b;
end;

function gcd(a, b: integer): integer;
begin
    if a<b then
        swap(a, b)
    ;
    if b=0 then
        gcd := a
    else
        gcd := gcd(b, mod(a, b))
    ;
end;

begin
    read(a, b);
    write(gcd(a, b));
end.
