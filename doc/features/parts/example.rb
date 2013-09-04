#! /usr/bin/env ruby

require 'sqlrelay'

sqlrcon=SQLRConnection.new("examplehost",9000,
				"/tmp/example.socket",
				"exampleuser",
				"examplepassword",0,1)
sqlrcur=SQLRCursor.new(sqlrcon)

sqlrcur.sendQuery("select * from exampletable")
for row in 0..sqlrcur.rowCount()-1
	for col in 0..sqlrcur.colCount()-1
		print sqlrcur.getField(row,col),","
	end
	print "\n"
end
