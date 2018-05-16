/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* On some platforms NewObjectArray requires a cast */
/* #undef CAST_NEW_OBJECT_ARRAY */

/* Version of DB2 */
#define DB2VERSION 8

/* Load DB2 libraries at runtime. */
/* #undef DB2_AT_RUNTIME */

/* default group to run SQL Relay as */
#define DEFAULT_RUNASGROUP "nobody"

/* default user to run SQL Relay as */
#define DEFAULT_RUNASUSER "nobody"

/* Use dmalloc */
#define DMALLOC 1

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Some versions of FreeTDS have function definitions */
/* #undef HAVE_FREETDS_FUNCTION_DEFINITIONS */

/* Some versions of FreeTDS have tdsver.h */
/* #undef HAVE_FREETDS_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* iODBC */
/* #undef HAVE_IODBC */

/* Define to 1 if you have the `z' library (-lz). */
/* #undef HAVE_LIBZ */

/* Some versions of mdbtools have mdb_close() */
/* #undef HAVE_MDB_CLOSE */

/* Some versions of mdbtools have 5 param mdb_col_to_string */
/* #undef HAVE_MDB_COL_TO_STRING_5_PARAM */

/* Some versions of mdbtools have mdb_open() with 2 parameters */
/* #undef HAVE_MDB_OPEN_2_PARAM 1 */

/* Some versions of mdbtools have mdb_remove_backends() */
/* #undef HAVE_MDB_REMOVE_BACKENDS */

/* Some versions of mdbtools define mdb_run_query */
/* #undef HAVE_MDB_RUN_QUERY */

/* Some versions of mdbtools define mdb_sql_fetch_row */
/* #undef HAVE_MDB_SQL_FETCH_ROW */

/* Some versions of mdbtools define mdb_sql_run_query */
/* #undef HAVE_MDB_SQL_RUN_QUERY */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* MySQL supports mysql_autocommit */
#define HAVE_MYSQL_AUTOCOMMIT 1

/* MySQL supports mysql_change_user */
#define HAVE_MYSQL_CHANGE_USER 1

/* MySQL supports mysql_commit */
#define HAVE_MYSQL_COMMIT 1

/* MySQL supports CR_SERVER_GONE_ERROR */
#define HAVE_MYSQL_CR_SERVER_GONE_ERROR 1

/* MySQL supports CR_SERVER_LOST */
#define HAVE_MYSQL_CR_SERVER_LOST 1

/* MySQL supports MYSQL_FIELD.name_length */
#define HAVE_MYSQL_FIELD_NAME_LENGTH 1

/* MySQL supports MYSQL_FIELD.org_table */
#define HAVE_MYSQL_FIELD_ORG_TABLE 1

/* MySQL supports MYSQL_FIELD.org_table_length */
#define HAVE_MYSQL_FIELD_ORG_TABLE_LENGTH 1

/* MySQL supports FIELD_TYPE_ENUM */
#define HAVE_MYSQL_FIELD_TYPE_ENUM 1

/* MySQL supports FIELD_TYPE_NEWDATE */
#define HAVE_MYSQL_FIELD_TYPE_NEWDATE 1

/* MySQL supports FIELD_TYPE_NEWDECIMAL */
#define HAVE_MYSQL_FIELD_TYPE_NEWDECIMAL 1

/* MySQL supports FIELD_TYPE_SET */
#define HAVE_MYSQL_FIELD_TYPE_SET 1

/* MySQL supports FIELD_TYPE_YEAR */
#define HAVE_MYSQL_FIELD_TYPE_YEAR 1

/* MySQL supports MYSQL_GET_SERVER_VERSION */
#define HAVE_MYSQL_GET_SERVER_VERSION 1

/* MySQL supports mysql_next_result */
#define HAVE_MYSQL_NEXT_RESULT a1

/* MySQL supports MYSQL_OPT_RECONNECT */
#define HAVE_MYSQL_OPT_RECONNECT 1

