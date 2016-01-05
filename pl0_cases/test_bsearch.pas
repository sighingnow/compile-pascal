var 
    list : array[10] of integer;
    i : integer;

function p(low,high,targ : integer) : integer;
var mid : integer;
begin
    if low > high then
        p := -1
    else if list[(low+high)/2] = targ then
        p := targ
    else if list[(low+high)/2] < targ then
        p := p((low+high)/2+1,high,targ)
    else p := p(low,(low+high)/2-1,targ)
end;

begin
    for i := 0 to 9 do
        list[i] := i;

    if p(0,0,0) = 0 then
        write("bsearch (0, 0, 0) success.")
    else
        write("bsearch (0, 0, 0) failed.", p(0,0,0));
    if p(0,9,10) = -1 then
        write("bsearch (0, 9, 10) success.")
    else
        write("bsearch (0, 9, 10) failed.", p(0,9,10));
    if p(0,9,1) = 1 then
        write("bsearch (0, 9, 1) success.")
    else
        write("bsearch (0, 9, 1) failed.", p(0,9,1));
    if p(0,9,2) = 2 then
        write("bsearch (0, 9, 2) success.")
    else
        write("bsearch (0, 9, 2) failed.", p(0,9,2));
    if p(0,9,4) = 4 then
        write("bsearch (0, 9, 4) success.")
    else
        write("bsearch (0, 9, 4) failed.", p(0,9,4));
    if p(0,9,8) = 8 then
        write("bsearch (0, 9, 8) success.")
    else
        write("bsearch (0, 9, 8) failed.", p(0,9,8));
end.