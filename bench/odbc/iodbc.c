#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

int main(int argc, char **argv) {

	SQLHENV		env;
	SQLHDBC		dbc;

	SQLAllocEnv(&env);
	SQLAllocConnect(env,&dbc);
	SQLFreeConnect(&dbc);
	SQLFreeEnv(&env);
}
