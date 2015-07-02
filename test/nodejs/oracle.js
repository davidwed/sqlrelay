var	sqlrelay=require("../../src/api/nodejs/build/Release/sqlrelay");

var	sqlrcon=new sqlrelay.SQLRConnection();
var	sqlrcur=new sqlrelay.SQLRCursor(sqlrcon);
console.log();
console.log("ping: "+sqlrcon.ping());
console.log("sendQuery: "+sqlrcur.sendQuery("select 1"));
