#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>

#define NEED_DATATYPESTRING 1
#define NEED_IS_NUMBER_TYPE_CHAR 1
#define NEED_IS_UNSIGNED_TYPE_CHAR 1
#define NEED_IS_BINARY_TYPE_CHAR 1
#include <datatypes.h>

extern "C" {

#define CR_UNKNOWN_ERROR	2000

typedef unsigned long long	my_ulonglong;
typedef bool			my_bool;
typedef unsigned long long	MYSQL_ROW_OFFSET;
typedef unsigned int		MYSQL_FIELD_OFFSET;

enum enum_mysql_set_option { MYSQL_SET_OPTION_UNKNOWN_OPTION };
enum mysql_option { MYSQL_OPTION_UNKNOWN_OPTION };

// Taken directly from mysql_com.h version 5.0.0-alpha
// Back-compatible with all previous versions.
enum enum_field_types { MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY,
			MYSQL_TYPE_SHORT,  MYSQL_TYPE_LONG,
			MYSQL_TYPE_FLOAT,  MYSQL_TYPE_DOUBLE,
			MYSQL_TYPE_NULL,   MYSQL_TYPE_TIMESTAMP,
			MYSQL_TYPE_LONGLONG,MYSQL_TYPE_INT24,
			MYSQL_TYPE_DATE,   MYSQL_TYPE_TIME,
			MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
			MYSQL_TYPE_NEWDATE,
			MYSQL_TYPE_ENUM=247,
			MYSQL_TYPE_SET=248,
			MYSQL_TYPE_TINY_BLOB=249,
			MYSQL_TYPE_MEDIUM_BLOB=250,
			MYSQL_TYPE_LONG_BLOB=251,
			MYSQL_TYPE_BLOB=252,
			MYSQL_TYPE_VAR_STRING=253,
			MYSQL_TYPE_STRING=254,
			MYSQL_TYPE_GEOMETRY=255
};

#ifdef COMPAT_MYSQL_3
// taken directly from mysql.h - 3.23.58
struct MYSQL_FIELD {
  char *name;			/* Name of column */
  char *table;			/* Table of column if column was a field */
  char *def;			/* Default value (set by mysql_list_fields) */
  enum enum_field_types type;	/* Type of field. Se mysql_com.h for types */
  unsigned int length;		/* Width of column */
  unsigned int max_length;	/* Max width of selected set */
  unsigned int flags;		/* Div flags */
  unsigned int decimals;	/* Number of decimals in field */
};
#endif

#ifdef COMPAT_MYSQL_4_0
// taken directly from mysql.h - 4.0.17
struct MYSQL_FIELD {
  char *name;			/* Name of column */
  char *table;			/* Table of column if column was a field */
  char *org_table;		/* Org table name if table was an alias */
  char *db;			/* Database for table */
  char *def;			/* Default value (set by mysql_list_fields) */
  unsigned long length;		/* Width of column */
  unsigned long max_length;	/* Max width of selected set */
  unsigned int flags;		/* Div flags */
  unsigned int decimals;	/* Number of decimals in field */
  enum enum_field_types type;	/* Type of field. Se mysql_com.h for types */
};
#endif

#if defined(COMPAT_MYSQL_4_1) || defined(COMPAT_MYSQL_5_0)
// taken directly from mysql.h - 4.1.1-alpha (5.0.0-alpha is the same)
struct MYSQL_FIELD {
  char *name;                 /* Name of column */
  char *org_name;             /* Original column name, if an alias */ 
  char *table;                /* Table of column if column was a field */
  char *org_table;            /* Org table name, if table was an alias */
  char *db;                   /* Database for table */
  char *catalog;	      /* Catalog for table */
  char *def;                  /* Default value (set by mysql_list_fields) */
  unsigned long length;       /* Width of column */
  unsigned long max_length;   /* Max width of selected set */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;         /* Div flags */
  unsigned int decimals;      /* Number of decimals in field */
  unsigned int charsetnr;     /* Character set */
  enum enum_field_types type; /* Type of field. Se mysql_com.h for types */
};
#endif

// This is the same for all versions of mysql that I've ever seen
typedef char **MYSQL_ROW;

struct MYSQL_RES {
	sqlrcursor		*sqlrcur;
	unsigned int		errno;
	MYSQL_ROW_OFFSET	currentrow;
	MYSQL_FIELD_OFFSET	currentfield;
	MYSQL_FIELD		*fields;
};

struct MYSQL_STMT {
	MYSQL_RES	*result;
};

struct MYSQL {
	const char	*host;
	unsigned int	port;
	const char	*unix_socket;
	sqlrconnection	*sqlrcon;
	MYSQL_STMT	*currentstmt;
	bool		deleteonclose;
};

// taken directly from mysql.h - 5.0
struct MYSQL_BIND {
  unsigned long	*length;          /* output length pointer */
  my_bool       *is_null;	  /* Pointer to null indicators */
  char		*buffer;	  /* buffer to get/put data */
  enum enum_field_types buffer_type;	/* buffer type */
  unsigned long buffer_length;    /* buffer length, must be set for str/binary */  

  /* Following are for internal use. Set by mysql_bind_param */
  unsigned char *inter_buffer;    /* for the current data position */
  unsigned long offset;           /* offset position for char/binary fetch */
  unsigned long	internal_length;  /* Used if length is 0 */
  unsigned int	param_number;	  /* For null count and error messages */
  my_bool	long_data_used;	  /* If used with mysql_send_long_data */
  my_bool       binary_data;      /* data buffer is binary */
  my_bool       null_field;       /* NULL data cache flag */
  my_bool	internal_is_null; /* Used if is_null is 0 */
  void (*store_param_func);/*(NET *net, struct MYSQL_BIND *param);*/
  void (*fetch_result);/*(struct MYSQL_BIND *, unsigned char **row);*/
};




MYSQL *mysql_init(MYSQL *mysql);
int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option);
int mysql_options(MYSQL *mysql, enum mysql_option option, const char *arg);
int mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert,
			const char *ca, const char *capath,
			const char *cipher);
