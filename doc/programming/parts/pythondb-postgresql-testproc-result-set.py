cur.execute('select * from examplefunc() as (col1 int, col2 float, col3 char(20))',{'in1':1,'in2':1.1,'in3':'hello'})
result=cur.fetchall()
