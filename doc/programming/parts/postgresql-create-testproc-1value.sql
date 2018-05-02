create function examplefunc(int,float,char(20)) returns int as '
declare
        in1 int;
        in2 float;
        in3 char(20);
begin
        in1:=$1;
        return;
end;
' language plpgsql