MYSQL *mysql_connect(MYSQL *mysql, const char *host,
			const char *user, const char *passwd);
MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user,
				const char *passwd, const char *db,
				unsigned int port, const char *unix_socket,
				unsigned long client_flag);
void mysql_close(MYSQL *mysql);
int mysql_ping(MYSQL *mysql);
char *mysql_stat(MYSQL *mysql);
int mysql_shutdown(MYSQL *mysql);
int mysql_reload(MYSQL *mysql);
unsigned long mysql_thread_id(MYSQL *mysql);
MYSQL_RES *mysql_list_processes(MYSQL *mysql);
int mysql_kill(MYSQL *mysql, unsigned long pid);
char *mysql_get_client_info();
unsigned long mysql_get_client_version();
char *mysql_get_host_info(MYSQL *mysql);
unsigned int mysql_get_proto_info(MYSQL *mysql);
char *mysql_get_server_info(MYSQL *mysql);
unsigned long mysql_get_server_version(MYSQL *mysql);
my_bool	mysql_change_user(MYSQL *mysql, const char *user,
				const char *password, const char *db);
const char *mysql_character_set_name(MYSQL *mysql);
void mysql_debug(const char *debug);
int mysql_dump_debug_info(MYSQL *mysql);
int mysql_create_db(MYSQL *mysql, const char *db);
int mysql_select_db(MYSQL *mysql, const char *db);
int mysql_drop_db(MYSQL *mysql, const char *db);
MYSQL_RES *mysql_list_dbs(MYSQL *mysql, const char *wild);
MYSQL_RES *mysql_list_tables(MYSQL *mysql, const char *wild);
unsigned long mysql_escape_string(char *to, const char *from,
					unsigned long length);
int mysql_query(MYSQL *mysql, const char *query);
unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,
					const char *from,
					unsigned long length);
int mysql_real_query(MYSQL *mysql, const char *query, unsigned long length);
char *mysql_info(MYSQL *mysql);
my_ulonglong mysql_insert_id(MYSQL *mysql);
MYSQL_RES *mysql_store_result(MYSQL *mysql);
MYSQL_RES *mysql_use_result(MYSQL *mysql);
void mysql_free_result(MYSQL_RES *result);
my_bool mysql_more_results(MYSQL *mysql);
int mysql_next_result(MYSQL *mysql);
MYSQL_RES *mysql_list_fields(MYSQL *mysql, const char *table,
						const char *wild);
unsigned int mysql_num_fields(MYSQL_RES *result);
MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result);
MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result);
MYSQL_FIELD *mysql_fetch_field_direct(MYSQL_RES *result, unsigned int fieldnr);
unsigned long *mysql_fetch_lengths(MYSQL_RES *result);
unsigned int mysql_field_count(MYSQL *mysql);
MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result,
					MYSQL_FIELD_OFFSET offset);
MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *result);
my_ulonglong mysql_num_rows(MYSQL_RES *result);
my_ulonglong mysql_affected_rows(MYSQL *mysql);
MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *result);
void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
my_bool mysql_eof(MYSQL_RES *result);
unsigned int mysql_warning_count(MYSQL *mysql);
unsigned int mysql_errno(MYSQL *mysql);
const char *mysql_error(MYSQL *mysql);
const char *mysql_sqlstate(MYSQL *mysql);
my_bool mysql_commit(MYSQL *mysql);
my_bool mysql_rollback(MYSQL *mysql);
my_bool mysql_autocommit(MYSQL *mysql, my_bool mode);
MYSQL_STMT *mysql_prepare(MYSQL *mysql, const char *query,
					unsigned long length);
my_bool mysql_bind_param(MYSQL_STMT *stmt, MYSQL_BIND *bind);
my_bool mysql_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *bind);
int mysql_execute(MYSQL_STMT *stmt);
unsigned long mysql_param_count(MYSQL_STMT *stmt);
MYSQL_RES *mysql_param_result(MYSQL_STMT *stmt);
int mysql_fetch(MYSQL_STMT *stmt);
int mysql_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind,
			unsigned int column, unsigned long offset);
MYSQL_RES *mysql_get_metadata(MYSQL_STMT *stmt);
my_bool mysql_send_long_data(MYSQL_STMT *stmt,
				unsigned int parameter_number,
				const char *data, unsigned long length);
my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt);
my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt);
MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,
					MYSQL_ROW_OFFSET offset);
MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt);
void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset);
my_bool mysql_stmt_close(MYSQL_STMT *stmt);
unsigned int mysql_stmt_errno(MYSQL_STMT *stmt);
const char *mysql_stmt_error(MYSQL_STMT *stmt);
const char *mysql_stmt_sqlstate(MYSQL_STMT *stmt);
int mysql_stmt_store_result(MYSQL_STMT *stmt);
my_bool mysql_stmt_free_result(MYSQL_STMT *stmt);
my_bool mysql_stmt_reset(MYSQL_STMT *stmt);






MYSQL *mysql_init(MYSQL *mysql) {
	if (mysql) {
		mysql->deleteonclose=false;
		return mysql;
	} else {
		MYSQL	*retval=new MYSQL;
		retval->deleteonclose=true;
		return retval;
	}
}

int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option) {
	return 0;
}

int mysql_options(MYSQL *mysql, enum mysql_option option, const char *arg) {
	return 0;
}

int mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert,
			const char *ca, const char *capath,
			const char *cipher) {
	return 0;
}

MYSQL *mysql_connect(MYSQL *mysql, const char *host,
			const char *user, const char *passwd) {
	return mysql_real_connect(mysql,host,user,passwd,NULL,9000,NULL,0);
}

MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user,
				const char *passwd, const char *db,
				unsigned int port, const char *unix_socket,
				unsigned long client_flag) {

	mysql->host=host;
	mysql->port=port;
	mysql->unix_socket=unix_socket;

	mysql->sqlrcon=new sqlrconnection(host,port,unix_socket,
						user,passwd,0,1);
	mysql->sqlrcon->copyReferences();
	mysql->currentstmt=NULL;
	return mysql;
}

void mysql_close(MYSQL *mysql) {
	if (mysql) {
		mysql_stmt_close(mysql->currentstmt);
		delete mysql->sqlrcon;
		if (mysql->deleteonclose) {
			delete mysql;
		}
	}
}


int mysql_ping(MYSQL *mysql) {
	return !mysql->sqlrcon->ping();
}

char *mysql_stat(MYSQL *mysql) {
	return "Uptime: 0  Threads: 0  Questions: 0  Slow queries: 0  Opens: 0  Flush tables: 0  Open tables: 0 Queries per second avg: 0.0";
}

int mysql_shutdown(MYSQL *mysql) {
	return 1;
}

int mysql_reload(MYSQL *mysql) {
	if (!strcmp(mysql->sqlrcon->identify(),"mysql")) {
		sqlrcursor	sqlrcur(mysql->sqlrcon);
		return !sqlrcur.sendQuery("FLUSH PRIVILEGES");
	}
	return 0;
}

unsigned long mysql_thread_id(MYSQL *mysql) {
	return 0;
}

MYSQL_RES *mysql_list_processes(MYSQL *mysql) {
	return NULL;
}

int mysql_kill(MYSQL *mysql, unsigned long pid) {
	return 1;
}



char *mysql_get_client_info() {
	// Returns a string that represents the client library version.
	return "";
}

unsigned long mysql_get_client_version() {
	// Returns an integer that represents the client library version.
	// The value has the format XYYZZ where X is the major version, YY is
	// the release level, and ZZ is the version number within the release
	// level. For example, a value of 40102 represents a client library
	// version of 4.1.2.
	return 40102;
}

