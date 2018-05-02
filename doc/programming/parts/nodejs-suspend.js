var	sqlrelay=require("sqlrelay");

var	con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
var	cur=new sqlrelay.SQLRCursor(con);

cur.sendQuery("insert into my_table values (1,2,3)");
cur.suspendResultSet();
con.suspendSession();
var     rs=cur.getResultSetId();
var     port=cur.getConnectionPort();
var     socket=cur.getConnectionSocket();

... pass the rs, port and socket to the next page ...
