create procedure exampleproc(in in1 int, in in2 double, in in3 varchar(20), out out1 int) language sql
begin
        set out1 = in1;
end
