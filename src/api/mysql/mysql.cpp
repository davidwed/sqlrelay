// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/character.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
#include <rudiments/dictionary.h>
#include <rudiments/file.h>
#include <rudiments/memorypool.h>
#include <rudiments/stdio.h>
//#define DEBUG_MESSAGES
#include <rudiments/debugprint.h>
#include <locale.h>

#define NEED_DATATYPESTRING 1
#define NEED_IS_NUMBER_TYPE_CHAR 1
#define NEED_IS_UNSIGNED_TYPE_CHAR 1
#define NEED_IS_BINARY_TYPE_CHAR 1
#include <datatypes.h>
#include <defines.h>

extern "C" {

#define CR_UNKNOWN_ERROR	2000
#define MYSQL_NO_DATA		100
#define REFRESH_GRANT		1
#define REFRESH_LOG		2
#define REFRESH_TABLES		4
#define REFRESH_HOSTS		8
#define REFRESH_STATUS		16
#define REFRESH_THREADS		32
#define REFRESH_SLAVE		64
#define REFRESH_MASTER		128

typedef unsigned long long	my_ulonglong;
typedef bool			my_bool;
typedef my_ulonglong *		MYSQL_ROW_OFFSET;
typedef unsigned int		MYSQL_FIELD_OFFSET;

enum enum_mysql_set_option {
	MYSQL_SET_OPTION_UNKNOWN_OPTION
};

enum mysql_option {
	MYSQL_OPTION_UNKNOWN_OPTION
};

// Taken directly from mysql_com.h version 5.something
// Back-compatible with all previous versions.
enum enum_field_types {
	MYSQL_TYPE_DECIMAL,
	MYSQL_TYPE_TINY,
	MYSQL_TYPE_SHORT,
	MYSQL_TYPE_LONG,
	MYSQL_TYPE_FLOAT,
	MYSQL_TYPE_DOUBLE,
	MYSQL_TYPE_NULL,
	MYSQL_TYPE_TIMESTAMP,
	MYSQL_TYPE_LONGLONG,
	MYSQL_TYPE_INT24,
	MYSQL_TYPE_DATE,
	MYSQL_TYPE_TIME,
	MYSQL_TYPE_DATETIME,
	MYSQL_TYPE_YEAR,
	MYSQL_TYPE_NEWDATE,
	MYSQL_TYPE_VARCHAR,
	MYSQL_TYPE_BIT,
	MYSQL_TYPE_TIMESTAMP2,
	MYSQL_TYPE_DATETIME2,
	MYSQL_TYPE_TIME2,
	MYSQL_TYPE_NEWDECIMAL=246,
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

enum enum_stmt_attr_type {
	STMT_ATTR_UPDATE_MAX_LENGTH,
	STMT_ATTR_CURSOR_TYPE,
	STMT_ATTR_PREFETCH_ROWS
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

#if defined(COMPAT_MYSQL_4_1) || \
	defined(COMPAT_MYSQL_5_0) || \
	defined(COMPAT_MYSQL_5_1)
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
  enum enum_field_types type; /* Type of field. See mysql_com.h for types */
#ifdef COMPAT_MYSQL_5_1
  // taken directly from mysql.h - 5.1.22
  void *extension;
#endif
};
#endif

#ifdef COMPAT_MYSQL_5_1
// taken from mysql.h - 5.1.22, modified
typedef struct st_mysql_bind {
  unsigned long	*length;          /* output length pointer */
  my_bool       *is_null;	  /* Pointer to null indicator */
  //void		*buffer;	  /* buffer to get/put data */
  char		*buffer;
  /* set this if you want to track data truncations happened during fetch */
  my_bool       *error;
  unsigned char *row_ptr;         /* for the current data position */
  //void (*store_param_func)(NET *net, struct st_mysql_bind *param);
  void (*store_param_func)(void *net, struct st_mysql_bind *param);
  void (*fetch_result)(struct st_mysql_bind *, MYSQL_FIELD *,
                       unsigned char **row);
  void (*skip_result)(struct st_mysql_bind *, MYSQL_FIELD *,
		      unsigned char **row);
  /* output buffer length, must be set when fetching str/binary */
  unsigned long buffer_length;
  unsigned long offset;           /* offset position for char/binary fetch */
  unsigned long	length_value;     /* Used if length is 0 */
  unsigned int	param_number;	  /* For null count and error messages */
  unsigned int  pack_length;	  /* Internal length for packed data */
  enum enum_field_types buffer_type;	/* buffer type */
  my_bool       error_value;      /* used if error is 0 */
  my_bool       is_unsigned;      /* set if integer type is unsigned */
  my_bool	long_data_used;	  /* If used with mysql_send_long_data */
  my_bool	is_null_value;    /* Used if is_null is 0 */
  void *extension;
} MYSQL_BIND;
#else
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
  void *store_param_func;/*(NET *net, struct MYSQL_BIND *param);*/
  void *fetch_result;/*(struct MYSQL_BIND *, unsigned char **row);*/
};
#endif

struct MYSQL_PARAMETERS {
	unsigned long *p_max_allowed_packet;
	unsigned long *p_net_buffer_length;
};

enum enum_mysql_timestamp_type {
	MYSQL_TIMESTAMP_NONE=-2,
	MYSQL_TIMESTAMP_ERROR=-1,
	MYSQL_TIMESTAMP_DATE=0,
	MYSQL_TIMESTAMP_DATETIME=1,
	MYSQL_TIMESTAMP_TIME=2
};

struct MYSQL_TIME {
	unsigned int	year;
	unsigned int	month;
	unsigned int	day;
	unsigned int	hour;
	unsigned int	minute;
	unsigned int	second;
	unsigned long	second_part;
	my_bool		neg;
	enum enum_mysql_timestamp_type	time_type;
};

// This is the same for all versions of mysql that I've ever seen
typedef char **MYSQL_ROW;

struct MYSQL_STMT;

struct MYSQL_RES {
	sqlrcursor		*sqlrcur;
	// don't call this errno, some systems have a macro for errno
	// which will get substituted in for all errno references and give
	// undesirable results
	unsigned int		errorno;
	my_ulonglong		previousrow;
	my_ulonglong		currentrow;
	uint32_t		fieldcount;
	MYSQL_FIELD_OFFSET	currentfield;
	MYSQL_FIELD		*fields;
	unsigned long		*lengths;
	MYSQL_STMT		*stmtbackptr;
	linkedlist< my_ulonglong >	rowcache;
};

struct MYSQL;

struct MYSQL_STMT {
	MYSQL_RES	*result;
	MYSQL_BIND	*resultbinds;
	MYSQL		*mysql;
	memorypool	bindvarnames;
};

struct MYSQL {
	const char	*host;
	unsigned int	port;
	const char	*unix_socket;
	sqlrconnection	*sqlrcon;
	MYSQL_STMT	*currentstmt;
	bool		deleteonclose;
	char		*error;
	int		errorno;
	dictionary< int64_t, unsigned int >	*errormap;
	bool		backendchecked;
	bool		backendismysql;
};

#define NOT_NULL_FLAG		1
#define PRI_KEY_FLAG		2
#define UNIQUE_KEY_FLAG		4
#define MULTIPLE_KEY_FLAG	8
#define BLOB_FLAG		16
#define UNSIGNED_FLAG		32
#define ZEROFILL_FLAG		64
#define BINARY_FLAG		128
#define ENUM_FLAG		256
#define AUTO_INCREMENT_FLAG	512
#define TIMESTAMP_FLAG		1024
#define ON_UPDATE_NOW_FLAG	8192
#define SET_FLAG		2048
#define NUM_FLAG		32768


unsigned int mysql_thread_safe();
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
const char *mysql_stat(MYSQL *mysql);
int mysql_shutdown(MYSQL *mysql);
int mysql_reload(MYSQL *mysql);
int mysql_refresh(MYSQL *mysql, unsigned int refresh_options);
unsigned long mysql_thread_id(MYSQL *mysql);
MYSQL_RES *mysql_list_processes(MYSQL *mysql);
int mysql_kill(MYSQL *mysql, unsigned long pid);
const char *mysql_get_client_info();
unsigned long mysql_get_client_version();
const char *mysql_get_host_info(MYSQL *mysql);
unsigned int mysql_get_proto_info(MYSQL *mysql);
const char *mysql_get_server_info(MYSQL *mysql);
unsigned long mysql_get_server_version(MYSQL *mysql);
my_bool	mysql_change_user(MYSQL *mysql, const char *user,
				const char *password, const char *db);
const char *mysql_character_set_name(MYSQL *mysql);
int mysql_set_character_set(MYSQL *mysql, const char *csname);
void mysql_debug(const char *debug);
int mysql_dump_debug_info(MYSQL *mysql);
int mysql_create_db(MYSQL *mysql, const char *db);
int mysql_select_db(MYSQL *mysql, const char *db);
int mysql_drop_db(MYSQL *mysql, const char *db);
MYSQL_RES *mysql_list_dbs(MYSQL *mysql, const char *wild);
MYSQL_RES *mysql_list_tables(MYSQL *mysql, const char *wild);
MYSQL_RES *mysql_list_fields(MYSQL *mysql, const char *table, const char *wild);
unsigned long mysql_escape_string(char *to, const char *from,
					unsigned long length);
char *mysql_odbc_escape_string(MYSQL *mysql, char *to,
				unsigned long to_length,
				const char *from,
				unsigned long from_length,
				void *param,
				char *(*extend_buffer)
					(void *, char *to,
					unsigned long *length));
void myodbc_remove_escape(MYSQL *mysql, char *name);
int mysql_query(MYSQL *mysql, const char *query);
int mysql_send_query(MYSQL *mysql, const char *query, unsigned int length);
int mysql_read_query_result(MYSQL *mysql);
unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,
					const char *from,
					unsigned long length);
