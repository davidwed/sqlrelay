from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=con.cursor()

cur.execute('select * from my_table')

print 'rowcount:', cur.rowcount

print 'the first row:'
print cur.fetchone()
print

print 'the next three rows:'
print cur.fetchmany(3)
print

print 'the rest of the rows:'
print cur.fetchall()
print
