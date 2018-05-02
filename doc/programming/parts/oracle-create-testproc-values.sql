create procedure exampleproc(in1 in number, in2 in number, in3 in varchar2, out1 out number, out2 out number, out3 out varchar2) is
begin
        out1:=in1;
        out2:=in2;
        out3:=in3;
end;
