var     sqlrelay=require("sqlrelay");

var     con=new sqlrelay.SQLRConnection("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);

... execute some queries ...
