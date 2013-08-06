#!/usr/bin/env ruby

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.

require 'rubygems'
require 'dbi'

#for index in 0..ARGV[1].to_i-1 do


	# instantiation
	db=DBI.connect("DBI:SQLRelay:host=localhost;port="+
			"9000;socket=/tmp/test.socket;","test","test")


	# debug and autocommit attributes
	db["sqlrelay_debug"]=true
	#db["AutoCommit"]=true


	# query functions
	stmt=db.prepare("select 1,2,3,4,5 from dual")

	print "FETCH ALL\n"
	stmt.execute()
	for i in stmt.fetch_all() do
		for j in i do
			print j+" "
		end
		print "\n"
	end
	print "\n\n"


	print "FETCH ONE\n"
	stmt.execute()
	for j in stmt.fetch() do
		print j+" "
	end
	print "\n\n"


	print "FETCH MANY\n"
	stmt.execute()
	for i in stmt.fetch_many(5) do
		for j in i do
			print j+" "
		end
		print "\n"
	end
	print "\n\n"


	print "FETCH SCROLL\n"
	stmt.execute()
	print "NEXT\n"
	for j in stmt.fetch_scroll(DBI::SQL_FETCH_NEXT) do
		print j+" "
	end
	print "\n"
	#print "PRIOR\n"
	#for j in stmt.fetch_scroll(DBI::SQL_FETCH_PRIOR) do
		#print j+" "
	#end
	#print "\n"
	print "FIRST\n"
	for j in stmt.fetch_scroll(DBI::SQL_FETCH_FIRST) do
		print j+" "
	end
	print "\n"
	print "LAST\n"
	for j in stmt.fetch_scroll(DBI::SQL_FETCH_LAST) do
		print j+" "
	end
	print "\n"
	print "ABSOLUTE 0\n"
	for j in stmt.fetch_scroll(DBI::SQL_FETCH_ABSOLUTE,0) do
		print j+" "
	end
	print "\n"
	#print "RELATIVE 5\n"
	#for j in stmt.fetch_scroll(DBI::SQL_FETCH_RELATIVE,5) do
		#print j+" "
	#end
	print "\n\n"


	# row count
	print "ROWS\n"
	print stmt.rows()
	print "\n\n"


	# column names
	print "COLUMNS\n"
	columns=stmt.column_info()
	for i in columns do
		print i['name']+":"+i['type_name']+":"
		print i['precision']
		print "\n"
	end
	print "\n\n"


	# bind functions
	print "BIND FUNCTIONS\n"
	stmt=db.prepare("select :var1,:var2,:var3 from dual")

	stmt.bind_param("var1",1,false)
	stmt.bind_param("var2","hello",false)
	stmt.bind_param("var3",1.1,false)
	stmt.execute()
	rows=stmt.fetch_all()
	for i in rows do
		for j in i do
			print j+" "
		end
		print "\n"
	end

	stmt.bind_param("var1",2,false)
	stmt.bind_param("var2","hi",false)
	stmt.bind_param("var3",2.22,false)
	stmt.execute()
	rows=stmt.fetch_all()
	for i in rows do
		for j in i do
			print j+" "
		end
		print "\n"
	end

	stmt.bind_param("var1",3,false)
	stmt.bind_param("var2","bye",false)
	stmt.bind_param("var3",3.333,{'precision'=>1,'scale'=>4})
	stmt.execute()
	rows=stmt.fetch_all()
	for i in rows do
		for j in i do
			print j+" "
		end
		print "\n"
	end
	print "\n\n"
	

	# ping
	print "PING\n"
	if db.ping()
		print "ping succeeded\n"
	else
		print "ping failed\n"
	end
	print "\n\n"


	# commit and rollback
	print "COMMIT\n"
	db.commit()
	print "\n\n"

	print "ROLLBACK\n"
	db.rollback()
	print "\n\n"


	# error message
	print "ERROR HANDLING\n"
	begin
		badstmt=db.prepare("invalid query")
		badstmt.execute()
	rescue DBI::ProgrammingError => error
		print error
		print "\n"
	end

    
	# invalid attribute
	begin
		db["invalid"]=true
	rescue DBI::NotSupportedError => error
		print error
		print "\n"
	end


	GC::start()
#end
