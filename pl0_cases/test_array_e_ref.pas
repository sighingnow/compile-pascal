var arr : array[2] of integer;
function swap(var l, r : integer) : integer;
    var tmp : integer;
    begin
        tmp := l;
        l := r;
        r := tmp;
        swap := 666;
    end;
begin
    arr[0] := 0;
    arr[1] := 1;
    arr[0] := swap(arr[0],arr[1]);
    if arr[0] = 666 then
        if arr[1] = 0 then
            write("var array member success.")
        else
            write("failed.")
    else
        write("failed");
end.
