from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=con.cursor()

cur.execute('select * from my_table')

for name,type,length in cur.desc:
        print 'Name:          ', name
        print 'Type:          ', type
        print 'Length:        ', length
