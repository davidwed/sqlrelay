from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=con.cursor()

cur.execute('select * from my_table where column1>:val1 and column2=:val2 and column3<:val3',{'val1':1,'val2':'hello','val3':50.546})

... process the result set ...
