var a : integer;

procedure p1(var c :integer);
begin
	c := c+1;
end;

procedure p2(var b :integer); 
begin
	b := b+1;
	p1(b);
end;

begin
	a := 0;
	p2(a);
	if a = 2 then
        write("reference argument right.")
    else
        write("reference argument failed.", a)
    ;
end.

