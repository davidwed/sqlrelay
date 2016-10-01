cur.execute('set @out1=0')
cur.execute('call testproc()')
cur.execute('select @out1')
result=cur.fetchone()[0]