int mysql_real_query(MYSQL *mysql, const char *query, unsigned long length);
const char *mysql_info(MYSQL *mysql);
my_ulonglong mysql_insert_id(MYSQL *mysql);
MYSQL_RES *mysql_store_result(MYSQL *mysql);
MYSQL_RES *mysql_use_result(MYSQL *mysql);
void mysql_free_result(MYSQL_RES *result);
my_bool mysql_more_results(MYSQL *mysql);
int mysql_next_result(MYSQL *mysql);
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
static void processFields(MYSQL_STMT *stmt);
enum enum_field_types map_col_type(const char *columntype, int64_t scale);
unsigned long mysql_param_count(MYSQL_STMT *stmt);
MYSQL_RES *mysql_param_result(MYSQL_STMT *stmt);
int mysql_fetch(MYSQL_STMT *stmt);
int mysql_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind,
			unsigned int column, unsigned long offset);
MYSQL_RES *mysql_get_metadata(MYSQL_STMT *stmt);
my_bool mysql_send_long_data(MYSQL_STMT *stmt,
				unsigned int parameternumber,
				const char *data, unsigned long length);

MYSQL_STMT *mysql_stmt_init(MYSQL *mysql);
int mysql_stmt_prepare(MYSQL_STMT *stmt,
				const char *query,
				unsigned long length);
int mysql_stmt_execute(MYSQL_STMT *stmt);
int mysql_stmt_fetch(MYSQL_STMT *stmt);
int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bindarg, 
                                    unsigned int column,
                                    unsigned long offset);
int mysql_stmt_store_result(MYSQL_STMT *stmt);
unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt);
my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt,
                                    enum enum_stmt_attr_type attrtype,
                                    const void *attr);
my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt,
                                    enum enum_stmt_attr_type attrtype,
                                    void *attr);
my_bool mysql_stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
my_bool mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd);
my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt, 
                                          unsigned int paramnumber,
                                          const char *data, 
                                          unsigned long length);
MYSQL_RES *mysql_stmt_result_metadata(MYSQL_STMT *stmt);
MYSQL_RES *mysql_stmt_param_metadata(MYSQL_STMT *stmt);
my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt);
my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt);
MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,
					MYSQL_ROW_OFFSET offset);
MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt);
void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset);
unsigned int mysql_stmt_errno(MYSQL_STMT *stmt);
const char *mysql_stmt_error(MYSQL_STMT *stmt);
my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt);
unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt);
const char *mysql_stmt_sqlstate(MYSQL_STMT *stmt);
my_bool mysql_stmt_free_result(MYSQL_STMT *stmt);
my_bool mysql_stmt_reset(MYSQL_STMT *stmt);
my_bool mysql_stmt_close(MYSQL_STMT *stmt);

void mysql_library_init(int argc, char **argv, char **groups);
void mysql_server_end();
void mysql_library_end();

static int unknownError(MYSQL *mysql);
static void setMySQLError(MYSQL *mysql,
			const char *error, unsigned int errorno);
static unsigned int mapErrorNumber(MYSQL *mysql, int64_t errorno);
static void loadErrorMap(MYSQL *mysql, const char *errormap);

unsigned int mysql_thread_safe() {
	debugFunction();
	return 1;
}

static MYSQL_PARAMETERS	mysql_parameters;
static unsigned long	p_max_allowed_packet=1024;
static unsigned long	p_net_buffer_length=1024;

my_bool my_init() {
	debugFunction();
	mysql_parameters.p_max_allowed_packet=&p_max_allowed_packet;
	mysql_parameters.p_net_buffer_length=&p_net_buffer_length;
	return 0;
}

MYSQL_PARAMETERS *mysql_get_parameters() {
	debugFunction();
	debugPrintf("p_max_allowed_packet=%ld\n",
			*mysql_parameters.p_max_allowed_packet);
	debugPrintf("p_net_buffer_length=%ld\n",
			*mysql_parameters.p_net_buffer_length);
	return &mysql_parameters;
}

MYSQL *mysql_init(MYSQL *mysql) {
	debugFunction();
	my_init();
	if (mysql) {
		bytestring::zero(mysql,sizeof(MYSQL));
		return mysql;
	} else {
		MYSQL	*retval=new MYSQL;
		bytestring::zero(retval,sizeof(MYSQL));
		return retval;
	}
}

int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option) {
	debugFunction();
	return 0;
}

int mysql_options(MYSQL *mysql, enum mysql_option option, const char *arg) {
	debugFunction();
	return 0;
}

int mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert,
			const char *ca, const char *capath,
			const char *cipher) {
	debugFunction();
	return 0;
}

const char *mysql_get_ssl_cipher(MYSQL *mysql) {
	debugFunction();
	return "";
}

my_bool mysql_thread_init(void) {
	debugFunction();
	return 0;
}

void mysql_thread_end(void) {
	debugFunction();
}

MYSQL *mysql_connect(MYSQL *mysql, const char *host,
			const char *user, const char *passwd) {
	debugFunction();
	return mysql_real_connect(mysql,host,user,passwd,NULL,9000,NULL,0);
}

MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user,
				const char *passwd, const char *db,
				unsigned int port, const char *unix_socket,
				unsigned long client_flag) {
	debugFunction();

	mysql->host=host;
	mysql->port=port;
	mysql->unix_socket=unix_socket;

	mysql->sqlrcon=new sqlrconnection(host,port,unix_socket,
						user,passwd,0,1,true);
	mysql->errormap=NULL;
	const char	*errormap=environment::getValue("SQLR_MYSQL_ERROR_MAP");
	if (charstring::length(errormap)) {
		loadErrorMap(mysql,errormap);
	}
	mysql->backendchecked=false;
	mysql->backendismysql=false;
	mysql->currentstmt=NULL;
	mysql_select_db(mysql,db);
	return mysql;
}

void mysql_close(MYSQL *mysql) {
	debugFunction();
	if (mysql) {
		mysql_stmt_close(mysql->currentstmt);
		delete mysql->sqlrcon;
		setMySQLError(mysql,NULL,0);
		if (mysql->deleteonclose) {
			delete[] mysql->error;
			delete mysql->errormap;
			delete mysql;
		}
	}
}


int mysql_ping(MYSQL *mysql) {
	debugFunction();
	return !mysql->sqlrcon->ping();
}

const char *mysql_stat(MYSQL *mysql) {
	debugFunction();
	return "Uptime: 0  Threads: 0  Questions: 0  "
		"Slow queries: 0  Opens: 0  Flush tables: 0  "
		"Open tables: 0 Queries per second avg: 0.0";
}

int mysql_shutdown(MYSQL *mysql) {
	debugFunction();
	sqlrcursor	sqlrcur(mysql->sqlrcon);
	return !sqlrcur.sendQuery("SHUTDOWN");
}

int mysql_refresh(MYSQL *mysql, unsigned int refresh_options) {
	debugFunction();
	const char	*query=NULL;
	switch (refresh_options) {
		case REFRESH_GRANT:
			query="FLUSH PRIVILEGES";
			break;
		case REFRESH_LOG:
			query="FLUSH LOGS";
			break;
		case REFRESH_TABLES:
			query="FLUSH TABLES";
			break;
		case REFRESH_HOSTS:
			query="FLUSH HOSTS";
			break;
		case REFRESH_STATUS:
			query="FLUSH STATUS";
			break;
		case REFRESH_THREADS:
			// FIXME: do something?
			return 0;
		case REFRESH_SLAVE:
			query="RESET SLAVE";
			break;
		case REFRESH_MASTER:
			query="RESET MASTER";
			break;
	}
	sqlrcursor	sqlrcur(mysql->sqlrcon);
	return !sqlrcur.sendQuery(query);
}

int mysql_reload(MYSQL *mysql) {
	debugFunction();
	return mysql_refresh(mysql,REFRESH_GRANT);
}

unsigned long mysql_thread_id(MYSQL *mysql) {
	debugFunction();
	return 0;
}

MYSQL_RES *mysql_list_processes(MYSQL *mysql) {
	debugFunction();
	mysql_stmt_close(mysql->currentstmt);

	mysql->currentstmt=new MYSQL_STMT;
	mysql->currentstmt->result=new MYSQL_RES;
	mysql->currentstmt->result->stmtbackptr=NULL;
	mysql->currentstmt->result->sqlrcur=new sqlrcursor(mysql->sqlrcon,true);
	mysql->currentstmt->result->errorno=0;
	mysql->currentstmt->result->fields=NULL;
	mysql->currentstmt->result->lengths=NULL;
	mysql->currentstmt->result->sqlrcur->sendQuery("SHOW PROCESSLIST");
	processFields(mysql->currentstmt);
	mysql->currentstmt->result->currentfield=0;

	MYSQL_RES	*retval=mysql->currentstmt->result;
	mysql->currentstmt->result=NULL;
	return retval;
}

int mysql_kill(MYSQL *mysql, unsigned long pid) {
	debugFunction();
	sqlrcursor	sqlrcur(mysql->sqlrcon);
	stringbuffer	query;
	query.append("KILL ")->append((uint64_t)pid);
	return !sqlrcur.sendQuery(query.getString());
}



const char *mysql_get_client_info() {
	debugFunction();
	// Returns a string that represents the client library version.
	#ifdef COMPAT_MYSQL_3
		return "3.23.58";
	#endif
	#ifdef COMPAT_MYSQL_4_0
		return "4.0.17";
	#endif
	#ifdef COMPAT_MYSQL_4_1
		return "4.1.1";
	#endif
	#ifdef COMPAT_MYSQL_5_0
		return "5.0.0";
	#endif
	#ifdef COMPAT_MYSQL_5_1
		return "5.1.22";
	#endif
}

