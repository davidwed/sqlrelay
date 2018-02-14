create procedure exampleproc @in1 int, @in2 float, @in3 varchar(20), @out1 int output, @out2 int output, @out3 int output as
        select @out1=convert(varchar(20),@in1),
                @out2=convert(varchar(20),@in2),
                @out2=convert(varchar(20),@in2)
