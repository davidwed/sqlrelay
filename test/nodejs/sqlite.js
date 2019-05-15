// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

var	sqlrelay=require("sqlrelay");

function checkSuccess(value,success) {

	if (value==success) {
		process.stdout.write("success ");
	} else {
		console.log(value+"!="+success+" ");
		console.log("failure ");
		process.exit(1);
	}
}
	
var	dbtype;
var	subvars=["var1","var2","var3"];
var	subvalstrings=["hi","hello","bye"];
var	subvallongs=[1,2,3];
var	subvaldoubles=[10.55,10.556,10.5556];
var	precs=[4,5,6];
var	scales=[2,3,4];
var	numvar;
var	stringvar;
var	floatvar;
var	cols;
var	fields;
var	port;
var	socket;
var	id;
var	filename;
var	fieldlens;
	
// instantiation
var	con=new sqlrelay.SQLRConnection("sqlrelay",
				9000,
				"/tmp/test.socket",
				"test","test",0,1);
var	cur=new sqlrelay.SQLRCursor(con);
	
// get database type
console.log("IDENTIFY: ");
checkSuccess(con.identify(),"sqlite");
console.log("\n");
	
// ping
console.log("PING: ");
checkSuccess(con.ping(),1);
console.log("\n");
	
// drop existing table
cur.sendQuery("begin transaction");
cur.sendQuery("drop table testtable");
con.commit();
	
// create a new table
console.log("CREATE TEMPTABLE: ");
cur.sendQuery("begin transaction");
checkSuccess(cur.sendQuery("create table testtable (testint int, testfloat float, testchar char(40), testvarchar varchar(40))"),1);
con.commit();
console.log("\n");
	
console.log("INSERT: ");
cur.sendQuery("begin transaction");
checkSuccess(cur.sendQuery("insert into testtable values (1,1.1,'testchar1','testvarchar1')"),1);
checkSuccess(cur.sendQuery("insert into testtable values (2,2.2,'testchar2','testvarchar2')"),1);
checkSuccess(cur.sendQuery("insert into testtable values (3,3.3,'testchar3','testvarchar3')"),1);
checkSuccess(cur.sendQuery("insert into testtable values (4,4.4,'testchar4','testvarchar4')"),1);
console.log("\n");
	
console.log("AFFECTED ROWS: ");
checkSuccess(cur.affectedRows(),0);
console.log("\n");
	
console.log("BIND BY NAME: ");
cur.prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4)");
checkSuccess(cur.countBindVariables(),4);
cur.inputBind("var1",5);
cur.inputBind("var2",5.5,4,1);
cur.inputBind("var3","testchar5");
cur.inputBind("var4","testvarchar5");
checkSuccess(cur.executeQuery(),1);
cur.clearBinds();
cur.inputBind("var1",6);
cur.inputBind("var2",6.6,4,1);
cur.inputBind("var3","testchar6");
cur.inputBind("var4","testvarchar6");
checkSuccess(cur.executeQuery(),1);
cur.clearBinds();
cur.inputBind("var1",7);
cur.inputBind("var2",7.7,4,1);
cur.inputBind("var3","testchar7");
cur.inputBind("var4","testvarchar7");
checkSuccess(cur.executeQuery(),1);
console.log("\n");
	
console.log("BIND BY NAME WITH VALIDATION: ");
cur.clearBinds();
cur.inputBind("var1",8);
cur.inputBind("var2",8.8,4,1);
cur.inputBind("var3","testchar8");
cur.inputBind("var4","testvarchar8");
cur.validateBinds();
checkSuccess(cur.executeQuery(),1);
console.log("\n");
	
console.log("SELECT: ");
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
console.log("\n");
	
console.log("COLUMN COUNT: ");
checkSuccess(cur.colCount(),4);
console.log("\n");
	
console.log("COLUMN NAMES: ");
checkSuccess(cur.getColumnName(0),"testint");
checkSuccess(cur.getColumnName(1),"testfloat");
checkSuccess(cur.getColumnName(2),"testchar");
checkSuccess(cur.getColumnName(3),"testvarchar");
cols=cur.getColumnNames();
checkSuccess(cols[0],"testint");
checkSuccess(cols[1],"testfloat");
checkSuccess(cols[2],"testchar");
checkSuccess(cols[3],"testvarchar");
console.log("\n");
	