unsigned long mysql_get_client_version() {
	debugFunction();
	// Returns an integer that represents the client library version.
	// The value has the format XYYZZ where X is the major version, YY is
	// the release level, and ZZ is the version number within the release
	// level. For example, a value of 40102 represents a client library
	// version of 4.1.2.
	#ifdef COMPAT_MYSQL_3
		return 32358;
	#endif
	#ifdef COMPAT_MYSQL_4_0
		return 40017;
	#endif
	#ifdef COMPAT_MYSQL_4_1
		return 40101;
	#endif
	#ifdef COMPAT_MYSQL_5_0
		return 50000;
	#endif
	#ifdef COMPAT_MYSQL_5_1
		return 50122;
	#endif
}

const char *mysql_get_host_info(MYSQL *mysql) {
	debugFunction();
	// Returns a string describing the type of connection in use,
	// including the server host name.
	// Should be "host via [unix|inet] socket"
	return "";
}

unsigned int mysql_get_proto_info(MYSQL *mysql) {
	debugFunction();
	// Returns the protocol version used by current connection.
	#ifdef COMPAT_MYSQL_3
		return 10;
	#endif
	#ifdef COMPAT_MYSQL_4_0
		return 12;
	#endif
	#ifdef COMPAT_MYSQL_4_1
		return 14;
	#endif
	#ifdef COMPAT_MYSQL_5_0
		return 14;
	#endif
	#ifdef COMPAT_MYSQL_5_1
		return 10;
	#endif
}

const char *mysql_get_server_info(MYSQL *mysql) {
	debugFunction();
	// Returns a string that represents the server version number.
	#ifdef COMPAT_MYSQL_3
		return "3.23.58";
	#endif
	#ifdef COMPAT_MYSQL_4_0
		return "4.0.17";
	#endif
	#ifdef COMPAT_MYSQL_4_1
		return "4.1.1";
	#endif
	#ifdef COMPAT_MYSQL_5_0
		return "5.0.0";
	#endif
	#ifdef COMPAT_MYSQL_5_1
		return "5.1.22";
	#endif
}

unsigned long mysql_get_server_version(MYSQL *mysql) {
	debugFunction();
	// A number that represents the MySQL server version in format:
	// main_version*10000 + minor_version *100 + sub_version
	// For example, 4.1.0 is returned as 40100.
	#ifdef COMPAT_MYSQL_3
		return 32358;
	#endif
	#ifdef COMPAT_MYSQL_4_0
		return 40017;
	#endif
	#ifdef COMPAT_MYSQL_4_1
		return 40101;
	#endif
	#ifdef COMPAT_MYSQL_5_0
		return 50000;
	#endif
	#ifdef COMPAT_MYSQL_5_1
		return 50122;
	#endif
}



my_bool	mysql_change_user(MYSQL *mysql, const char *user,
				const char *password, const char *db) {
	debugFunction();

	if (!mysql->sqlrcon->rollback()) {
		return 1;
	}
	mysql_stmt_close(mysql->currentstmt);
	delete mysql->sqlrcon;
	return (mysql_real_connect(mysql,mysql->host,user,password,db,
					mysql->port,mysql->unix_socket,0))?0:1;
}

const char *mysql_character_set_name(MYSQL *mysql) {
	debugFunction();
	return "latin1";
}

int mysql_set_character_set(MYSQL *mysql, const char *csname) {
	debugFunction();
	// FIXME: implement this somehow
	return 0;
}

struct MY_CHARSET_INFO;

void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset) {
	debugFunction();
	// FIXME: implement this somehow
}



void mysql_debug(const char *debug) {
	// don't do anything...
}

int mysql_dump_debug_info(MYSQL *mysql) {
	debugFunction();
	return unknownError(mysql);
}



int mysql_create_db(MYSQL *mysql, const char *db) {
	debugFunction();
	return unknownError(mysql);
}

int mysql_select_db(MYSQL *mysql, const char *db) {
	debugFunction();
	return !mysql->sqlrcon->selectDatabase(db);
}

int mysql_drop_db(MYSQL *mysql, const char *db) {
	debugFunction();
	return unknownError(mysql);
}

MYSQL_RES *mysql_list_dbs(MYSQL *mysql, const char *wild) {

	debugFunction();
	mysql_stmt_close(mysql->currentstmt);

	mysql->currentstmt=new MYSQL_STMT;
	mysql->currentstmt->result=new MYSQL_RES;
	mysql->currentstmt->result->stmtbackptr=NULL;
	mysql->currentstmt->result->sqlrcur=new sqlrcursor(mysql->sqlrcon,true);
	mysql->currentstmt->result->errorno=0;
	mysql->currentstmt->result->fields=NULL;
	mysql->currentstmt->result->lengths=NULL;
	mysql->currentstmt->result->sqlrcur->getDatabaseList(wild,
						SQLRCLIENTLISTFORMAT_MYSQL);
	processFields(mysql->currentstmt);
	mysql->currentstmt->result->currentfield=0;

	MYSQL_RES	*retval=mysql->currentstmt->result;
	mysql->currentstmt->result=NULL;
	return retval;
}

MYSQL_RES *mysql_list_tables(MYSQL *mysql, const char *wild) {

	debugFunction();
	mysql_stmt_close(mysql->currentstmt);

	mysql->currentstmt=new MYSQL_STMT;
	mysql->currentstmt->result=new MYSQL_RES;
	mysql->currentstmt->result->stmtbackptr=NULL;
	mysql->currentstmt->result->sqlrcur=new sqlrcursor(mysql->sqlrcon,true);
	mysql->currentstmt->result->errorno=0;
	mysql->currentstmt->result->fields=NULL;
	mysql->currentstmt->result->lengths=NULL;
	mysql->currentstmt->result->sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_MYSQL,
					DB_OBJECT_TABLE|DB_OBJECT_VIEW|
					DB_OBJECT_ALIAS|DB_OBJECT_SYNONYM);
	processFields(mysql->currentstmt);
	mysql->currentstmt->result->currentfield=0;

	MYSQL_RES	*retval=mysql->currentstmt->result;
	mysql->currentstmt->result=NULL;
	return retval;
}

bool isTrue(const char *value) {
	return (value &&
		(value[0]=='y' ||
		value[0]=='Y' ||
		value[0]=='t' ||
		value[0]=='T' ||
		value[0]=='1'));
}

