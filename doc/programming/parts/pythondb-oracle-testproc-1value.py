cur.execute('select exampleproc(:in1,:in2,:in3) from dual',{'in1':1,'in2':1.1,'in3':'hello'})
result=cur.fetchone()[0]
