create procedure exampleproc(in1 integer, in2 float, in3 varchar(20)) as
begin
        insert into mytable values (in1,in2,in3);
        suspend;
end;