console.log("COLUMN TYPES: ");
checkSuccess(cur.getColumnType(0),"INTEGER");
checkSuccess(cur.getColumnType("testint"),"INTEGER");
checkSuccess(cur.getColumnType(1),"FLOAT");
checkSuccess(cur.getColumnType("testfloat"),"FLOAT");
checkSuccess(cur.getColumnType(2),"STRING");
checkSuccess(cur.getColumnType("testchar"),"STRING");
checkSuccess(cur.getColumnType(3),"STRING");
checkSuccess(cur.getColumnType("testvarchar"),"STRING");
console.log("\n");
	
console.log("COLUMN LENGTH: ");
checkSuccess(cur.getColumnLength(0),0);
checkSuccess(cur.getColumnLength("testint"),0);
checkSuccess(cur.getColumnLength(1),0);
checkSuccess(cur.getColumnLength("testfloat"),0);
checkSuccess(cur.getColumnLength(2),0);
checkSuccess(cur.getColumnLength("testchar"),0);
checkSuccess(cur.getColumnLength(3),0);
checkSuccess(cur.getColumnLength("testvarchar"),0);
console.log("\n");
	
console.log("LONGEST COLUMN: ");
checkSuccess(cur.getLongest(0),1);
checkSuccess(cur.getLongest("testint"),1);
checkSuccess(cur.getLongest(1),3);
checkSuccess(cur.getLongest("testfloat"),3);
checkSuccess(cur.getLongest(2),9);
checkSuccess(cur.getLongest("testchar"),9);
checkSuccess(cur.getLongest(3),12);
checkSuccess(cur.getLongest("testvarchar"),12);
console.log("\n");
	
console.log("ROW COUNT: ");
checkSuccess(cur.rowCount(),8);
console.log("\n");
	
console.log("TOTAL ROWS: ");
checkSuccess(cur.totalRows(),0);
console.log("\n");
	
console.log("FIRST ROW INDEX: ");
checkSuccess(cur.firstRowIndex(),0);
console.log("\n");
	
console.log("END OF RESULT SET: ");
checkSuccess(cur.endOfResultSet(),1);
console.log("\n");
	
console.log("FIELDS BY INDEX: ");
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(0,1),"1.1");
checkSuccess(cur.getField(0,2),"testchar1");
checkSuccess(cur.getField(0,3),"testvarchar1");
console.log();
checkSuccess(cur.getField(7,0),"8");
checkSuccess(cur.getField(7,1),"8.8");
checkSuccess(cur.getField(7,2),"testchar8");
checkSuccess(cur.getField(7,3),"testvarchar8");
console.log("\n");
	
console.log("FIELD LENGTHS BY INDEX: ");
checkSuccess(cur.getFieldLength(0,0),1);
checkSuccess(cur.getFieldLength(0,1),3);
checkSuccess(cur.getFieldLength(0,2),9);
checkSuccess(cur.getFieldLength(0,3),12);
console.log();
checkSuccess(cur.getFieldLength(7,0),1);
checkSuccess(cur.getFieldLength(7,1),3);
checkSuccess(cur.getFieldLength(7,2),9);
checkSuccess(cur.getFieldLength(7,3),12);
console.log("\n");
	
console.log("FIELDS BY NAME: ");
checkSuccess(cur.getField(0,"testint"),"1");
checkSuccess(cur.getField(0,"testfloat"),"1.1");
checkSuccess(cur.getField(0,"testchar"),"testchar1");
checkSuccess(cur.getField(0,"testvarchar"),"testvarchar1");
console.log();
checkSuccess(cur.getField(7,"testint"),"8");
checkSuccess(cur.getField(7,"testfloat"),"8.8");
checkSuccess(cur.getField(7,"testchar"),"testchar8");
checkSuccess(cur.getField(7,"testvarchar"),"testvarchar8");
console.log("\n");
	
console.log("FIELD LENGTHS BY NAME: ");
checkSuccess(cur.getFieldLength(0,"testint"),1);
checkSuccess(cur.getFieldLength(0,"testfloat"),3);
checkSuccess(cur.getFieldLength(0,"testchar"),9);
checkSuccess(cur.getFieldLength(0,"testvarchar"),12);
console.log();
checkSuccess(cur.getFieldLength(7,"testint"),1);
checkSuccess(cur.getFieldLength(7,"testfloat"),3);
checkSuccess(cur.getFieldLength(7,"testchar"),9);
checkSuccess(cur.getFieldLength(7,"testvarchar"),12);
console.log("\n");
	
console.log("FIELDS BY ARRAY: ");
fields=cur.getRow(0);
checkSuccess(fields[0],"1");
checkSuccess(fields[1],"1.1");
checkSuccess(fields[2],"testchar1");
checkSuccess(fields[3],"testvarchar1");
console.log("\n");
	