char *mysql_get_host_info(MYSQL *mysql) {
	// Returns a string describing the type of connection in use,
	// including the server host name.
	return "";
}

unsigned int mysql_get_proto_info(MYSQL *mysql) {
	// Returns the protocol version used by current connection.
	return 0;
}

char *mysql_get_server_info(MYSQL *mysql) {
	// Returns a string that represents the server version number.
	return "";
}

unsigned long mysql_get_server_version(MYSQL *mysql) {
	// A number that represents the MySQL server version in format:
	// main_version*10000 + minor_version *100 + sub_version
	// For example, 4.1.0 is returned as 40100.
	return 40100;
}



my_bool	mysql_change_user(MYSQL *mysql, const char *user,
				const char *password, const char *db) {

	if (!mysql->sqlrcon->rollback()) {
		return false;
	}
	mysql_stmt_close(mysql->currentstmt);
	delete mysql->sqlrcon;
	return (mysql_real_connect(mysql,mysql->host,user,password,db,
				mysql->port,mysql->unix_socket,0))?true:false;
}

const char *mysql_character_set_name(MYSQL *mysql) {
	return "UTF-8";
}



void mysql_debug(const char *debug) {
	// don't do anything...
}

int mysql_dump_debug_info(MYSQL *mysql) {
	return 1;
}



int mysql_create_db(MYSQL *mysql, const char *db) {
	return 1;
}

int mysql_select_db(MYSQL *mysql, const char *db) {
	return 1;
}

int mysql_drop_db(MYSQL *mysql, const char *db) {
	return 1;
}

MYSQL_RES *mysql_list_dbs(MYSQL *mysql, const char *wild) {
	return NULL;
}

MYSQL_RES *mysql_list_tables(MYSQL *mysql, const char *wild) {
	return NULL;
}



unsigned long mysql_escape_string(char *to, const char *from,
					unsigned long length) {
	return mysql_real_escape_string(NULL,to,from,length);
}

int mysql_query(MYSQL *mysql, const char *query) {
	return mysql_real_query(mysql,query,strlen(query));
}

unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,
					const char *from,
					unsigned long length) {

	if (strcmp(mysql->sqlrcon->identify(),"mysql")) {
		memcpy(to,from,length);
		return length;
	}
	
	unsigned long	fromindex=0;
	unsigned long	toindex=0;
	while (fromindex<length) {
		if (from[fromindex]=='\'') {
			to[toindex++]='\\';
			to[toindex]='\'';
		} else if (from[fromindex]=='\"') {
			to[toindex++]='\\';
			to[toindex]='"';
		} else if (from[fromindex]=='\n') {
			to[toindex++]='\\';
			to[toindex]='n';
		} else if (from[fromindex]=='\r') {
			to[toindex++]='\\';
			to[toindex]='r';
		} else if (from[fromindex]=='\\') {
			to[toindex++]='\\';
			to[toindex]='\\';
		} else if (from[fromindex]!=';') {
			to[toindex]=from[fromindex];
		}
		toindex++;
		fromindex++;
	}
	return fromindex;
}

int mysql_real_query(MYSQL *mysql, const char *query, unsigned long length) {
	mysql->currentstmt=mysql_prepare(mysql,query,length);
	return mysql_execute(mysql->currentstmt);
}

char *mysql_info(MYSQL *mysql) {
	return "";
}

my_ulonglong mysql_insert_id(MYSQL *mysql) {
	return 0;
}


MYSQL_RES *mysql_store_result(MYSQL *mysql) {
	MYSQL_RES	*retval=mysql->currentstmt->result;
	mysql->currentstmt->result=NULL;
	return retval;
}

MYSQL_RES *mysql_use_result(MYSQL *mysql) {
	return mysql_store_result(mysql);
}

void mysql_free_result(MYSQL_RES *result) {
	if (result) {
		delete result->sqlrcur;
		delete[] result->fields;
		delete result;
	}
}

my_bool mysql_more_results(MYSQL *mysql) {
	return false;
}

int mysql_next_result(MYSQL *mysql) {
	return -1;
}


MYSQL_RES *mysql_list_fields(MYSQL *mysql, const char *table,
						const char *wild) {
	// FIXME:
	return NULL;
}

unsigned int mysql_num_fields(MYSQL_RES *result) {
	return result->sqlrcur->colCount();
}

MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result) {
	if (result->currentfield>=
		(MYSQL_FIELD_OFFSET)result->sqlrcur->colCount()) {
		return NULL;
	}
	return &result->fields[result->currentfield++];
}

MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result) {
	result->currentfield=0;
	return result->fields;
}

MYSQL_FIELD *mysql_fetch_field_direct(MYSQL_RES *result, unsigned int fieldnr) {
	if (fieldnr>(unsigned int)result->sqlrcur->colCount()) {
		return NULL;
	}
	// FIXME: it's possible that we shouldn't increment the fieldnr here
	result->currentfield=fieldnr+1;
	return &result->fields[fieldnr];
}

unsigned long *mysql_fetch_lengths(MYSQL_RES *result) {
	return (unsigned long *)result->sqlrcur->
				getRowLengths(result->currentrow);
}

unsigned int mysql_field_count(MYSQL *mysql) {
	return mysql->currentstmt->result->sqlrcur->colCount();
}

MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result,
					MYSQL_FIELD_OFFSET offset) {
	MYSQL_FIELD_OFFSET	oldoffset=result->currentfield;
	result->currentfield=offset;
	return oldoffset;
}

MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *result) {
	return result->currentfield;
}


my_ulonglong mysql_num_rows(MYSQL_RES *result) {
	return result->sqlrcur->rowCount();
}

my_ulonglong mysql_affected_rows(MYSQL *mysql) {
	return mysql->currentstmt->result->sqlrcur->affectedRows();
}

MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset) {
	my_ulonglong	oldrow=result->currentrow;
	result->currentrow=result->currentrow+offset;
	return oldrow;
}

MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *result) {
	return result->currentrow;
}

void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset) {
	result->currentrow=offset;
}

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result) {
	MYSQL_ROW	retval=result->sqlrcur->getRow(result->currentrow);
	if (retval) {
		result->currentrow++;
	}
	return retval;
}

my_bool mysql_eof(MYSQL_RES *result) {
	MYSQL_ROW_OFFSET	rowcount=
		(MYSQL_ROW_OFFSET)result->sqlrcur->rowCount();
	return (!rowcount || result->currentrow>=rowcount);
}


unsigned int mysql_warning_count(MYSQL *mysql) {
	return 0;
}

unsigned int mysql_errno(MYSQL *mysql) {
	return mysql->currentstmt->result->errno;
}

const char *mysql_error(MYSQL *mysql) {
	char	*err=mysql->currentstmt->result->sqlrcur->errorMessage();
	return (err)?err:"";
}

const char *mysql_sqlstate(MYSQL *mysql) {
	return "";
}


my_bool mysql_commit(MYSQL *mysql) {
	return mysql->sqlrcon->commit();
}

my_bool mysql_rollback(MYSQL *mysql) {
	return mysql->sqlrcon->rollback();
}

my_bool mysql_autocommit(MYSQL *mysql, my_bool mode) {
	return (mode)?mysql->sqlrcon->autoCommitOn():
				mysql->sqlrcon->autoCommitOff();
}







MYSQL_STMT *mysql_prepare(MYSQL *mysql, const char *query,
					unsigned long length) {
	MYSQL_STMT	*stmt=new MYSQL_STMT;
	stmt->result=new MYSQL_RES;
	stmt->result->sqlrcur=new sqlrcursor(mysql->sqlrcon);
	stmt->result->sqlrcur->copyReferences();
	stmt->result->errno=0;
	stmt->result->fields=NULL;
	stmt->result->sqlrcur->prepareQuery(query,length);
	return stmt;
}

my_bool mysql_bind_param(MYSQL_STMT *stmt, MYSQL_BIND *bind) {

	unsigned long	paramcount=mysql_param_count(stmt);
	for (unsigned long i=0; i<paramcount; i++) {

		char		*variable=charstring::parseNumber((long)i);
		sqlrcursor	*cursor=stmt->result->sqlrcur;
		switch (bind[i].buffer_type) {
			case MYSQL_TYPE_NULL: {
				cursor->inputBind(variable,(char *)NULL);
				break;
			}
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_TIMESTAMP:
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_NEWDATE: {
				char	*value=(char *)bind[i].buffer;
				cursor->inputBind(variable,value);
				break;
			}
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DOUBLE: {
				double	value=*((double *)bind[i].buffer);
				// FIXME: precision/scale???
				cursor->inputBind(variable,value,0,0);
				break;
			}
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_YEAR: {
				long	value=*((long *)bind[i].buffer);
				cursor->inputBind(variable,value);
				break;
			}
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_INT24: {
				// FIXME: what should I do here?
				return false;
				break;
			}
			case MYSQL_TYPE_TINY_BLOB:
			case MYSQL_TYPE_MEDIUM_BLOB:
			case MYSQL_TYPE_LONG_BLOB:
			case MYSQL_TYPE_BLOB: {
				char		*value=(char *)bind[i].buffer;
				unsigned long	size=*(bind[i].length);
				cursor->inputBindBlob(variable,value,size);
				break;
			}
			case MYSQL_TYPE_ENUM:
			case MYSQL_TYPE_SET:
			case MYSQL_TYPE_GEOMETRY: {
				// FIXME: what should I do here?
				return false;
				break;
			}
			default: {
				// FIXME: what should I do here?
				return false;
			}
		}
	}
	return true;
}

