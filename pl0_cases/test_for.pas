const c = 1;
var a, b, i: integer;

begin
    a := 0;
    b := 0;
	for i:= 0 to 10 do
	begin
		b := a + c;
		a := b;
	end;
	if a = 11 then
		write("for success.")
	else
		write("for failed.", a);
end.