console.log("FIELD LENGTHS BY ARRAY: ");
fieldlens=cur.getRowLengths(0);
checkSuccess(fieldlens[0],1);
checkSuccess(fieldlens[1],3);
checkSuccess(fieldlens[2],9);
checkSuccess(fieldlens[3],12);
console.log("\n");
	
console.log("INDIVIDUAL SUBSTITUTIONS: ");
cur.sendQuery("drop table testtable1");
checkSuccess(cur.sendQuery("create table testtable1 (col1 int, col2 char, col3 float)"),1);
cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
cur.substitution("var1",1);
cur.substitution("var2","hello");
cur.substitution("var3",10.5556,6,4);
checkSuccess(cur.executeQuery(),1);
console.log("\n");
	
console.log("FIELDS: ");
checkSuccess(cur.sendQuery("select * from testtable1"),1);
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(0,1),"hello");
checkSuccess(cur.getField(0,2),"10.5556");
checkSuccess(cur.sendQuery("delete from testtable1"),1);
console.log("\n");
	
console.log("ARRAY SUBSTITUTIONS: ");
cur.prepareQuery("insert into testtable1 values ('$(var1)','$(var2)','$(var3)')");
cur.substitutions(subvars,subvalstrings);
checkSuccess(cur.executeQuery(),1);
console.log("\n");
	
console.log("FIELDS: ");
checkSuccess(cur.sendQuery("select * from testtable1"),1);
checkSuccess(cur.getField(0,0),"hi");
checkSuccess(cur.getField(0,1),"hello");
checkSuccess(cur.getField(0,2),"bye");
checkSuccess(cur.sendQuery("delete from testtable1"),1);
console.log("\n");
	
	
console.log("ARRAY SUBSTITUTIONS: ");
cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
cur.substitutions(subvars,subvallongs);
checkSuccess(cur.executeQuery(),1);
console.log("\n");
	
console.log("FIELDS: ");
checkSuccess(cur.sendQuery("select * from testtable1"),1);
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(0,1),"2");
checkSuccess(cur.getField(0,2),"3.0");
checkSuccess(cur.sendQuery("delete from testtable1"),1);
console.log("\n");
	
	
console.log("ARRAY SUBSTITUTIONS: ");
cur.prepareQuery("insert into testtable1 values ($(var1),'$(var2)',$(var3))");
cur.substitutions(subvars,subvaldoubles,precs,scales);
checkSuccess(cur.executeQuery(),1);
console.log("\n");
	
console.log("FIELDS: ");
checkSuccess(cur.sendQuery("select * from testtable1"),1);
checkSuccess(cur.getField(0,0),"10.55");
checkSuccess(cur.getField(0,1),"10.556");
checkSuccess(cur.getField(0,2),"10.5556");
checkSuccess(cur.sendQuery("delete from testtable1"),1);
console.log("\n");
	
	
console.log("nullS as Nulls: ");
cur.getNullsAsNulls();
checkSuccess(cur.sendQuery("insert into testtable1 values (1,null,null)"),1);
checkSuccess(cur.sendQuery("select * from testtable1"),1);
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(0,1),null);
checkSuccess(cur.getField(0,2),null);
cur.getNullsAsEmptyStrings();
checkSuccess(cur.sendQuery("select * from testtable1"),1);
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(0,1),"");
checkSuccess(cur.getField(0,2),"");
cur.getNullsAsNulls();
console.log("\n");
	
console.log("RESULT SET BUFFER SIZE: ");
checkSuccess(cur.getResultSetBufferSize(),0);
cur.setResultSetBufferSize(2);
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
checkSuccess(cur.getResultSetBufferSize(),2);
console.log();
checkSuccess(cur.firstRowIndex(),0);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),2);
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(1,0),"2");
checkSuccess(cur.getField(2,0),"3");
console.log();
checkSuccess(cur.firstRowIndex(),2);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),4);
checkSuccess(cur.getField(6,0),"7");
checkSuccess(cur.getField(7,0),"8");
console.log();
checkSuccess(cur.firstRowIndex(),6);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),8);
checkSuccess(cur.getField(8,0),null);
console.log();
checkSuccess(cur.firstRowIndex(),8);
checkSuccess(cur.endOfResultSet(),1);
checkSuccess(cur.rowCount(),8);
console.log("\n");
	
console.log("DONT GET COLUMN INFO: ");
cur.dontGetColumnInfo();
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
checkSuccess(cur.getColumnName(0),null);
checkSuccess(cur.getColumnLength(0),0);
checkSuccess(cur.getColumnType(0),null);
cur.getColumnInfo();
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
checkSuccess(cur.getColumnName(0),"testint");
checkSuccess(cur.getColumnLength(0),0);
checkSuccess(cur.getColumnType(0),"INTEGER");
console.log("\n");
	
