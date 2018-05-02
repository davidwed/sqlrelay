from SQLRelay import PySQLRDB

con=PySQLRDB.connect('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=con.cursor()

cur.executemany('insert into my_table values (:val1,:val2,:val3)',
		[{'val1':1,'val2':'hello','val3':1.11},
		{'val1':2,'val2':'hi','val3':2.22},
		{'val1':3,'val2':'bye','val3':3,33}])
