cur.execute('select * from testproc(?,?,?)',{'in1':1,'in2':1.1,'in3':'hello'})
result=cur.fetchone()[0]