MYSQL_RES *mysql_list_fields(MYSQL *mysql,
				const char *table, const char *wild) {

	debugFunction();
	debugPrintf("%s\n",table);
	mysql_stmt_close(mysql->currentstmt);

	MYSQL_STMT	*stmt=new MYSQL_STMT;
	mysql->currentstmt=stmt;
	stmt->result=new MYSQL_RES;
	stmt->result->stmtbackptr=NULL;
	stmt->result->sqlrcur=new sqlrcursor(mysql->sqlrcon,true);
	stmt->result->errorno=0;
	stmt->result->fields=NULL;
	stmt->result->lengths=NULL;
	stmt->result->sqlrcur->getColumnList(table,wild,
					SQLRCLIENTLISTFORMAT_MYSQL);


	// translate the rows into fields...
	delete[] stmt->result->fields;
	delete[] stmt->result->lengths;

	sqlrcursor	*sqlrcur=stmt->result->sqlrcur;

	uint32_t	colcount=sqlrcur->rowCount();
	if (colcount) {

		MYSQL_FIELD	*fields=new MYSQL_FIELD[colcount];
		stmt->result->fields=fields;

		stmt->result->lengths=new unsigned long[colcount];

		for (uint32_t i=0; i<colcount; i++) {

			fields[i].name=const_cast<char *>(
					sqlrcur->getField(i,(uint32_t)0));
			fields[i].table=const_cast<char *>(table);
			const char	*def=sqlrcur->getField(i,(uint32_t)7);
			fields[i].def=(charstring::isNullOrEmpty(def))?
							NULL:(char *)def;

			#if defined(COMPAT_MYSQL_4_0) || \
				defined(COMPAT_MYSQL_4_1) || \
				defined(COMPAT_MYSQL_5_0) || \
				defined(COMPAT_MYSQL_5_1)
  			fields[i].org_table=fields[i].table;
  			fields[i].db=(char *)"";
			#if defined(COMPAT_MYSQL_4_1) || \
				defined(COMPAT_MYSQL_5_0) || \
				defined(COMPAT_MYSQL_5_1)
  			fields[i].catalog=(char *)"def";
  			fields[i].org_name=(char *)"";
			fields[i].name_length=
				charstring::length(fields[i].name);
			fields[i].org_name_length=
				charstring::length(fields[i].org_name);
			fields[i].table_length=
				charstring::length(fields[i].table);
			fields[i].org_table_length=
				charstring::length(fields[i].org_table);
			fields[i].db_length=
				charstring::length(fields[i].db);
			fields[i].catalog_length=
				charstring::length(fields[i].catalog);
			fields[i].def_length=
				charstring::length(fields[i].def);
			// FIXME: need a character set number here
			fields[i].charsetnr=0;
			#endif
			#endif

			// figure out the column type
			const char	*columntypestring=
						sqlrcur->getField(i,1);
			const char	*lengthstring=
						sqlrcur->getField(i,2);
			int64_t	prec=sqlrcur->getFieldAsInteger(i,3);
			int64_t	scale=sqlrcur->getFieldAsInteger(i,4);
			const char	*nullable=sqlrcur->getField(i,5);
			const char	*key=sqlrcur->getField(i,6);
			const char	*extra=sqlrcur->getField(i,8);
			enum enum_field_types	columntype=
					map_col_type(columntypestring,scale);
			fields[i].type=columntype;

			// determine the length...
			unsigned int	len=0;
			if (!charstring::isNullOrEmpty(lengthstring)) {
				len=charstring::toInteger(lengthstring);
			} else {
				switch (columntype) {
					case MYSQL_TYPE_DECIMAL:
						len=prec+2;
						break;
					case MYSQL_TYPE_TINY:
						len=4;
						break;
					case MYSQL_TYPE_SHORT:
						len=6;
						break;
					case MYSQL_TYPE_LONG:
						len=11;
						break;
					case MYSQL_TYPE_FLOAT:
						len=12;
						break;
					case MYSQL_TYPE_DOUBLE:
						len=22;
						break;
					case MYSQL_TYPE_TIMESTAMP:
						len=19;
						break;
					case MYSQL_TYPE_LONGLONG:
						len=20;
						break;
					case MYSQL_TYPE_INT24:
						len=9;
						break;
					case MYSQL_TYPE_DATE:
						len=10;
						break;
					case MYSQL_TYPE_TIME:
						len=10;
						break;
					case MYSQL_TYPE_DATETIME:
						len=19;
						break;
					case MYSQL_TYPE_YEAR:
						len=4;
						break;
					case MYSQL_TYPE_NEWDATE:
						len=10;
						break;
					case MYSQL_TYPE_BIT:
						len=1;
						break;
					case MYSQL_TYPE_TIMESTAMP2:
						len=19;
						break;
					case MYSQL_TYPE_DATETIME2:
						len=19;
						break;
					case MYSQL_TYPE_TIME2:
						len=10;
						break;
					case MYSQL_TYPE_NEWDECIMAL:
						len=sqlrcur->getFieldAsInteger(
							i,(uint32_t)3)+2;
						break;
					case MYSQL_TYPE_ENUM:
					case MYSQL_TYPE_SET:
					case MYSQL_TYPE_GEOMETRY:
						// FIXME: not really sure
						// about these
						len=8;
						break;
					default:
						// fall back to 50
						len=50;
						break;
				}
			}
			fields[i].length=len;

			// it looks like this is always 0
			fields[i].max_length=0;

			// figure out the flags
			unsigned int	flags=0;
			if (!isTrue(nullable)) {
				flags|=NOT_NULL_FLAG;
			}
			if (!charstring::compareIgnoringCase(key,"pri")) {
				flags|=PRI_KEY_FLAG;
			}
			if (!charstring::compareIgnoringCase(key,"uni")) {
				flags|=UNIQUE_KEY_FLAG;
			}
			if (!charstring::isNullOrEmpty(key)) {
				flags|=MULTIPLE_KEY_FLAG;
			}
			if (columntype==MYSQL_TYPE_TINY_BLOB ||
				columntype==MYSQL_TYPE_MEDIUM_BLOB ||
				columntype==MYSQL_TYPE_LONG_BLOB ||
				columntype==MYSQL_TYPE_BLOB) {
				flags|=BLOB_FLAG;
			}
			// FIXME: the "unsigned" test won't work with most db's
			if (charstring::contains(columntypestring,"unsigned") ||
				isUnsignedTypeChar(columntypestring)) {
				flags|=UNSIGNED_FLAG;
			}
			// FIXME: there are probably other
			// types that are zero-filled
			if (columntype==MYSQL_TYPE_YEAR) {
				flags|=ZEROFILL_FLAG;
			}
			if (isBinaryTypeChar(columntypestring)) {
				flags|=BINARY_FLAG;
			}
			if (columntype==MYSQL_TYPE_ENUM) {
				flags|=ENUM_FLAG;
			}
			if (charstring::contains(extra,"auto_increment")) {
				flags|=AUTO_INCREMENT_FLAG;
			}
			if (columntype==MYSQL_TYPE_TIMESTAMP ||
				columntype==MYSQL_TYPE_TIMESTAMP2) {
				flags|=TIMESTAMP_FLAG|ON_UPDATE_NOW_FLAG;
			}
			if (columntype==MYSQL_TYPE_SET) {
				flags|=SET_FLAG;
			}
			if (isNumberTypeChar(columntypestring)) {
				flags|=NUM_FLAG;
			}
			fields[i].flags=flags;
		
			// set the number of decimal places (scale)
			fields[i].decimals=scale;
		}

		// set the field count
		stmt->result->fieldcount=colcount;
	} else {
		stmt->result->fields=NULL;
		stmt->result->lengths=NULL;
		stmt->result->fieldcount=0;
	}

	stmt->result->currentfield=0;

	// set rowcount beyond the end so no rows can be fetched
	stmt->result->currentrow=sqlrcur->rowCount()+1;

	MYSQL_RES	*retval=stmt->result;
	stmt->result=NULL;
	return retval;
}


unsigned long mysql_escape_string(char *to, const char *from,
					unsigned long length) {
	debugFunction();
	return mysql_real_escape_string(NULL,to,from,length);
}

char *mysql_odbc_escape_string(MYSQL *mysql, char *to,
				unsigned long to_length,
				const char *from,
				unsigned long from_length,
				void *param,
				char *(*extend_buffer)
					(void *, char *to,
					unsigned long *length)) {
	debugFunction();
	// FIXME: implement this
	return NULL;
}

void myodbc_remove_escape(MYSQL *mysql, char *name) {
	debugFunction();
	// FIXME: implement this
}

unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,
					const char *from,
					unsigned long length) {
	debugFunction();

	if (mysql && charstring::compare(mysql->sqlrcon->identify(),"mysql")) {
		bytestring::copy(to,from,length);
		to[length]='\0';
		return length;
	}
	
	unsigned long	fromindex=0;
	unsigned long	toindex=0;
	while (fromindex<length) {
		if (from[fromindex]=='\'') {
			to[toindex++]='\\';
			to[toindex++]='\'';
		} else if (from[fromindex]=='\"') {
			to[toindex++]='\\';
			to[toindex++]='"';
		} else if (from[fromindex]=='\n') {
			to[toindex++]='\\';
			to[toindex++]='n';
		} else if (from[fromindex]=='\r') {
			to[toindex++]='\\';
			to[toindex++]='r';
		} else if (from[fromindex]=='\\') {
			to[toindex++]='\\';
			to[toindex++]='\\';
		} else if (from[fromindex]==26) {
			to[toindex++]='\\';
			to[toindex++]='Z';
		} else {
			to[toindex++]=from[fromindex];
		}
		fromindex++;
	}
	to[toindex]='\0';
	return toindex;
}

int mysql_query(MYSQL *mysql, const char *query) {
	debugFunction();
	return mysql_real_query(mysql,query,charstring::length(query));
}

int mysql_send_query(MYSQL *mysql, const char *query, unsigned int length) {
	debugFunction();
	// FIXME: looks like this sends a query in the background then returns
	// so we can do something else in the foreground.
	return mysql_real_query(mysql,query,length);
}

int mysql_read_query_result(MYSQL *mysql) {
	debugFunction();
	// FIXME: looks like this checks to see if a query sent with
	// mysql_send_query has finished or not
	return 0;
}

int mysql_real_query(MYSQL *mysql, const char *query, unsigned long length) {
	debugFunction();
	mysql_stmt_close(mysql->currentstmt);
	mysql->currentstmt=mysql_prepare(mysql,query,length);
	return mysql_stmt_execute(mysql->currentstmt);
}

const char *mysql_info(MYSQL *mysql) {
	debugFunction();
	return NULL;
}

my_ulonglong mysql_insert_id(MYSQL *mysql) {
	debugFunction();
	return mysql->sqlrcon->getLastInsertId();
}


MYSQL_RES *mysql_store_result(MYSQL *mysql) {
	debugFunction();
	MYSQL_RES	*retval=mysql->currentstmt->result;
	mysql->currentstmt->result=NULL;
	return retval;
}

MYSQL_RES *mysql_use_result(MYSQL *mysql) {
	debugFunction();
	return mysql_store_result(mysql);
}

void mysql_free_result(MYSQL_RES *result) {
	debugFunction();
	if (result) {
		delete result->sqlrcur;
		if (result->fields) {
			delete[] result->fields;
			delete[] result->lengths;
		}

		// if the result is attached to a stmt then set the stmt's
		// result pointer to NULL, that way if someone calls
		// mysql_free_result, followed by mysql_stmt_close then
		// a double free won't occur but calling mysql_stmt_close on
		// its own will free the result and close the stmt
		if (result->stmtbackptr) {
			result->stmtbackptr->result=NULL;
		}
		delete result;
	}
}

my_bool mysql_more_results(MYSQL *mysql) {
	debugFunction();
	return 0;
}

int mysql_next_result(MYSQL *mysql) {
	return -1;
}


unsigned int mysql_num_fields(MYSQL_RES *result) {
	debugFunction();
	unsigned int	retval=result->fieldcount;
	debugPrintf("num fields: %d\n",retval);
	return retval;
}

MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result) {
	debugFunction();
	if (result->currentfield>=(MYSQL_FIELD_OFFSET)result->fieldcount) {
		return NULL;
	}
	debugPrintf("name: \"%s\"\n",
			result->fields[result->currentfield].name);
	return &result->fields[result->currentfield++];
}

MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result) {
	debugFunction();
	result->currentfield=0;
	return result->fields;
}

MYSQL_FIELD *mysql_fetch_field_direct(MYSQL_RES *result, unsigned int fieldnr) {
	debugFunction();
	if (fieldnr>(unsigned int)result->fieldcount) {
		return NULL;
	}
	// FIXME: it's possible that we shouldn't increment the fieldnr here
	result->currentfield=fieldnr+1;
	return &result->fields[fieldnr];
}

unsigned long *mysql_fetch_lengths(MYSQL_RES *result) {
	debugFunction();
	uint32_t	*lengths=result->sqlrcur->
					getRowLengths(result->previousrow);
	for (uint32_t i=0; i<result->fieldcount; i++) {
		result->lengths[i]=(unsigned long)lengths[i];
	}
	return result->lengths;
}

