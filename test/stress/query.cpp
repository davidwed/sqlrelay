#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <rudiments/stdio.h>

int main() {
	//sqlrconnection	sqlrcon("localhost",9000,"","test","test",0,1);
	//sqlrcursor	sqlrcur(&sqlrcon);

	//datetime	dt;
	//dt.getSystemDateAndTime();

	//int	seed=dt.getEpoch();
	int	loop=0;
	for (;;) {

		sqlrconnection	sqlrcon("localhost",9000,"",
					"test","test",0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		//seed=randomnumber::generateNumber(seed);
		//int	count=randomnumber::scaleNumber(seed,1,50);
		int	count=1;
								
		stdoutput.printf("looping %d times\n",count);
		for (int i=0; i<count; i++) {
			if (!sqlrcur.sendQuery("select * from user_tables")) {
				stdoutput.printf("error: %s\n",
						sqlrcur.errorMessage());
			}
		}
		stdoutput.printf("%d\n",loop);
		loop++;
	}
}