/* MySQL supports MYSQL_OPT_SSL_CRL */
#define HAVE_MYSQL_OPT_SSL_CRL 1

/* MySQL supports MYSQL_OPT_SSL_CRLPATH */
#define HAVE_MYSQL_OPT_SSL_CRLPATH 1

/* MySQL supports MYSQL_OPT_SSL_ENFORCE */
/* #undef HAVE_MYSQL_OPT_SSL_ENFORCE */

/* MySQL supports MYSQL_OPT_SSL_MODE */
/* #undef HAVE_MYSQL_OPT_SSL_MODE */

/* MySQL supports MYSQL_OPT_SSL_VERIFY_SERVER_CERT */
#define HAVE_MYSQL_OPT_SSL_VERIFY_SERVER_CERT 1

/* MySQL supports MYSQL_OPT_TLS_VERSION */
/* #undef HAVE_MYSQL_OPT_TLS_VERSION */

/* MySQL supports mysql_ping */
#define HAVE_MYSQL_PING 1

/* MySQL supports mysql_real_connect */
#define HAVE_MYSQL_REAL_CONNECT_FOR_SURE 1

/* MySQL supports MYSQL_REPORT_DATA_TRUNCATION */
#define HAVE_MYSQL_REPORT_DATA_TRUNCATION 1

/* MySQL supports mysql_rollback */
#define HAVE_MYSQL_ROLLBACK 1

/* MySQL supports mysql_select_db */
#define HAVE_MYSQL_SELECT_DB 1

/* MySQL supports mysql_set_character_set */
#define HAVE_MYSQL_SET_CHARACTER_SET 1

/* MySQL supports SSL_MODE_DISABLED */
/* #undef HAVE_MYSQL_SSL_MODE_DISABLED */

/* MySQL supports SSL_MODE_PREFERRED */
/* #undef HAVE_MYSQL_SSL_MODE_PREFERRED */

/* MySQL supports SSL_MODE_REQUIRED */
/* #undef HAVE_MYSQL_SSL_MODE_REQUIRED */

/* MySQL supports SSL_MODE_VERIFY_CA */
/* #undef HAVE_MYSQL_SSL_MODE_VERIFY_CA */

/* MySQL supports SSL_MODE_VERIFY_IDENTITY */
/* #undef HAVE_MYSQL_SSL_MODE_VERIFY_IDENTITY */

/* MySQL supports mysql_ssl_set */
#define HAVE_MYSQL_SSL_SET 1

/* MySQL supports mysql_stmt_prepare */
#define HAVE_MYSQL_STMT_PREPARE 1

/* oci.h */
#define HAVE_OCI_H

/* Oracle 8i or greater */
#define HAVE_ORACLE_8i

/* Some versions of PHP PDO have PDO::ATTR_EMULATE_PREPARES */
#define HAVE_PHP_PDO_ATTR_EMULATE_PREPARES 1

/* Some versions of PHP PDO don't support const zend_function_entry */
#define HAVE_PHP_PDO_CONST_ZEND_FUNCTION_ENTRY 1

/* Some versions of PHP PDO have PDO_PARAM_ZVAL */
#define HAVE_PHP_PDO_PARAM_ZVAL 1

/* Some versions of postgresql have PQbinaryTuples */
#define HAVE_POSTGRESQL_PQBINARYTUPLES 1

/* Some versions of postgresql have PQexecPrepared */
#define HAVE_POSTGRESQL_PQEXECPREPARED 1

/* Some versions of postgresql have PQftable */
#define HAVE_POSTGRESQL_PQFTABLE 1

/* Some versions of postgresql have PQfmod */
#define HAVE_POSTGRESQL_PQFMOD 1

/* Some versions of postgresql have PQoidValue */
#define HAVE_POSTGRESQL_PQOIDVALUE 1

/* Some versions of postgresql have PQparameterStatus */
#define HAVE_POSTGRESQL_PQPARAMETERSTATUS 1

