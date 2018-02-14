var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

con.resumeSession(port,socket);
cur.resumeResultSet(rs);
cur.sendQuery("commit");
con.endSession();
