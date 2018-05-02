cur.execute('select * from examplefunc($1,$2,$3) as (col1 int, col2 float, col3 char(20))',{'1':1,'2':1.1,'3':'hello'})
out1=cur.fetchone()[0]
out2=cur.fetchone()[1]
out3=cur.fetchone()[2]
