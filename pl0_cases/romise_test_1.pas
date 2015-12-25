var a1:integer;
function f1(var e1:integer):integer;
    var a1:integer;
    function f2(var e3:integer):integer;
        begin
            e3:=e3+1;
            f2:=5;
        end;
    procedure p2(e4:integer);
        begin
            e4:=e4+1;
            if a1=5 then write("success")
            else write("failed");
        end;
    begin
        a1:=f2(e1);
        e1:=e1+1;
        p2(e1);
        f1:=5;
    end;
procedure p1(var e1:integer);
    var e2:integer;
    begin
        e1:=e1+1;
        e2:=f1(e1);
        if e2=5 then write("success")
        else write("failed")
    end;
begin
    a1:=1;
    p1(a1);
    if a1=4 then write("success")
    else write("failed");
end.