unsigned int mysql_field_count(MYSQL *mysql) {
	debugFunction();
	return mysql_num_fields(mysql->currentstmt->result);
}

MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result,
					MYSQL_FIELD_OFFSET offset) {
	debugFunction();
	MYSQL_FIELD_OFFSET	oldoffset=result->currentfield;
	result->currentfield=offset;
	return oldoffset;
}

MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *result) {
	debugFunction();
	return result->currentfield;
}


my_ulonglong mysql_num_rows(MYSQL_RES *result) {
	debugFunction();
	return result->sqlrcur->rowCount();
}

my_ulonglong mysql_affected_rows(MYSQL *mysql) {
	debugFunction();
	return mysql_stmt_affected_rows(mysql->currentstmt);
}

MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset) {
	debugFunction();
	MYSQL_ROW_OFFSET	currentoffset=mysql_row_tell(result);
	result->previousrow=result->currentrow;
	result->currentrow=
		((linkedlistnode< my_ulonglong > *)offset)->getValue();
	return (MYSQL_ROW_OFFSET)currentoffset;
}

MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *result) {
	debugFunction();
	linkedlistnode< my_ulonglong >	*rowcachenode=
		new linkedlistnode< my_ulonglong >(result->currentrow);
	result->rowcache.append(rowcachenode);
	return (MYSQL_ROW_OFFSET)rowcachenode;
}

void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset) {
	debugFunction();
	result->previousrow=result->currentrow;
	result->currentrow=offset;
}

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result) {
	debugFunction();
	MYSQL_ROW	retval=
		(MYSQL_ROW)result->sqlrcur->getRow(result->currentrow);
	if (retval) {
		result->previousrow=result->currentrow;
		result->currentrow++;
	}
	return retval;
}

my_bool mysql_eof(MYSQL_RES *result) {
	debugFunction();
	my_ulonglong	rowcount=(my_ulonglong)result->sqlrcur->rowCount();
	return (!rowcount || result->currentrow>=rowcount);
}


unsigned int mysql_warning_count(MYSQL *mysql) {
	debugFunction();
	return 0;
}

unsigned int mysql_errno(MYSQL *mysql) {
	debugFunction();
	debugPrintf("errno: %d\n",mysql->errorno);
	return mysql->errorno;
}

const char *mysql_error(MYSQL *mysql) {
	debugFunction();
	debugPrintf("error: %s\n",mysql->error);
	return (mysql->error)?mysql->error:"";
}

const char *mysql_sqlstate(MYSQL *mysql) {
	debugFunction();
	return mysql_stmt_sqlstate(mysql->currentstmt);
}


my_bool mysql_commit(MYSQL *mysql) {
	debugFunction();
	return mysql->sqlrcon->commit();
}

my_bool mysql_rollback(MYSQL *mysql) {
	debugFunction();
	return mysql->sqlrcon->rollback();
}

my_bool mysql_autocommit(MYSQL *mysql, my_bool mode) {
	debugFunction();
	return (mode)?mysql->sqlrcon->autoCommitOn():
				mysql->sqlrcon->autoCommitOff();
}



MYSQL_STMT *mysql_prepare(MYSQL *mysql, const char *query,
					unsigned long length) {
	debugFunction();
	MYSQL_STMT	*stmt=mysql_stmt_init(mysql);
	if (!mysql_stmt_prepare(stmt,query,length)) {
		return stmt;
	}
	mysql_stmt_close(stmt);
	return NULL;
}

my_bool mysql_bind_param(MYSQL_STMT *stmt, MYSQL_BIND *bind) {
	debugFunction();
	return mysql_stmt_bind_param(stmt,bind);
}

my_bool mysql_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *bind) {
	debugFunction();
	return mysql_stmt_bind_result(stmt,bind);
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
	MYSQL_TYPE_NEWDECIMAL,
	// "DATETIME"
	MYSQL_TYPE_DATETIME,
	// "NUMERIC"
	MYSQL_TYPE_NEWDECIMAL,
	// "DECIMAL"
	MYSQL_TYPE_NEWDECIMAL,
	// "SMALLDATETIME"
	MYSQL_TYPE_DATETIME,
	// "SMALLMONEY"
	MYSQL_TYPE_NEWDECIMAL,
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
	MYSQL_TYPE_BLOB,
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
	MYSQL_TYPE_DATE,
	// "TIME"
	MYSQL_TYPE_TIME,
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
	MYSQL_TYPE_NEWDECIMAL,
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
	// added by firebird
	// "D_FLOAT"
	MYSQL_TYPE_DOUBLE,
	// "ARRAY"
	MYSQL_TYPE_SET,
	// "QUAD"
	MYSQL_TYPE_SET,
	// "INT64"
	MYSQL_TYPE_LONGLONG,
	// "DOUBLE PRECISION"
	MYSQL_TYPE_DOUBLE,
	// added by postgresql
	// "BOOL"
	MYSQL_TYPE_TINY,
	// "BYTEA"
	MYSQL_TYPE_BLOB,
	// "NAME"
	MYSQL_TYPE_STRING,
	// "INT8"
	MYSQL_TYPE_LONG,
	// "INT2"
	MYSQL_TYPE_SHORT,
	// "INT2VECTOR"
	MYSQL_TYPE_SET,
	// "INT4"
	MYSQL_TYPE_LONG,
	// "REGPROC"
	MYSQL_TYPE_LONG,
	// "OID"
	MYSQL_TYPE_LONG,
	// "TID"
	MYSQL_TYPE_LONG,
	// "XID"
	MYSQL_TYPE_LONG,
	// "CID"
	MYSQL_TYPE_LONG,
	// "OIDVECTOR"
	MYSQL_TYPE_SET,
	// "SMGR"
	MYSQL_TYPE_STRING,
	// "POINT"
	MYSQL_TYPE_STRING,
	// "LSEG"
	MYSQL_TYPE_STRING,
	// "PATH"
	MYSQL_TYPE_STRING,
	// "BOX"
	MYSQL_TYPE_STRING,
	// "POLYGON"
	MYSQL_TYPE_STRING,
	// "LINE"
	MYSQL_TYPE_STRING,
	// "LINE_ARRAY"
	MYSQL_TYPE_SET,
	// "FLOAT4"
	MYSQL_TYPE_FLOAT,
	// "FLOAT8"
	MYSQL_TYPE_DOUBLE,
	// "ABSTIME"
	MYSQL_TYPE_LONG,
	// "RELTIME"
	MYSQL_TYPE_LONG,
	// "TINTERVAL"
	MYSQL_TYPE_LONG,
	// "CIRCLE"
	MYSQL_TYPE_STRING,
	// "CIRCLE_ARRAY"
	MYSQL_TYPE_SET,
	// "MONEY_ARRAY"
	MYSQL_TYPE_SET,
	// "MACADDR"
	MYSQL_TYPE_STRING,
	// "INET"
	MYSQL_TYPE_STRING,
	// "CIDR"
	MYSQL_TYPE_STRING,
	// "BOOL_ARRAY"
	MYSQL_TYPE_SET,
	// "BYTEA_ARRAY"
	MYSQL_TYPE_SET,
	// "CHAR_ARRAY"
	MYSQL_TYPE_SET,
	// "NAME_ARRAY"
	MYSQL_TYPE_SET,
	// "INT2_ARRAY"
	MYSQL_TYPE_SET,
	// "INT2VECTOR_ARRAY"
	MYSQL_TYPE_SET,
	// "INT4_ARRAY"
	MYSQL_TYPE_SET,
	// "REGPROC_ARRAY"
	MYSQL_TYPE_SET,
	// "TEXT_ARRAY"
	MYSQL_TYPE_SET,
	// "OID_ARRAY"
	MYSQL_TYPE_SET,
	// "TID_ARRAY"
	MYSQL_TYPE_SET,
	// "XID_ARRAY"
	MYSQL_TYPE_SET,
	// "CID_ARRAY"
	MYSQL_TYPE_SET,
	// "OIDVECTOR_ARRAY"
	MYSQL_TYPE_SET,
	// "BPCHAR_ARRAY"
	MYSQL_TYPE_SET,
	// "VARCHAR_ARRAY"
	MYSQL_TYPE_SET,
	// "INT8_ARRAY"
	MYSQL_TYPE_SET,
	// "POINT_ARRAY"
	MYSQL_TYPE_SET,
	// "LSEG_ARRAY"
	MYSQL_TYPE_SET,
	// "PATH_ARRAY"
	MYSQL_TYPE_SET,
	// "BOX_ARRAY"
	MYSQL_TYPE_SET,
	// "FLOAT4_ARRAY"
	MYSQL_TYPE_SET,
	// "FLOAT8_ARRAY"
	MYSQL_TYPE_SET,
	// "ABSTIME_ARRAY"
	MYSQL_TYPE_SET,
	// "RELTIME_ARRAY"
	MYSQL_TYPE_SET,
	// "TINTERVAL_ARRAY"
	MYSQL_TYPE_SET,
	// "POLYGON_ARRAY"
	MYSQL_TYPE_SET,
	// "ACLITEM"
	MYSQL_TYPE_SET,
	// "ACLITEM_ARRAY"
	MYSQL_TYPE_SET,
	// "MACADDR_ARRAY"
	MYSQL_TYPE_SET,
	// "INET_ARRAY"
	MYSQL_TYPE_SET,
	// "CIDR_ARRAY"
	MYSQL_TYPE_SET,
	// "BPCHAR"
	MYSQL_TYPE_STRING,
	// "TIMESTAMP_ARRAY"
	MYSQL_TYPE_SET,
	// "DATE_ARRAY"
	MYSQL_TYPE_SET,
	// "TIME_ARRAY"
	MYSQL_TYPE_SET,
	// "TIMESTAMPTZ"
	MYSQL_TYPE_STRING,
	// "TIMESTAMPTZ_ARRAY"
	MYSQL_TYPE_SET,
	// "INTERVAL"
	MYSQL_TYPE_LONG,
	// "INTERVAL_ARRAY"
	MYSQL_TYPE_SET,
	// "NUMERIC_ARRAY"
	MYSQL_TYPE_SET,
	// "TIMETZ"
	MYSQL_TYPE_STRING,
	// "TIMETZ_ARRAY"
	MYSQL_TYPE_SET,
	// "BIT_ARRAY"
	MYSQL_TYPE_SET,
	// "VARBIT"
	MYSQL_TYPE_STRING,
	// "VARBIT_ARRAY"
	MYSQL_TYPE_SET,
	// "REFCURSOR"
	MYSQL_TYPE_LONG,
	// "REFCURSOR_ARRAY"
	MYSQL_TYPE_SET,
	// "REGPROCEDURE"
	MYSQL_TYPE_LONG,
	// "REGOPER"
	MYSQL_TYPE_LONG,
	// "REGOPERATOR"
	MYSQL_TYPE_LONG,
	// "REGCLASS"
	MYSQL_TYPE_LONG,
	// "REGTYPE"
	MYSQL_TYPE_LONG,
	// "REGPROCEDURE_ARRAY"
	MYSQL_TYPE_SET,
	// "REGOPER_ARRAY"
	MYSQL_TYPE_SET,
	// "REGOPERATOR_ARRAY"
	MYSQL_TYPE_SET,
	// "REGCLASS_ARRAY"
	MYSQL_TYPE_SET,
	// "REGTYPE_ARRAY"
	MYSQL_TYPE_SET,
	// "RECORD"
	MYSQL_TYPE_SET,
	// "CSTRING"
	MYSQL_TYPE_STRING,
	// "ANY"
	MYSQL_TYPE_STRING,
	// "ANYARRAY"
	MYSQL_TYPE_SET,
	// "TRIGGER"
	MYSQL_TYPE_STRING,
	// "LANGUAGE_HANDLER"
	MYSQL_TYPE_STRING,
	// "INTERNAL"
	MYSQL_TYPE_STRING,
	// "OPAQUE"
	MYSQL_TYPE_STRING,
	// "ANYELEMENT"
	MYSQL_TYPE_STRING,
	// "PG_TYPE"
	MYSQL_TYPE_STRING,
	// "PG_ATTRIBUTE"
	MYSQL_TYPE_STRING,
	// "PG_PROC"
	MYSQL_TYPE_STRING,
	// "PG_CLASS"
	MYSQL_TYPE_STRING,
	// none added by sqlite
	// added by sqlserver
	// "UBIGINT"
	MYSQL_TYPE_LONGLONG,
	// "UNIQUEIDENTIFIER"
	MYSQL_TYPE_STRING,
	// added by informix
	// "SMALLFLOAT"
	MYSQL_TYPE_FLOAT,
	// "BYTE"
	MYSQL_TYPE_BLOB,
	// "BOOLEAN"
	MYSQL_TYPE_TINY,
	// "TINYTEXT"
	MYSQL_TYPE_BLOB,
	// "MEDIUMTEXT"
	MYSQL_TYPE_BLOB,
	// "LONGTEXT"
	MYSQL_TYPE_BLOB,
	// "JSON"
	MYSQL_TYPE_BLOB,
	// "GEOMETRY"
	MYSQL_TYPE_BLOB,
	// "SDO_GEOMETRY"
	MYSQL_TYPE_BLOB,
	// "NCHAR"
	MYSQL_TYPE_STRING,
	// "NVARCHAR"
	MYSQL_TYPE_VAR_STRING,
	// "NTEXT"
	MYSQL_TYPE_BLOB,
	// "XML"
	MYSQL_TYPE_BLOB,
	// "DATETIMEOFFSET"
	MYSQL_TYPE_DATETIME
};