my_bool mysql_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *bind) {
	// FIXME:
	return false;
}

static enum enum_field_types	mysqltypemap[]={
	// "UNKNOWN"
	MYSQL_TYPE_NULL,
	// addded by freetds
	// "CHAR"
	MYSQL_TYPE_STRING,
	// "INT"
	MYSQL_TYPE_LONG,
	// "SMALLINT"
	MYSQL_TYPE_SHORT,
	// "TINYINT"
	MYSQL_TYPE_TINY,
	// "MONEY"
	MYSQL_TYPE_DECIMAL,
	// "DATETIME"
	MYSQL_TYPE_DATETIME,
	// "NUMERIC"
	MYSQL_TYPE_DECIMAL,
	// "DECIMAL"
	MYSQL_TYPE_DECIMAL,
	// "SMALLDATETIME"
	MYSQL_TYPE_DATETIME,
	// "SMALLMONEY"
	MYSQL_TYPE_DECIMAL,
	// "IMAGE"
	MYSQL_TYPE_BLOB,
	// "BINARY"
	MYSQL_TYPE_BLOB,
	// "BIT"
	MYSQL_TYPE_TINY,
	// "REAL"
	MYSQL_TYPE_DECIMAL,
	// "FLOAT"
	MYSQL_TYPE_FLOAT,
	// "TEXT"
	MYSQL_TYPE_STRING,
	// "VARCHAR"
	MYSQL_TYPE_VAR_STRING,
	// "VARBINARY"
	MYSQL_TYPE_BLOB,
	// "LONGCHAR"
	MYSQL_TYPE_BLOB,
	// "LONGBINARY"
	MYSQL_TYPE_BLOB,
	// "LONG"
	MYSQL_TYPE_BLOB,
	// "ILLEGAL"
	MYSQL_TYPE_NULL,
	// "SENSITIVITY"
	MYSQL_TYPE_STRING,
	// "BOUNDARY"
	MYSQL_TYPE_STRING,
	// "VOID"
	MYSQL_TYPE_NULL,
	// "USHORT"
	MYSQL_TYPE_SHORT,
	// added by lago
	// "UNDEFINED"
	MYSQL_TYPE_NULL,
	// "DOUBLE"
	MYSQL_TYPE_DOUBLE,
	// "DATE"
	MYSQL_TYPE_DATETIME,
	// "TIME"
	MYSQL_TYPE_DATETIME,
	// "TIMESTAMP"
	MYSQL_TYPE_TIMESTAMP,
	// added by msql
	// "UINT"
	MYSQL_TYPE_LONG,
	// "LASTREAL"
	MYSQL_TYPE_DECIMAL,
	// added by mysql
	// "STRING"
	MYSQL_TYPE_STRING,
	// "VARSTRING"
	MYSQL_TYPE_VAR_STRING,
	// "LONGLONG"
	MYSQL_TYPE_LONGLONG,
	// "MEDIUMINT"
	MYSQL_TYPE_INT24,
	// "YEAR"
	MYSQL_TYPE_YEAR,
	// "NEWDATE"
	MYSQL_TYPE_NEWDATE,
	// "NULL"
	MYSQL_TYPE_NULL,
	// "ENUM"
	MYSQL_TYPE_ENUM,
	// "SET"
	MYSQL_TYPE_SET,
	// "TINYBLOB"
	MYSQL_TYPE_TINY_BLOB,
	// "MEDIUMBLOB"
	MYSQL_TYPE_MEDIUM_BLOB,
	// "LONGBLOB"
	MYSQL_TYPE_LONG_BLOB,
	// "BLOB"
	MYSQL_TYPE_BLOB,
	// added by oracle
	// "VARCHAR2"
	MYSQL_TYPE_VAR_STRING,
	// "NUMBER"
	MYSQL_TYPE_DECIMAL,
	// "ROWID"
	MYSQL_TYPE_LONGLONG,
	// "RAW"
	MYSQL_TYPE_BLOB,
	// "LONG_RAW"
	MYSQL_TYPE_BLOB,
	// "MLSLABEL"
	MYSQL_TYPE_BLOB,
	// "CLOB"
	MYSQL_TYPE_BLOB,
	// "BFILE"
	MYSQL_TYPE_BLOB,
	// added by odbc
	// "BIGINT"
	MYSQL_TYPE_LONGLONG,
	// "INTEGER"
	MYSQL_TYPE_LONG,
	// "LONGVARBINARY"
	MYSQL_TYPE_BLOB,
	// "LONGVARCHAR"
	MYSQL_TYPE_BLOB,
	// added by db2
	// "GRAPHIC"
	MYSQL_TYPE_BLOB,
	// "VARGRAPHIC"
	MYSQL_TYPE_BLOB,
	// "LONGVARGRAPHIC"
	MYSQL_TYPE_BLOB,
	// "DBCLOB"
	MYSQL_TYPE_BLOB,
	// "DATALINK"
	MYSQL_TYPE_STRING,
	// "USER_DEFINED_TYPE"
	MYSQL_TYPE_STRING,
	// "SHORT_DATATYPE"
	MYSQL_TYPE_SHORT,
	// "TINY_DATATYPE"
	MYSQL_TYPE_TINY,
	// added by interbase
	// "D_FLOAT"
	MYSQL_TYPE_DOUBLE,
	// "ARRAY"
	MYSQL_TYPE_SET,
	// "QUAD"
	MYSQL_TYPE_SET,
	// "INT64"
	MYSQL_TYPE_LONGLONG,
	// "DOUBLE PRECISION"
	MYSQL_TYPE_DOUBLE
};

