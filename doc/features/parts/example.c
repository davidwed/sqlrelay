#include <sqlrelay/sqlrclientwrapper.h>
#include <stdio.h>

int main() {

	sqlrcon	sqlrconref=sqlrcon_alloc("examplehost",9000,
						"/tmp/example.socket",
						"exampleuser",
						"examplepassword",0,1);
	sqlrcur	sqlrcurref=sqlrcur_alloc(sqlrconref);

	sqlrcur_sendQuery(sqlrcurref,"select * from exampletable");
	uint64_t	row;
	uint64_t	col;
	for (row=0; row<sqlrcur_rowCount(sqlrcurref); row++) {
		for (col=0; col<sqlrcur_colCount(sqlrcurref); col++) {
			printf("%s,",sqlrcur_getFieldByIndex(sqlrcurref,row,col));
		}
		printf("\n");
	}

	sqlrcur_free(sqlrcurref);
	sqlrcon_free(sqlrconref);
}
