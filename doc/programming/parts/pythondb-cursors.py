from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cursor1=con.cursor()
cursor2=con.cursor()

cursor1.execute('select * from my_huge_table')

for a in cursor1.fetchall():
        cursor2.execute('insert into my_other_table values (:1,:2,:3)',{':1',a[0],':2',a[1],':3',a[2]})
