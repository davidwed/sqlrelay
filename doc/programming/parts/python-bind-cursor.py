from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection("sqlrserver",9000,"","user","password",0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.prepareQuery("begin  :curs:=sp_mytable end;")
cur.defineOutputBindCursor("curs")
cur.executeQuery()

bindcur=cur.getOutputBindCursor("curs")
bindcur.fetchFromBindCursor()

# print fields from table
for i in range(0,bindcur.rowCount()-1):
        for j in range(0,bindcur.colCount()-1):
                print bindcur.getField(i,j), ", "
        print