/* Some versions of postgresql have PQprepare */
#define HAVE_POSTGRESQL_PQPREPARE 1

/* Some versions of postgresql have PQsendQueryPrepared */
#define HAVE_POSTGRESQL_PQSENDQUERYPREPARED 1

/* Some versions of postgresql have PQserverVersion */
#define HAVE_POSTGRESQL_PQSERVERVERSION 1

/* Some versions of postgresql have PQsetClientEncoding */
#define HAVE_POSTGRESQL_PQSETCLIENTENCODING 1

/* Some versions of postgresql have PQsetNoticeProcessor */
#define HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR 1

/* Some versions of postgresql have PQsetSingleRowMode */
#define HAVE_POSTGRESQL_PQSETSINGLEROWMODE 1

/* Do we have SIGABRT */
#define HAVE_SIGABRT 1

/* Do we have SIGALRM */
/* #undef HAVE_SIGALRM */

/* Do we have SIGBUS */
/* #undef HAVE_SIGBUS */

/* Do we have SIGCANCEL */
/* #undef HAVE_SIGCANCEL */

/* Do we have SIGCHLD */
/* #undef HAVE_SIGCHLD */

/* Do we have SIGCLD */
/* #undef HAVE_SIGCLD */

/* Do we have SIGCONT */
/* #undef HAVE_SIGCONT */

/* Do we have SIGEMT */
/* #undef HAVE_SIGEMT */

/* Do we have SIGFPE */
#define HAVE_SIGFPE 1

/* Do we have SIGFREEZE */
/* #undef HAVE_SIGFREEZE */

/* Do we have SIGHUP */
/* #undef HAVE_SIGHUP */

/* Do we have SIGILL */
#define HAVE_SIGILL 1

/* Do we have SIGINT */
#define HAVE_SIGINT 1

/* Do we have SIGIO */
/* #undef HAVE_SIGIO */

/* Do we have SIGIOT */
/* #undef HAVE_SIGIOT */

/* Do we have SIGKILL */
/* #undef HAVE_SIGKILL */

/* Do we have SIGLOST */
/* #undef HAVE_SIGLOST */

/* Do we have SIGLWP */
/* #undef HAVE_SIGLWP */

/* Do we have SIGPIPE */
/* #undef HAVE_SIGPIPE */

/* Do we have SIGPOLL */
/* #undef HAVE_SIGPOLL */

/* Do we have SIGPROF */
/* #undef HAVE_SIGPROF */

/* Do we have SIGPWR */
/* #undef HAVE_SIGPWR */

/* Do we have SIGQUIT */
/* #undef HAVE_SIGQUIT */

/* Do we have SIGRTMAX */
/* #undef HAVE_SIGRTMAX */

/* Do we have SIGRTMIN */
/* #undef HAVE_SIGRTMIN */

/* Do we have SIGSEGV */
#define HAVE_SIGSEGV 1

/* Do we have SIGSTKFLT */
/* #undef HAVE_SIGSTKFLT */

/* Do we have SIGSTOP */
/* #undef HAVE_SIGSTOP */

/* Do we have SIGSYS */
/* #undef HAVE_SIGSYS */

/* Do we have SIGTERM */
#define HAVE_SIGTERM 1

/* Do we have SIGTHAW */
/* #undef HAVE_SIGTHAW */

/* Do we have SIGTRAP */
/* #undef HAVE_SIGTRAP */

/* Do we have SIGTSTP */
/* #undef HAVE_SIGTSTP */

/* Do we have SIGTTIN */
/* #undef HAVE_SIGTTIN */

/* Do we have SIGTTOU */
/* #undef HAVE_SIGTTOU */

/* Do we have SIGUNUSED */
/* #undef HAVE_SIGUNUSED */

/* Do we have SIGURG */
/* #undef HAVE_SIGURG */

