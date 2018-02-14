create procedure exampleproc(in1 integer, in2 float, in3 varchar(20)) returns (out1 integer) as
begin
        out1=in1;
        suspend;
end;
