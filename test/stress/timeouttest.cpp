#include <sqlrelay/sqlrclient.h>
#include <stdio.h>

main() {

	sqlrconnection	sqlrcon("localhost",8009,"",
				"oracle8test","oracle8test",0,1);
	sqlrcursor	sqlrcur(&sqlrcon);
	sqlrcon.setConnectTimeout(0,1);

	sqlrcon.debugOn();

	sqlrcur.sendQuery("select * from user_tables");
}
