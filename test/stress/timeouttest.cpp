#include <sqlrelay/sqlrclient.h>

int main() {

	sqlrconnection	sqlrcon("sqlrserver",9000,"/tmp/test.socket",
							"test","test",0,1);
	sqlrcursor	sqlrcur(&sqlrcon);
	sqlrcon.setConnectTimeout(0,1);

	sqlrcon.debugOn();

	sqlrcur.sendQuery("select * from user_tables");
}