enum enum_field_types map_col_type(const char *columntype, int64_t scale) {
	debugFunction();

	size_t		columntypelen=charstring::length(columntype);

	// sometimes column types have parentheses, like CHAR(40)
	const char	*leftparen=charstring::findFirst(columntype,"(");
	if (leftparen) {
		columntypelen=leftparen-columntype;
	}

	for (int index=0; datatypestring[index]; index++) {

		// compare "columntypelen" bytes but also make sure that the
		// byte afterward is a NULL, we don't want "DATE" to match
		// "DATETIME" for example
		if (!charstring::compareIgnoringCase(
					datatypestring[index],
					columntype,columntypelen) &&
				datatypestring[index][columntypelen]=='\0') {

			enum_field_types	retval=mysqltypemap[index];

			// Some DB's, like oracle, don't distinguish between
			// decimal and integer types, they just have a numeric
			// field which may or may not have decimal points.
			// Those fields types get translated to "decimal"
			// but if there are 0 decimal points, then we need to
			// translate them to an integer type here.
			if ((retval==MYSQL_TYPE_DECIMAL ||
				retval==MYSQL_TYPE_NEWDECIMAL) && !scale) {
				retval=MYSQL_TYPE_LONG;
			}

			// Some DB's, like oracle, don't have separate DATE
			// and DATETIME types.  Rather, a DATE can store the
			// date and time, but which components it reports
			// depends on something like the NLS_DATE_FORMAT.  By
			// default, we map DATE to MYSQL_TYPE_DATE, but we also
			// provide the option of mapping it to
			// MYSQL_TYPE_DATETIME.
			if (retval==MYSQL_TYPE_DATE &&
				charstring::isYes(
					environment::getValue(
					"SQLR_MYSQL_MAP_DATE_TO_DATETIME"))) {
				retval=MYSQL_TYPE_DATETIME;
			}
			return retval;
		}
	}
	return MYSQL_TYPE_NULL;
}

int mysql_execute(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_stmt_execute(stmt);
}

#include <parsedatetime.h>
static void getDate(const char *field, uint32_t length, MYSQL_BIND *bind) {

	// result variables
	MYSQL_TIME	*tm=(MYSQL_TIME *)bind->buffer;
	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;
	int32_t	usec=-1;
	bool	isnegative=false;

	// copy into a buffer (to make sure it's null-terminated)
	char	*buffer=new char[length+1];
	charstring::copy(buffer,field,length);
	buffer[length]='\0';

	// parse
	bool	ddmm=charstring::isYes(environment::getValue(
					"SQLR_MYSQL_DATE_DDMM"));
	bool		yyyyddmm=ddmm;
	const char	*yyyyddmmstr=environment::getValue(
					"SQLR_MYSQL_DATE_YYYYDDMM");
	if (yyyyddmmstr) {
		yyyyddmm=charstring::isYes(yyyyddmmstr);
	}
	parseDateTime(buffer,ddmm,yyyyddmm,
				"/-:",&year,&month,&day,
				&hour,&minute,&second,
				&usec,&isnegative);

	// copy back data
	tm->year=(year!=-1)?year:0;
	tm->month=(month!=-1)?month:0;
	tm->day=(day!=-1)?day:0;
	tm->hour=(hour!=-1)?hour:0;
	tm->minute=(minute!=-1)?minute:0;
	tm->second=(second!=-1)?second:0;
	tm->neg=isnegative;

	// clean up
	delete[] buffer;
}

void fixDecimalPoint(char *str) {
	struct lconv	*l=localeconv();
	for (char *c=str; *c; c++) {
		if (*c=='.') {
			*c=*(l->decimal_point);
		}
	}
}

