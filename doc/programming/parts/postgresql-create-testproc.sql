create function testfunc(int,float,varchar(20)) returns void as '
begin
        insert into mytable values ($1,$2,$3);
        return;
end;' language plpgsql