enum enum_field_types map_col_type(const char *columntype) {
	for (int index=0; datatypestring[index]; index++) {
		if (!strcmp(datatypestring[index],columntype)) {
			return mysqltypemap[index];
		}
	}
	return MYSQL_TYPE_NULL;
}

int mysql_execute(MYSQL_STMT *stmt) {

	stmt->result->currentrow=0;
	stmt->result->currentfield=0;
	sqlrcursor	*sqlrcur=stmt->result->sqlrcur;

	int	retval=!sqlrcur->executeQuery();

	delete[] stmt->result->fields;

	int	colcount=sqlrcur->colCount();
	if (colcount) {

		MYSQL_FIELD	*fields=new MYSQL_FIELD[colcount];
		stmt->result->fields=fields;

		for (int i=0; i<colcount; i++) {

			fields[i].name=sqlrcur->getColumnName(i);
			fields[i].table="";
			fields[i].def="";

			#if defined(COMPAT_MYSQL_4_0) || \
				defined(COMPAT_MYSQL_4_1) || \
				defined(COMPAT_MYSQL_5_0)
  			fields[i].org_table="";
  			fields[i].db="";
			#if defined(COMPAT_MYSQL_4_1) || \
				defined(COMPAT_MYSQL_5_0)
  			fields[i].catalog="";
  			fields[i].org_name=sqlrcur->getColumnName(i);
			fields[i].name_length=strlen(fields[i].name);
			fields[i].org_name_length=strlen(fields[i].org_name);
			fields[i].table_length=strlen(fields[i].table);
			fields[i].org_table_length=strlen(fields[i].org_table);
			fields[i].db_length=strlen(fields[i].db);
			fields[i].catalog_length=strlen(fields[i].catalog);
			fields[i].def_length=strlen(fields[i].def);
			// FIXME: need a character set number here
			fields[i].charsetnr=0;
			#endif
			#endif

			// figure out the column type
			char	*columntypestring=sqlrcur->getColumnType(i);
			enum enum_field_types	columntype=
					map_col_type(columntypestring);
			fields[i].type=columntype;

			fields[i].length=sqlrcur->getColumnLength(i);
			fields[i].max_length=sqlrcur->getLongest(i);

			// figure out the flags
			unsigned int	flags=0;
			if (sqlrcur->getColumnIsNullable(i)) {
				#define NOT_NULL_FLAG	1
				flags|=NOT_NULL_FLAG;
			}
			if (sqlrcur->getColumnIsPrimaryKey(i)) {
				#define PRI_KEY_FLAG	2
				flags|=PRI_KEY_FLAG;
			}
			if (sqlrcur->getColumnIsUnique(i)) {
				#define UNIQUE_KEY_FLAG 4
				flags|=UNIQUE_KEY_FLAG;
			}
			if (sqlrcur->getColumnIsPartOfKey(i)) {
				#define MULTIPLE_KEY_FLAG 8
				flags|=MULTIPLE_KEY_FLAG;
			}
			if (columntype==MYSQL_TYPE_TINY_BLOB ||
				columntype==MYSQL_TYPE_MEDIUM_BLOB ||
				columntype==MYSQL_TYPE_LONG_BLOB ||
				columntype==MYSQL_TYPE_BLOB) {
				#define BLOB_FLAG	16
				flags|=BLOB_FLAG;
			}
			if (sqlrcur->getColumnIsUnsigned(i) ||
				isUnsignedTypeChar(columntypestring)) {
				#define UNSIGNED_FLAG	32
				flags|=UNSIGNED_FLAG;
			}
			if (sqlrcur->getColumnIsZeroFilled(i)) {
				#define ZEROFILL_FLAG	64
				flags|=ZEROFILL_FLAG;
			}
			if (sqlrcur->getColumnIsBinary(i) ||
				isBinaryTypeChar(columntypestring)) {
				#define BINARY_FLAG	128
				flags|=BINARY_FLAG;
			}
			if (columntype==MYSQL_TYPE_ENUM) {
				#define ENUM_FLAG	256
				flags|=ENUM_FLAG;
			}
			if (sqlrcur->getColumnIsAutoIncrement(i)) {
				#define AUTO_INCREMENT_FLAG 512
				flags|=AUTO_INCREMENT_FLAG;
			}
			if (columntype==MYSQL_TYPE_TIMESTAMP) {
				#define TIMESTAMP_FLAG	1024
				flags|=TIMESTAMP_FLAG;
			}
			if (columntype==MYSQL_TYPE_SET) {
				#define SET_FLAG	2048
				flags|=SET_FLAG;
			}
			if (isNumberTypeChar(columntypestring)) {
				#define NUM_FLAG	32768
				flags|=NUM_FLAG;
			}
			// Presumably these don't matter...
			// Intern; Part of some key
			//#define PART_KEY_FLAG	16384
			// Intern: Group field
			//#define GROUP_FLAG	32768
			// Intern: Used by sql_yacc
			//#define UNIQUE_FLAG	65536
			fields[i].flags=flags;

			fields[i].decimals=sqlrcur->getColumnPrecision(i);
		}
	} else {
		stmt->result->fields=NULL;
	}

	return retval;
}

