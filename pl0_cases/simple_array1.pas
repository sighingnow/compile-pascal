var a, b, c, d: integer;
    e: array [10] of integer;

begin
    read(a);
    read(b);
    e[a] := b;
    write("assign to array b: ", e[a]);
end.

