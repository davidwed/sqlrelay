#include <sqlrelay/sqlrclient.h>
#include <rudiments/stdio.h>

using namespace rudiments;

int main() {

	sqlrconnection	sqlrcon("examplehost",9000,
				"/tmp/example.socket",
				"exampleuser",
				"examplepassword",0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	sqlrcur.sendQuery("select * from exampletable");
	for (uint64_t row=0; row<sqlrcur.rowCount(); row++) {
		for (uint64_t col=0; col<sqlrcur.colCount(); col++) {
			stdoutput.printf("%s,",sqlrcur.getField(row,col));
		}
		stdoutput.printf("\n");
	}
}
