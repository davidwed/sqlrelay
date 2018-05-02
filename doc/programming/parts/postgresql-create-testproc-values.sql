create function examplefunc(int,float,char(20)) returns record as '
declare
        output record;
begin
        select $1,$2,$3 into output;
        return output;
end;
' language plpgsql
