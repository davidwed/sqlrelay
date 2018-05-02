create procedure exampleproc(in1 integer, in2 float, in3 varchar(20)) returns (out1 integer, out2 float, out3 varchar(20)) as
begin
        out1=in1;
        out2=in2;
        out3=in3;
        suspend;
end;
