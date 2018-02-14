from SQLRelay import PySQLRClient

... get the filename from the previous page ...

... get the page to display from the previous page ...

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.openCachedResultSet(filename)
con.endSession()

for row in range(pagetodisplay*20,((pagetodisplay+1)*20)-1):
        for col in range(0,cur.colCount()-1):
                print cur.getField(row,col), ',',
        print
