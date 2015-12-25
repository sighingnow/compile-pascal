var a1,a2,a3,a4,a5:integer;
    g :char;
procedure p1;
    var l1,l2,l3,l4,l5:integer;
        g2:integer;
    procedure p2;
        procedure p3(var tmp:integer);
            procedure p4(var tmp:integer);
                var tmp4: integer;
                procedure p5(var tmp:integer);
                begin
                    tmp:= 1024;
                    a1:=256;
                end;
            begin
                p5(tmp);
            end;
        begin
            p4(tmp);
        end;
    begin
        p3(l1);
    end;
begin
    p2;
    if l1 = 1024 then
        write("success")
    else
        write("failed", l1)
    ;
end;
begin
    a1:=0;
    p1;
    if a1 <> 256 then
        write("failed", a1)
    else
        write("success")
    ;
end.

