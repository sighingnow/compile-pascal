const a = 1;
var c: integer;

function f(b: integer): integer;
begin
	if b = a then begin
		f:= 1
	end
	else begin
		f := b + f(b - 1);
	end
end;
begin
	if f(f(f(3))) = 231 then 
		write("recursion success.")
	else
		write("recursion failed.", c);
end.