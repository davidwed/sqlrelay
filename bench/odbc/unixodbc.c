#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

int main(int argc, char **argv) {

	SQLHENV		env;
	SQLHDBC		dbc;

	SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env);
	SQLAllocHandle(SQL_HANDLE_DBC,env,&dbc);
	SQLFreeHandle(SQL_HANDLE_DBC,dbc);
	SQLFreeHandle(SQL_HANDLE_ENV,env);
}
