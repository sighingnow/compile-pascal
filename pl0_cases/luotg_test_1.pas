const a = 1;
var retval, i, num, q, w : integer; c : char; array1 : array[14] of integer;
function f1(b:integer):integer;
	begin
		if b = a
		then f1:= 1
		else
			f1 := b+f1(b-1);
	end;
function f12(ret:integer):char;
	begin
		f12 := ret;
	end;
function f2:integer;
	var array2 : array[14] of integer;
	begin
		array2[0] := 0; array2[1] := 1; array2[2] := 2; array2[3] := 3; array2[4] := 4;
		array2[5] := 5; array2[6] := 6; array2[7] := 7; array2[8] := 8; array2[9] := 9;
		array2[10] := 10; array2[11] := 11; array2[12] := 12; array2[13] := 13;
		array1[0] := array2[0];
		array1[1] := array2[1];
		for i := 2 to 13
			do
			array1[array2[i]] := array1[array2[i-1]]+array1[array2[i-2]];
		f2 := array1[array2[f12(array2[f1(5)-2])]];
	end;
function f3:integer;
	var q,w,e:integer;
	begin
		q:=0;
		w:=1;
		e:=2;
		if q < w then 
			if e > w then 
				if q = q then
					if q<= q then
						if e>= q then
							if q<>q then
							else
								for i:= 0 to 22 do
									begin
										w := q + a;
										q := w;
									end
						else
					else
				else 
			else
		else
			;
		f3 := w;
	end;
procedure p1(var c :integer);
	begin
		c := c+1;
	end;
procedure p2(var b :integer); 
	begin
		b := b+1;
		p1(b);
	end;
procedure p3;
	var step,a1,a2,a3,a4,a5,a6,a7,a8,a9:char;
	begin
		a1 := f1(1); a2 := 2; a3 := f1(2); a4 := 4; a5 := 5; a6 := f1(a3); a7 := 7; a8 := 8; a9 := 9;
		for step := 0 to 1 do
			begin
				a1 := a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 - (f1(4)+1)*4;
				a2 := a2 + a3*a4 + a4*a5 + a6 + a7 +a8 + a9 - step + a9*step - 31*2;
				a3 := a3*a4*a5 + a4*a9 + a2 - a4*a5 - a4*a8 - 43;
				a4 := a1 + a2 + a3 + a5*a6 +a3 + a8*a9*f1(2) - a1*a7 - 245 + 1;
				a5 := a1 + a2 + a3 + a4 + a5*a5 + a6 + a7 + a8 + a9 - 5*21 + 11*4 + 1;
				a6 := a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 - 7*10 + 5*7 - 4;
				a7 := a1 + a4 + a5 + a7 + a8 + a9 - 7*12 + 23*2 + 11;
				a8 := a2 + a3 + a4 + a5 + a6 + a8 + a9 - 5*14 + 13*2 + 2*2*2*2 -1;
				a9 := a1 + a2 + a3 + a4 + a5 + a6 - 5*10 + 1*13*1 + 1*1*1*1 + 24;
			end;
			a1 := a1 + 103; a2 := a2 + 91; a3 := a3 + 97; a4 := a4 + 83; a5 := a5 - 1; a6 := a6 + 33; a7 := a7 + 43; a8 := a8 - 60; a9 := a9 - 69;
			write(a1);write(a2);write(a3);write(a3);write(a4);write(a5);write(a6);write(a4);write(a7);write(a8);write(a9);
			write("");
	end;
begin
	write("begin test recursion.");
	retval := f1(f1(f1(3)));
	if retval = 231
	then 
		write("recursion success.")
	else
		write("recursion failed.", retval);

	retval := f2;
	if retval = 233 then
		write("array operation right.")
	else
		write("array operation failed.", retval);

	retval := f3;
	if retval = 23 then
		write("Jump label set success.")
	else
		write("Jump label set failed.", retval);

	i := 0;
	p2(i);
	if i = 2 then
		write("var parameter success.")
	else
		write("var parameter failed.");

	write("begin test register arrange(if success, would print hello, world).");
	p3;
end.

