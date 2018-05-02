create procedure exampleproc(in in1 int, in in2 double, in in3 varchar(20), out out1 int, out out2 double, out out3 varchar(20)) language sql
begin
        set out1 = in1;
        set out2 = in2;
        set out3 = in3;
end
