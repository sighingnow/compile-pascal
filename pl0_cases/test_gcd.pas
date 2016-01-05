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
    if gcd(456, 8910) = 6 then
        write("gcd-1 success.")
    else
        write("gcd-1 failed.", gcd(456, 8910))
    ;
    if gcd(3456, 78910) = 2 then
        write("gcd-2 success.")
    else
        write("gcd-2 failed.", gcd(3456, 78910))
    ;
end.
