#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <stdio.h>

main() {

	datetime	dt;
	dt.getSystemDateAndTime();

	int	count=dt.getEpoch();
	for (;;) {

		sqlrconnection	sqlrcon("localhost",8010,"",
					"postgresqltest","postgresqltest",0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		count=randomnumber::generateScaledNumber(count,1,50);
								
		printf("looping %d times\n",count);
		for (int i=0; i<count; i++) {
			if (!sqlrcur.sendQuery("select * from testtable")) {
				printf("error: %s\n",sqlrcur.errorMessage());
				exit(0);
			}
		}
	}
}