console.log("SUSPENDED SESSION: ");
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
cur.suspendResultSet();
checkSuccess(con.suspendSession(),1);
port=con.getConnectionPort();
socket=con.getConnectionSocket();
checkSuccess(con.resumeSession(port,socket),1);
console.log();
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(1,0),"2");
checkSuccess(cur.getField(2,0),"3");
checkSuccess(cur.getField(3,0),"4");
checkSuccess(cur.getField(4,0),"5");
checkSuccess(cur.getField(5,0),"6");
checkSuccess(cur.getField(6,0),"7");
checkSuccess(cur.getField(7,0),"8");
console.log();
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
cur.suspendResultSet();
checkSuccess(con.suspendSession(),1);
port=con.getConnectionPort();
socket=con.getConnectionSocket();
checkSuccess(con.resumeSession(port,socket),1);
console.log();
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(1,0),"2");
checkSuccess(cur.getField(2,0),"3");
checkSuccess(cur.getField(3,0),"4");
checkSuccess(cur.getField(4,0),"5");
checkSuccess(cur.getField(5,0),"6");
checkSuccess(cur.getField(6,0),"7");
checkSuccess(cur.getField(7,0),"8");
console.log();
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
cur.suspendResultSet();
checkSuccess(con.suspendSession(),1);
port=con.getConnectionPort();
socket=con.getConnectionSocket();
checkSuccess(con.resumeSession(port,socket),1);
console.log();
checkSuccess(cur.getField(0,0),"1");
checkSuccess(cur.getField(1,0),"2");
checkSuccess(cur.getField(2,0),"3");
checkSuccess(cur.getField(3,0),"4");
checkSuccess(cur.getField(4,0),"5");
checkSuccess(cur.getField(5,0),"6");
checkSuccess(cur.getField(6,0),"7");
checkSuccess(cur.getField(7,0),"8");
console.log("\n");
	
console.log("SUSPENDED RESULT SET: ");
cur.setResultSetBufferSize(2);
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
checkSuccess(cur.getField(2,0),"3");
id=cur.getResultSetId();
cur.suspendResultSet();
checkSuccess(con.suspendSession(),1);
port=con.getConnectionPort();
socket=con.getConnectionSocket();
checkSuccess(con.resumeSession(port,socket),1);
checkSuccess(cur.resumeResultSet(id),1);
console.log();
checkSuccess(cur.firstRowIndex(),4);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),6);
checkSuccess(cur.getField(7,0),"8");
console.log();
checkSuccess(cur.firstRowIndex(),6);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),8);
checkSuccess(cur.getField(8,0),null);
console.log();
checkSuccess(cur.firstRowIndex(),8);
checkSuccess(cur.endOfResultSet(),1);
checkSuccess(cur.rowCount(),8);
cur.setResultSetBufferSize(0);
console.log("\n");
	
console.log("CACHED RESULT SET: ");
cur.cacheToFile("cachefile1");
cur.setCacheTtl(200);
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
filename=cur.getCacheFileName();
checkSuccess(filename,"cachefile1");
cur.cacheOff();
checkSuccess(cur.openCachedResultSet(filename),1);
checkSuccess(cur.getField(7,0),"8");
console.log("\n");
	
console.log("COLUMN COUNT FOR CACHED RESULT SET: ");
checkSuccess(cur.colCount(),4);
console.log("\n");
	
console.log("COLUMN NAMES FOR CACHED RESULT SET: ");
checkSuccess(cur.getColumnName(0),"testint");
checkSuccess(cur.getColumnName(1),"testfloat");
checkSuccess(cur.getColumnName(2),"testchar");
checkSuccess(cur.getColumnName(3),"testvarchar");
cols=cur.getColumnNames();
checkSuccess(cols[0],"testint");
checkSuccess(cols[1],"testfloat");
checkSuccess(cols[2],"testchar");
checkSuccess(cols[3],"testvarchar");
console.log("\n");
	
console.log("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: ");
cur.setResultSetBufferSize(2);
cur.cacheToFile("cachefile1");
cur.setCacheTtl(200);
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
filename=cur.getCacheFileName();
checkSuccess(filename,"cachefile1");
cur.cacheOff();
checkSuccess(cur.openCachedResultSet(filename),1);
checkSuccess(cur.getField(7,0),"8");
checkSuccess(cur.getField(8,0),null);
cur.setResultSetBufferSize(0);
console.log("\n");
	
