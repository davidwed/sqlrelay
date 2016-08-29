create or replace package types as
        type cursorType is ref cursor;
end;

create function testproc return types.cursortype is
        l_cursor    types.cursorType;
begin
        open l_cursor for select * from mytable;
        return l_cursor;
end;
