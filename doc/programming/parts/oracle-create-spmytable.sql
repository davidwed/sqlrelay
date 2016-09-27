FUNCTION sp_mytable RETURN types.cursorType
l_cursor types.cursorType;
BEGIN
        OPEN l_cursor FOR SELECT * FROM mytable;
        RETURN l_cursor;
END;
