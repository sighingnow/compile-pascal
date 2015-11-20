const
    zero = '0', minus = '1';
var
    str: array[80] of char;
    sp: integer;
    a, b: integer;
    ans: integer;

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

procedure tostr(x: integer);
var
    rem: integer;
    neg: integer;
begin
    sp := 79;
    if x < 0 then begin
        x := -x;
        neg := 1
    end
    else
        neg := 0;
    


    if neg <> 0 then begin
        sp := sp-1;
        str[sp] := minus;
    end;
end;

procedure printstr;
var
    i: integer;
begin
    for i:=sp to 79 do
        write(str[sp])
end;

function gcd(a, b: integer): integer;
begin
    if a<b then
        swap(a, b)
    ;
    if b=0 then
        gcd := 0
    else begin
        gcd := gcd(b, mod(a, b))
    end;
end;

begin
    str[79] := 10;
    write("Please input a, b");
    read(a,b);
    ans := gcd(a,b);
    write("gcd(a,b)=", ans);
    tostr(ans);
    printstr;
end.
