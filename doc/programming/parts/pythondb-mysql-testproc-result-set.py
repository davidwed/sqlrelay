cur.execute('call testfunc()',{'1':1,'2':1.1,'3':'hello'})
result=cur.fetchall()
