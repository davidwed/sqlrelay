cur.execute('select examplefunc(?,?,?)',{'1':1,'2':1.1,'3':'hello'})
out1=cur.fetchone()[0]
out2=cur.fetchone()[1]
out3=cur.fetchone()[2]
