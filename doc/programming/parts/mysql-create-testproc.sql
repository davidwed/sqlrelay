create procedure exampleproc(in in1 int, in in2 float, in in3 varchar(20))
begin
        insert into mytable values (in1,in2,in3);
end;
