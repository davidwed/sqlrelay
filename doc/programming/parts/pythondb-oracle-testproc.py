cur.execute('begin exampleproc(:in1,:in2,:in3); end;',{'in1':1,'in2':1.1,'in3':'hello'})