/* Do we have SIGUSR1 */
/* #undef HAVE_SIGUSR1 */

/* Do we have SIGUSR2 */
/* #undef HAVE_SIGUSR2 */

/* Do we have SIGVTALRM */
/* #undef HAVE_SIGVTALRM */

/* Do we have SIGWAITING */
/* #undef HAVE_SIGWAITING */

/* Do we have SIGWINCH */
/* #undef HAVE_SIGWINCH */

/* Do we have SIGXCPU */
/* #undef HAVE_SIGXCPU */

/* Do we have SIGXFSZ */
/* #undef HAVE_SIGXFSZ */

/* Some systems have SQLConnectW */
/* #undef HAVE_SQLCONNECTW */

/* SQLite supports sqlite3_malloc */
#define HAVE_SQLITE3_FREE_WITH_CHAR 1

/* SQLite supports sqlite3_malloc */
#define HAVE_SQLITE3_MALLOC 1

/* SQLite supports sqlite3_prepare_v2 */
#define HAVE_SQLITE3_PREPARE_V2 1

/* SQLite supports sqlite3_stmt */
#define HAVE_SQLITE3_STMT 1

/* Some systems have SQLROWSETSIZE */
#define HAVE_SQLROWSETSIZE 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Some versions of TCL don't use const char ** arguments */
/* #undef HAVE_TCL_CONSTCHAR */

/* Some versions of TCL don't have Tcl_GetString */
/* #undef HAVE_TCL_GETSTRING */

/* Some versions of TCL don't use const char ** arguments */
#define HAVE_TCL_NEWSTRINGOBJ_CONST_CHAR 1

/* Some versions of TCL don't have Tcl_WideInt */
/* #undef HAVE_TCL_WIDEINT */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* UnixODBC */
/* #undef HAVE_UNIXODBC */

/* Do we have _SIGRTMAX */
/* #undef HAVE__SIGRTMAX */

/* Do we have _SIGRTMIN */
/* #undef HAVE__SIGRTMIN */

/* Some iconv implementations use a const char ** parameter */
/* #undef ICONV_CONST_CHAR */

/* Load Informix libraries at runtime. */
/* #undef INFORMIX_AT_RUNTIME */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Mingw32 environment */
#define MINGW32 1

/* Some versions of glibc-2.3 need a fixup */
/* #undef NEED_REDHAT_9_GLIBC_2_3_2_HACK */

/* Load Oracle libraries at runtime. */
/* #undef ORACLE_AT_RUNTIME */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Some systems use SQLLEN * in SQLBindCol */
#define SQLBINDCOL_SQLLEN 1

/* Some systems use SQLLEN * in SQLBINDPARAMETER */
#define SQLBINDPARAMETER_SQLLEN 1

/* Some systems use SQLLEN * in SQLColAttribute */
#define SQLCOLATTRIBUTE_SQLLEN 1

/* Some versions of sqlite are transactional */
/* #undef SQLITE_TRANSACTIONAL */

/* replacement for "sqlr" */
#define SQLR "@SQLR@"

/* replacement for "sqlrelay" */
#define SQLRELAY "@SQLRELAY@"

/* Platform supports shared libraries */
#define SQLRELAY_ENABLE_SHARED 1

/* Some systems have sys/vnode.h */
/* #undef SQLRELAY_HAVE_SYS_VNODE_H */

/* Suffix for loadable modules */
#define SQLRELAY_MODULESUFFIX "dll"

/* Some systems use SQLLEN * in SQLRowCount */
#define SQLROWCOUNT_SQLLEN 1

/* Version */
#define SQLR_VERSION "@SQLR_VERSION@"

/* replacement for "SQL Relay" */
#define SQL_RELAY "@SQL_RELAY@"

/* Define to 1 if you have the ANSI C header files. */
/* #undef STDC_HEADERS */

/* Load SAP/Sybase libraries at runtime. */
/* #undef SYBASE_AT_RUNTIME */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */
