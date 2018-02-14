create procedure exampleproc(out out1 int, out out2 float, out out3 varchar(20)) begin select 1,1.1,'hello' into out1, out2, out3; end;