console.log("FROM ONE CACHE FILE TO ANOTHER: ");
cur.cacheToFile("cachefile2");
checkSuccess(cur.openCachedResultSet("cachefile1"),1);
cur.cacheOff();
checkSuccess(cur.openCachedResultSet("cachefile2"),1);
checkSuccess(cur.getField(7,0),"8");
checkSuccess(cur.getField(8,0),null);
console.log("\n");
	
console.log("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: ");
cur.setResultSetBufferSize(2);
cur.cacheToFile("cachefile2");
checkSuccess(cur.openCachedResultSet("cachefile1"),1);
cur.cacheOff();
checkSuccess(cur.openCachedResultSet("cachefile2"),1);
checkSuccess(cur.getField(7,0),"8");
checkSuccess(cur.getField(8,0),null);
cur.setResultSetBufferSize(0);
console.log("\n");
	
console.log("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: ");
cur.setResultSetBufferSize(2);
cur.cacheToFile("cachefile1");
cur.setCacheTtl(200);
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
checkSuccess(cur.getField(2,0),"3");
filename=cur.getCacheFileName();
checkSuccess(filename,"cachefile1");
id=cur.getResultSetId();
cur.suspendResultSet();
checkSuccess(con.suspendSession(),1);
port=con.getConnectionPort();
socket=con.getConnectionSocket();
console.log();
checkSuccess(con.resumeSession(port,socket),1);
checkSuccess(cur.resumeCachedResultSet(id,filename),1);
console.log();
checkSuccess(cur.firstRowIndex(),4);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),6);
checkSuccess(cur.getField(7,0),"8");
console.log();
checkSuccess(cur.firstRowIndex(),6);
checkSuccess(cur.endOfResultSet(),0);
checkSuccess(cur.rowCount(),8);
checkSuccess(cur.getField(8,0),null);
console.log();
checkSuccess(cur.firstRowIndex(),8);
checkSuccess(cur.endOfResultSet(),1);
checkSuccess(cur.rowCount(),8);
cur.cacheOff();
console.log();
checkSuccess(cur.openCachedResultSet(filename),1);
checkSuccess(cur.getField(7,0),"8");
checkSuccess(cur.getField(8,0),null);
cur.setResultSetBufferSize(0);
console.log("\n");

console.log("COMMIT AND ROLLBACK: \n");
var	secondcon=new sqlrelay.SQLRConnection("sqlrelay",
				9000,
				"/tmp/test.socket",
				"test","test",0,1);
var	secondcur=new sqlrelay.SQLRCursor(secondcon);
checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
checkSuccess(secondcur.getField(0,0),"0");
checkSuccess(con.commit(),1);
checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
checkSuccess(secondcur.getField(0,0),"8");
checkSuccess(cur.sendQuery("insert into testtable values (10,10.1,'testchar10','testvarchar10')"),1);
checkSuccess(secondcur.sendQuery("select count(*) from testtable"),1);
checkSuccess(secondcur.getField(0,0),"9");
console.log("\n");


console.log("FINISHED SUSPENDED SESSION: ");
checkSuccess(cur.sendQuery("select * from testtable order by testint"),1);
checkSuccess(cur.getField(4,0),"5");
checkSuccess(cur.getField(5,0),"6");
checkSuccess(cur.getField(6,0),"7");
checkSuccess(cur.getField(7,0),"8");
id=cur.getResultSetId();
cur.suspendResultSet();
checkSuccess(con.suspendSession(),1);
port=con.getConnectionPort();
socket=con.getConnectionSocket();
checkSuccess(con.resumeSession(port,socket),1);
checkSuccess(cur.resumeResultSet(id),1);
checkSuccess(cur.getField(4,0),null);
checkSuccess(cur.getField(5,0),null);
checkSuccess(cur.getField(6,0),null);
checkSuccess(cur.getField(7,0),null);
console.log("\n");
    	
// drop existing table
cur.sendQuery("drop table testtable");
	
// invalid queries...
console.log("INVALID QUERIES: ");
checkSuccess(cur.sendQuery("select * from testtable"),0);
checkSuccess(cur.sendQuery("select * from testtable"),0);
checkSuccess(cur.sendQuery("select * from testtable"),0);
checkSuccess(cur.sendQuery("select * from testtable"),0);
console.log();
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
checkSuccess(cur.sendQuery("insert into testtable values (1,2,3,4)"),0);
console.log();
checkSuccess(cur.sendQuery("create table testtable"),0);
checkSuccess(cur.sendQuery("create table testtable"),0);
checkSuccess(cur.sendQuery("create table testtable"),0);
checkSuccess(cur.sendQuery("create table testtable"),0);
console.log("\n");
