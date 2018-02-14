create procedure exampleproc(in1 in number, in2 in number, in3 in varchar2) is
begin
        insert into mytable values (in1,in2,in3);
end;
