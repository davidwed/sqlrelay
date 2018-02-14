create function examplefunc() returns setof record as '
        declare output record;
begin
        for output in select * from mytable loop
                return next output;
        end loop;
        return;
end;
' language plpgsql
