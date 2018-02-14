from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=con.cursor()

try:
	cur.execute('select * from my_nonexistant_table')
except PySQLRDB.DatabaseError, e:
	print e
