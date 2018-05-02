cur.execute('select * from examplefunc(?,?,?)',{'in1':1,'in2':1.1,'in3':'hello'})
out1=cur.fetchone()[0]
out2=cur.fetchone()[1]
out3=cur.fetchone()[2]
