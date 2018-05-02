cur.execute('select * from examplefunc($1,$2,$3)',{'1':1,'2':1.1,'3':'hello'})
result=cur.fetchone()[0]
