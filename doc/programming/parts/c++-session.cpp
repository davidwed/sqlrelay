#include <sqlrelay/sqlrclient.h>

main() {

       sqlrconnection      *con=new sqlrconnection("sqlrserver",9000,"/tmp/test.socket","user","password",0,1);

       ... execute some queries ...

       delete con;
}
