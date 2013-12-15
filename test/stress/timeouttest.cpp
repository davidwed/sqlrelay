#include <sqlrelay/sqlrclient.h>

int main() {

	sqlrconnection	sqlrcon("localhost",9000,"/tmp/test.socket",
							"test","test",0,1);
	sqlrcursor	sqlrcur(&sqlrcon);
	sqlrcon.setConnectTimeout(0,1);

	sqlrcon.debugOn();

	sqlrcur.sendQuery("select * from user_tables");
}
