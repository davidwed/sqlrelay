#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <rudiments/stdio.h>

int main() {

	datetime	dt;
	dt.getSystemDateAndTime();

	sqlrconnection	sqlrcon("localhost",9000,"/tmp/test.socket",
							"test","test",0,1);
	sqlrcursor	sqlrcur1(&sqlrcon);
	sqlrcursor	sqlrcur2(&sqlrcon);

	sqlrcon.debugOn();

	sqlrcur1.setResultSetBufferSize(1);
	sqlrcur2.setResultSetBufferSize(1);

	sqlrcur1.sendQuery("select * from user_tables");

	stdoutput.printf("running second query\n");
	sqlrcur2.sendQuery("select * from user_tables");

	uint64_t	i=0;
	uint32_t	cols2=sqlrcur2.colCount();
	const char	*field="";
	while (cols2 && field) {
		for (uint32_t j=0; j<cols2; j++) {
			if ((field=sqlrcur2.getField(i,j))) {
				stdoutput.printf("\"%s\"",field);
				if (j<cols2-1) {
					stdoutput.printf(",");
				} else {
					stdoutput.printf("\n");
				}
			}
		}
		i++;
	}
}