int mysql_stmt_fetch(MYSQL_STMT *stmt) {
	debugFunction();

	// run the query
	MYSQL_ROW	row=mysql_fetch_row(stmt->result);
	if (!row) {
		return MYSQL_NO_DATA;
	}

	// if there are no result binds, just return
	if (!stmt->resultbinds) {
		return 0;
	}

	// copy data into result binds
	uint32_t	*lengths=stmt->result->sqlrcur->
				getRowLengths(stmt->result->previousrow);

	for (uint32_t i=0; i<stmt->result->fieldcount; i++) {

		// Some apps bind fewer bind buffers than there are columns
		// in the result set and "NULL" terminate the list with a
		// bind buffer containing all zeros.  The buffer, length and
		// is_null components are all settable.  If we find bind
		// buffer with those values NULL then assume we've hit the
		// NULL terminator.
		if (charstring::isYes(environment::getValue(
				"SQLR_MYSQL_NULL_TERMINATED_RESULT_BINDS")) &&
			!stmt->resultbinds[i].buffer &&
			!stmt->resultbinds[i].length &&
			!stmt->resultbinds[i].is_null) {
			break;
		}

		// make sure there's something to fetch the data into
		if (!&(stmt->resultbinds[i])) {
			continue;
		}

		if (!charstring::length(row[i])) {

			// set the null indicator (if we can)
			if (stmt->resultbinds[i].is_null) {
				*(stmt->resultbinds[i].is_null)=true;
			}

		} else {

			// set the null indicator (if we can)
			if (stmt->resultbinds[i].is_null) {
				*(stmt->resultbinds[i].is_null)=false;
			}

			// copy data into the buffer...
			switch (stmt->resultbinds[i].buffer_type) {
				case MYSQL_TYPE_NULL:
					stmt->resultbinds[i].buffer=NULL;
					break;
				case MYSQL_TYPE_VAR_STRING:
				case MYSQL_TYPE_STRING:
				case MYSQL_TYPE_TINY_BLOB:
				case MYSQL_TYPE_MEDIUM_BLOB:
				case MYSQL_TYPE_LONG_BLOB:
				case MYSQL_TYPE_BLOB: {

					// initialize len to the buffer size
					unsigned long	len=stmt->
								resultbinds[i].
								buffer_length;

					// if the column itself is shorter than
					// the buffer size, then reset len
					if (lengths[i]<stmt->
						resultbinds[i].buffer_length) {

						len=lengths[i];

						// add 1 for the null terminator
						if (stmt->resultbinds[i].
							buffer_type==
							MYSQL_TYPE_VAR_STRING ||
							stmt->resultbinds[i].
							buffer_type==
							MYSQL_TYPE_STRING) {
							len++;
						}
					}

					// set the output length (if we can)
					if (stmt->resultbinds[i].length) {
						*(stmt->resultbinds[i].
								length)=len;
					}

					// copy data into the buffer (if we can)
					if (stmt->resultbinds[i].buffer) {
						bytestring::copy(
						stmt->resultbinds[i].buffer,
						row[i],len);
					}
					}
					break;
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_DATE:
				case MYSQL_TYPE_TIME:
				case MYSQL_TYPE_DATETIME:
				case MYSQL_TYPE_NEWDATE:
					if (stmt->resultbinds[i].buffer) {
						getDate(row[i],lengths[i],
						&(stmt->resultbinds[i]));
					}
					break;
				case MYSQL_TYPE_TINY:
					if (stmt->resultbinds[i].buffer) {
						*((char *)stmt->
							resultbinds[i].buffer)=
							(char)charstring::
							toInteger(row[i]);
					}
					break;
				case MYSQL_TYPE_SHORT:
					if (stmt->resultbinds[i].buffer) {
						*((uint16_t *)stmt->
							resultbinds[i].buffer)=
							(uint16_t)charstring::
							toInteger(row[i]);
					}
					break;
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_YEAR:
					if (stmt->resultbinds[i].buffer) {
						*((uint32_t *)stmt->
							resultbinds[i].buffer)=
							(uint32_t)charstring::
							toInteger(row[i]);
					}
					break;
				case MYSQL_TYPE_LONGLONG:
				case MYSQL_TYPE_INT24:
					if (stmt->resultbinds[i].buffer) {
						*((uint64_t *)stmt->
							resultbinds[i].buffer)=
							(uint64_t)charstring::
							toInteger(row[i]);
					}
					break;
				case MYSQL_TYPE_FLOAT:
					if (stmt->resultbinds[i].buffer) {
						fixDecimalPoint(row[i]);
						*((float *)stmt->
							resultbinds[i].buffer)=
							(float)charstring::
							toFloatC(row[i]);
					}
					break;
				case MYSQL_TYPE_NEWDECIMAL:
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_DOUBLE:
					if (stmt->resultbinds[i].buffer) {
						fixDecimalPoint(row[i]);
						*((double *)stmt->
							resultbinds[i].buffer)=
							(double)charstring::
							toFloatC(row[i]);
					}
					break;
				case MYSQL_TYPE_ENUM:
				case MYSQL_TYPE_SET:
				case MYSQL_TYPE_GEOMETRY:
					// FIXME: I'm not sure what
					// to do with these types
					//break;
				default:
					if (stmt->resultbinds[i].length) {
						*(stmt->resultbinds[i].
								length)=0;
					}
			}
		}
	}
	return 0;
}

int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind, 
                                    unsigned int column,
                                    unsigned long offset) {
	debugFunction();
	// according to the mysql 5.6 docs, this function is:
	// "To be added."
	return 1;
}

static void processFields(MYSQL_STMT *stmt) {
	debugFunction();

	delete[] stmt->result->fields;
	delete[] stmt->result->lengths;

	sqlrcursor	*sqlrcur=stmt->result->sqlrcur;

	uint32_t colcount=sqlrcur->colCount();
	if (colcount) {

		MYSQL_FIELD	*fields=new MYSQL_FIELD[colcount];
		stmt->result->fields=fields;

		stmt->result->lengths=new unsigned long[colcount];

		for (uint32_t i=0; i<colcount; i++) {

			fields[i].name=const_cast<char *>(
					sqlrcur->getColumnName(i));
			fields[i].table=(char *)"";
			fields[i].def=(char *)"";

			#if defined(COMPAT_MYSQL_4_0) || \
				defined(COMPAT_MYSQL_4_1) || \
				defined(COMPAT_MYSQL_5_0) || \
				defined(COMPAT_MYSQL_5_1)
  			fields[i].org_table=(char *)"";
  			fields[i].db=(char *)"";
			#if defined(COMPAT_MYSQL_4_1) || \
				defined(COMPAT_MYSQL_5_0) || \
				defined(COMPAT_MYSQL_5_1)
  			fields[i].catalog=(char *)"def";
  			fields[i].org_name=(char *)"";
			fields[i].name_length=
				charstring::length(fields[i].name);
			fields[i].org_name_length=
				charstring::length(fields[i].org_name);
			fields[i].table_length=
				charstring::length(fields[i].table);
			fields[i].org_table_length=
				charstring::length(fields[i].org_table);
			fields[i].db_length=
				charstring::length(fields[i].db);
			fields[i].catalog_length=
				charstring::length(fields[i].catalog);
			fields[i].def_length=
				charstring::length(fields[i].def);
			// FIXME: need a character set number here
			fields[i].charsetnr=0;
			#endif
			#endif

			// figure out the column type
			const char	*columntypestring=
						sqlrcur->getColumnType(i);
			int64_t	precision=sqlrcur->getColumnPrecision(i);
			// Occasionally an integer column will come back with a
			// length and scale but no precision.  This has got to
			// be some kind of error.  It causes the column to be
			// defined as a decimal and causes various cascading
			// problems later.  In those cases, also setting the
			// scale to 0 is probably the best course of action.
			// The column will remain an integer and the length
			// instead of precision to size it.
			int64_t	scale=(precision)?sqlrcur->getColumnScale(i):0;
			enum enum_field_types	columntype=
					map_col_type(columntypestring,scale);
			fields[i].type=columntype;

			fields[i].length=sqlrcur->getColumnLength(i);
			fields[i].max_length=sqlrcur->getLongest(i);

			// figure out the flags
			unsigned int	flags=0;
			if (sqlrcur->getColumnIsNullable(i)) {
				flags|=NOT_NULL_FLAG;
			}
			if (sqlrcur->getColumnIsPrimaryKey(i)) {
				flags|=PRI_KEY_FLAG;
			}
			if (sqlrcur->getColumnIsUnique(i)) {
				flags|=UNIQUE_KEY_FLAG;
			}
			if (sqlrcur->getColumnIsPartOfKey(i)) {
				flags|=MULTIPLE_KEY_FLAG;
			}
			if (columntype==MYSQL_TYPE_TINY_BLOB ||
				columntype==MYSQL_TYPE_MEDIUM_BLOB ||
				columntype==MYSQL_TYPE_LONG_BLOB ||
				columntype==MYSQL_TYPE_BLOB) {
				flags|=BLOB_FLAG;
			}
			// FIXME: the "unsigned" test won't work with most db's
			if (charstring::contains(columntypestring,"unsigned") ||
				sqlrcur->getColumnIsUnsigned(i) ||
				isUnsignedTypeChar(columntypestring)) {
				flags|=UNSIGNED_FLAG;
			}
			if (sqlrcur->getColumnIsZeroFilled(i)) {
				flags|=ZEROFILL_FLAG;
			}
			if (sqlrcur->getColumnIsBinary(i) ||
				isBinaryTypeChar(columntypestring)) {
				flags|=BINARY_FLAG;
			}
			if (columntype==MYSQL_TYPE_ENUM) {
				flags|=ENUM_FLAG;
			}
			if (sqlrcur->getColumnIsAutoIncrement(i)) {
				flags|=AUTO_INCREMENT_FLAG;
			}
			if (columntype==MYSQL_TYPE_TIMESTAMP ||
				columntype==MYSQL_TYPE_TIMESTAMP2) {
				flags|=TIMESTAMP_FLAG|ON_UPDATE_NOW_FLAG;
			}
			if (columntype==MYSQL_TYPE_SET) {
				flags|=SET_FLAG;
			}
			if (isNumberTypeChar(columntypestring)) {
				flags|=NUM_FLAG;
			}
			fields[i].flags=flags;

			fields[i].decimals=scale;
		}

		// set the field count
		stmt->result->fieldcount=colcount;
	} else {
		stmt->result->fields=NULL;
		stmt->result->lengths=NULL;
		stmt->result->fieldcount=0;
	}
}

unsigned long mysql_param_count(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_stmt_param_count(stmt);
}

MYSQL_RES *mysql_param_result(MYSQL_STMT *stmt) {
	debugFunction();
	// FIXME: The MySQL docs don't even explain this one
	return NULL;
}



int mysql_fetch(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_stmt_fetch(stmt);
}

int mysql_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind,
			unsigned int column, unsigned long offset) {
	debugFunction();
	return mysql_stmt_fetch_column(stmt,bind,column,offset);
}



MYSQL_RES *mysql_get_metadata(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_stmt_result_metadata(stmt);
}



my_bool mysql_send_long_data(MYSQL_STMT *stmt,
				unsigned int parameternumber,
				const char *data, unsigned long length) {
	debugFunction();
	return mysql_stmt_send_long_data(stmt,parameternumber,data,length);
}


MYSQL_STMT *mysql_stmt_init(MYSQL *mysql) {
	debugFunction();
	MYSQL_STMT	*stmt=new MYSQL_STMT;
	stmt->mysql=mysql;
	stmt->result=new MYSQL_RES;
	stmt->result->stmtbackptr=stmt;
	stmt->result->sqlrcur=new sqlrcursor(mysql->sqlrcon,true);
	stmt->result->errorno=0;
	stmt->result->fields=NULL;
	stmt->result->lengths=NULL;
	return stmt;
}