unsigned long mysql_param_count(MYSQL_STMT *stmt) {
	// FIXME: really need this one
	return 0;
}

MYSQL_RES *mysql_param_result(MYSQL_STMT *stmt) {
	// FIXME: The MySQL docs don't even explain this one
	return NULL;
}



int mysql_fetch(MYSQL_STMT *stmt) {
	return 0;
}

int mysql_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind,
			unsigned int column, unsigned long offset) {
	// FIXME: The MySQL docs don't even explain this one
	return 0;
}



MYSQL_RES *mysql_get_metadata(MYSQL_STMT *stmt) {
	return stmt->result;
}



my_bool mysql_send_long_data(MYSQL_STMT *stmt,
				unsigned int parameter_number,
				const char *data, unsigned long length) {
	return false;
}



my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt) {
	return stmt->result->sqlrcur->rowCount();
}

my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt) {
	return stmt->result->sqlrcur->affectedRows();
}

MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,
					MYSQL_ROW_OFFSET offset) {
	MYSQL_ROW_OFFSET	oldrow=stmt->result->currentrow;
	stmt->result->currentrow=stmt->result->currentrow+offset;
	return oldrow;
}

MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt) {
	return stmt->result->currentrow;
}

void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset) {
	stmt->result->currentrow=offset;
}



my_bool mysql_stmt_close(MYSQL_STMT *stmt) {
	if (stmt) {
		mysql_free_result(stmt->result);
		delete stmt;
	}
	return true;
}



unsigned int mysql_stmt_errno(MYSQL_STMT *stmt) {
	return stmt->result->errno;
}

const char *mysql_stmt_error(MYSQL_STMT *stmt) {
	return stmt->result->sqlrcur->errorMessage();
}

const char *mysql_stmt_sqlstate(MYSQL_STMT *stmt) {
	return "";
}



int mysql_stmt_store_result(MYSQL_STMT *stmt) {
	return 0;
}

my_bool mysql_stmt_free_result(MYSQL_STMT *stmt) {
	mysql_free_result(stmt->result);
	return true;
}



my_bool mysql_stmt_reset(MYSQL_STMT *stmt) {
	stmt->result->sqlrcur->clearBinds();
	return true;
}

}
