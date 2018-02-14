from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

cur.sendQuery('select * from my_table')
con.endSession()

for i in range(0,cur.colCount()-1):
        print 'Name:          ', cur.getColumnName(i)
        print 'Type:          ', cur.getColumnType(i)
        print 'Length:        ', cur.getColumnLength(i)
        print 'Precision:     ', cur.getColumnPrecision(i)
        print 'Scale:         ', cur.getColumnScale(i)
        print 'Longest Field: ', cur.getLongest(i)
        print 'Nullable:      ', cur.getColumnIsNullable(i)
        print 'Primary Key:   ', cur.getColumnIsPrimaryKey(i)
        print 'Unique:        ', cur.getColumnIsUnique(i)
        print 'Part Of Key:   ', cur.getColumnIsParyOfKey(i)
        print 'Unsigned:      ', cur.getColumnIsUnsigned(i)
        print 'Zero Filled:   ', cur.getColumnIsZeroFilled(i)
        print 'Binary:        ', cur.getColumnIsBinary(i)
        print 'Auto Increment:', cur.getColumnIsAutoIncrement(i)