int mysql_stmt_prepare(MYSQL_STMT *stmt,
				const char *query,
				unsigned long length) {
	debugFunction();
	debugPrintf(query);
	debugPrintf("\n");
	stmt->resultbinds=NULL;
	stmt->result->sqlrcur->prepareQuery(query,length);
	return 0;
}

int mysql_stmt_execute(MYSQL_STMT *stmt) {
	debugFunction();

	setMySQLError(stmt->mysql,NULL,0);

	stmt->result->previousrow=0;
	stmt->result->currentrow=0;
	stmt->result->currentfield=0;
	stmt->result->rowcache.clear();
	sqlrcursor	*sqlrcur=stmt->result->sqlrcur;

	int	retval=!sqlrcur->executeQuery();
	processFields(stmt);

	if (retval) {
		setMySQLError(stmt->mysql,
				sqlrcur->errorMessage(),
				mapErrorNumber(stmt->mysql,
						sqlrcur->errorNumber()));
	}
	return retval;
}

my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_num_rows(stmt->result);
}

my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt) {
	debugFunction();
	return stmt->result->sqlrcur->affectedRows();
}

MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt,
					MYSQL_ROW_OFFSET offset) {
	debugFunction();
	return mysql_row_seek(stmt->result,offset);
}

MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_row_tell(stmt->result);
}

void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset) {
	debugFunction();
	mysql_data_seek(stmt->result,offset);
}



my_bool mysql_stmt_close(MYSQL_STMT *stmt) {
	debugFunction();
	if (stmt) {
		mysql_free_result(stmt->result);
		delete stmt;
	}
	return 0;
}



unsigned int mysql_stmt_errno(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_errno(stmt->mysql);
}

const char *mysql_stmt_error(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_error(stmt->mysql);
}

my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt) {
	debugFunction();
	return stmt->mysql->sqlrcon->getLastInsertId();
}

unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt) {
	debugFunction();
	return mysql_num_fields(stmt->result);
}

const char *mysql_stmt_sqlstate(MYSQL_STMT *stmt) {
	debugFunction();
	// FIXME:
	return "";
}

int mysql_stmt_store_result(MYSQL_STMT *stmt) {
	debugFunction();
	return 0;
}

unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt) {
	debugFunction();
	return stmt->result->sqlrcur->countBindVariables();
}

my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt,
                                    enum enum_stmt_attr_type attrtype,
                                    const void *attr) {
	debugFunction();
	switch (attrtype) {
		case STMT_ATTR_UPDATE_MAX_LENGTH:
			break;
		case STMT_ATTR_CURSOR_TYPE:
			break;
		case STMT_ATTR_PREFETCH_ROWS:
			const unsigned long	*val=
				(const unsigned long *)attr;
			stmt->result->sqlrcur->
				setResultSetBufferSize((uint64_t)(*val));
			break;
	}
	return 0;
}

my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt,
                                    enum enum_stmt_attr_type attrtype,
                                    void *attr) {
	debugFunction();
	switch (attrtype) {
		case STMT_ATTR_UPDATE_MAX_LENGTH:
			break;
		case STMT_ATTR_CURSOR_TYPE:
			break;
		case STMT_ATTR_PREFETCH_ROWS:
			unsigned long	*val=(unsigned long *)attr;
			*val=(unsigned long)stmt->result->sqlrcur->
						getResultSetBufferSize();
			break;
	}
	return 0;
}

my_bool mysql_stmt_bind_param(MYSQL_STMT *stmt, MYSQL_BIND *bind) {
	debugFunction();

	stmt->bindvarnames.clear();

	unsigned long	paramcount=mysql_param_count(stmt);
	for (unsigned long i=0; i<paramcount; i++) {

		// use 1-based index for variable names
		size_t	buffersize=charstring::integerLength((uint32_t)i+1)+1;
		char	*variable=
			(char *)stmt->bindvarnames.allocate(buffersize);
		charstring::printf(variable,buffersize,"%ld",i+1);

		// get the cursor
		sqlrcursor	*cursor=stmt->result->sqlrcur;

		// handle null's first
		if (*(bind[i].is_null)) {
			cursor->inputBind(variable,(char *)NULL);
			continue;
		}

		// handle various datatypes
		switch (bind[i].buffer_type) {
			case MYSQL_TYPE_NULL: {
				cursor->inputBind(variable,(char *)NULL);
				break;
			}
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING: {
				char	*value=(char *)bind[i].buffer;
				cursor->inputBind(variable,value);
				break;
			}
			case MYSQL_TYPE_DATE: {
				MYSQL_TIME	*tm=
						(MYSQL_TIME *)bind[i].buffer;
				cursor->inputBind(variable,
						tm->year,
						tm->month,
						tm->day,
						-1,
						-1,
						-1,
						-1,
						NULL,
						tm->neg);
				break;
			}
			case MYSQL_TYPE_TIME: {
				MYSQL_TIME	*tm=
						(MYSQL_TIME *)bind[i].buffer;
				cursor->inputBind(variable,
						-1,
						-1,
						((tm->neg)?-1:1)*tm->day,
						tm->hour,
						tm->minute,
						tm->second,
						0,
						NULL,
						tm->neg);
				break;
			}
			case MYSQL_TYPE_TIMESTAMP:
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_NEWDATE: {
				MYSQL_TIME	*tm=
						(MYSQL_TIME *)bind[i].buffer;
				cursor->inputBind(variable,
						tm->year,
						tm->month,
						tm->day,
						tm->hour,
						tm->minute,
						tm->second,
						0,
						NULL,
						tm->neg);
				break;
			}
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DOUBLE: {
				double	value=*((double *)bind[i].buffer);
				cursor->inputBind(variable,value,0,0);
				break;
			}
			case MYSQL_TYPE_TINY: {
				int8_t	value=*((int8_t *)bind[i].buffer);
				cursor->inputBind(variable,value);
				break;
			}
			case MYSQL_TYPE_SHORT: {
				int64_t	value=*((int16_t *)bind[i].buffer);
				cursor->inputBind(variable,value);
				break;
			}
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_YEAR: {
				int64_t	value=*((int32_t *)bind[i].buffer);
				cursor->inputBind(variable,value);
				break;
			}
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_INT24: {
				int64_t	value=*((int64_t *)bind[i].buffer);
				cursor->inputBind(variable,value);
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
				return 1;
			}
			default: {
				// FIXME: what should I do here?
				return 1;
			}
		}
	}
	return 0;
}

my_bool mysql_stmt_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *bind) {
	debugFunction();
	stmt->resultbinds=bind;
	return 0;
}

my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt, 
                                          unsigned int paramnumber,
                                          const char *data, 
                                          unsigned long length) {
	debugFunction();
	return 1;
}

MYSQL_RES *mysql_stmt_result_metadata(MYSQL_STMT *stmt) {
	debugFunction();
	return stmt->result;
}

MYSQL_RES *mysql_stmt_param_metadata(MYSQL_STMT *stmt) {
	debugFunction();
	// according to the mysql 5.6 docs:
	// "This function currently does nothing."
	return NULL;
}

my_bool mysql_stmt_free_result(MYSQL_STMT *stmt) {
	debugFunction();
	mysql_free_result(stmt->result);
	return 0;
}

my_bool mysql_stmt_reset(MYSQL_STMT *stmt) {
	debugFunction();
	stmt->result->sqlrcur->clearBinds();
	return 0;
}



int mysql_server_init(int argc, char **argv, char **groups) {
	debugFunction();
	mysql_library_init(argc,argv,groups);
	return 0;
}

void mysql_library_init(int argc, char **argv, char **groups) {
	debugFunction();
	my_init();
}

void mysql_server_end() {
	debugFunction();
	mysql_library_end();
}

void mysql_library_end() {
	debugFunction();
}

my_bool mysql_embedded() {
	debugFunction();
	return 0;
}

int unknownError(MYSQL *mysql) {
	setMySQLError(mysql,"Unknown MySQL error",CR_UNKNOWN_ERROR);
	return CR_UNKNOWN_ERROR;
}

void setMySQLError(MYSQL *mysql, const char *error, unsigned int errorno) {
	debugFunction();
	mysql->errorno=errorno;
	delete[] mysql->error;
	mysql->error=charstring::duplicate(error);
}

unsigned int mapErrorNumber(MYSQL *mysql, int64_t errorno) {
	debugFunction();
	unsigned int	retval=CR_UNKNOWN_ERROR;
	if (mysql->errormap) {
		mysql->errormap->getValue(errorno,&retval);
	} else {
		if (!mysql->backendchecked) {
			mysql->backendismysql=
				!charstring::compare(
					mysql->sqlrcon->identify(),"mysql");
			mysql->backendchecked=true;
		}
		if (mysql->backendismysql) {
			retval=errorno;
		}
	}
	return retval;
}

void loadErrorMap(MYSQL *mysql, const char *errormap) {
	debugFunction();

	debugPrintf("error map file: %s\n",errormap);

	// parse the error map file
	file	mapfile;
	if (!mapfile.open(errormap,O_RDONLY)) {
		debugPrintf("failed to open error map file\n");
		return;
	}

	// create a new errormap
	mysql->errormap=new dictionary< int64_t, unsigned int >;

	// run through the file, line-by line...
	char	*line=NULL;
	ssize_t	linelength=0;
	for (;;) {

		// clean up after the previous line
		delete[] line;

		// get a line
		linelength=mapfile.read(&line,"\n");
		if (linelength<1) {
			return;
		}

		// ignore lines that start with #'s (comments)
		if (line[0]=='#') {
			continue;
		}

		// it should be in <native code>:<mysql code> format...
		const char	*colon=charstring::findFirst(line,':');
		if (colon) {
			int64_t		nativecode=charstring::toInteger(line);
			unsigned int	mysqlcode=
				(unsigned int)charstring::toInteger(colon+1);
			mysql->errormap->setValue(nativecode,mysqlcode);
		}
	}
}

}
