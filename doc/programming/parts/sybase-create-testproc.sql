create procedure exampleproc @in1 int, @in2 float, @in3 varchar(20) as
        insert into mytable values (@in1,@in2,@in3)
