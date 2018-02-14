cur.execute('set @out1=0, @out2=0.0, @out3=\'\'')
cur.execute('call examplefunc(@out1,@out2,@out3)')
cur.execute('select @out, @out2, @out3')
out1=cur.fetchone()[0]
out2=cur.fetchone()[1]
out3=cur.fetchone()[2]
