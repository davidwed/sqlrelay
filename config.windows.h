/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* On some platforms */
/* #undef ADD_NEWLINE_AFTER_READ_FROM_STDIN */

/* On some platforms NewObjectArray requires a cast */
/* #undef CAST_NEW_OBJECT_ARRAY */

/* Version of DB2 */
#define DB2VERSION 8

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
/* #undef HAVE_MYSQL_AUTOCOMMIT */

/* MySQL supports mysql_change_user */
/* #undef HAVE_MYSQL_CHANGE_USER */

/* MySQL supports mysql_commit */
/* #undef HAVE_MYSQL_COMMIT */

/* MySQL supports CR_SERVER_GONE_ERROR */
/* #undef HAVE_MYSQL_CR_SERVER_GONE_ERROR */

/* MySQL supports CR_SERVER_LOST */
/* #undef HAVE_MYSQL_CR_SERVER_LOST */

/* MySQL supports FIELD_TYPE_ENUM */
/* #undef HAVE_MYSQL_FIELD_TYPE_ENUM */

/* MySQL supports FIELD_TYPE_NEWDATE */
/* #undef HAVE_MYSQL_FIELD_TYPE_NEWDATE */

/* MySQL supports FIELD_TYPE_NEWDECIMAL */
/* #undef HAVE_MYSQL_FIELD_TYPE_NEWDECIMAL */

/* MySQL supports FIELD_TYPE_SET */
/* #undef HAVE_MYSQL_FIELD_TYPE_SET */

/* MySQL supports FIELD_TYPE_YEAR */
/* #undef HAVE_MYSQL_FIELD_TYPE_YEAR */

/* MySQL supports MYSQL_GET_SERVER_VERSION */
/* #undef HAVE_MYSQL_GET_SERVER_VERSION */

/* MySQL supports mysql_next_result */
/* #undef HAVE_MYSQL_NEXT_RESULT */

/* MySQL supports MYSQL_OPT_RECONNECT */
/* #undef HAVE_MYSQL_OPT_RECONNECT */

/* MySQL supports mysql_ping */
/* #undef HAVE_MYSQL_PING */

/* MySQL supports mysql_real_connect */
/* #undef HAVE_MYSQL_REAL_CONNECT_FOR_SURE */

/* MySQL supports mysql_rollback */
/* #undef HAVE_MYSQL_ROLLBACK */

/* MySQL supports mysql_select_db */
/* #undef HAVE_MYSQL_SELECT_DB */

/* MySQL supports mysql_set_character_set */
/* #undef HAVE_MYSQL_SET_CHARACTER_SET */

/* MySQL supports mysql_stmt_prepare */
/* #undef HAVE_MYSQL_STMT_PREPARE */

/* oci.h */
/* #undef HAVE_OCI_H */

/* Oracle 8i or greater */
/* #undef HAVE_ORACLE_8i */

/* Some versions of postgresql have PQbinaryTuples */
/* #undef HAVE_POSTGRESQL_PQBINARYTUPLES */

/* Some versions of postgresql have PQexecPrepared */
/* #undef HAVE_POSTGRESQL_PQEXECPREPARED */

/* Some versions of postgresql have PQfmod */
/* #undef HAVE_POSTGRESQL_PQFMOD */

/* Some versions of postgresql have PQoidValue */
/* #undef HAVE_POSTGRESQL_PQOIDVALUE */

/* Some versions of postgresql have PQparameterStatus */
/* #undef HAVE_POSTGRESQL_PQPARAMETERSTATUS */

/* Some versions of postgresql have PQprepare */
/* #undef HAVE_POSTGRESQL_PQPREPARE */

/* Some versions of postgresql have PQserverVersion */
/* #undef HAVE_POSTGRESQL_PQSERVERVERSION */

/* Some versions of postgresql have PQsetClientEncoding */
/* #undef HAVE_POSTGRESQL_PQSETCLIENTENCODING */

/* Some versions of postgresql have PQsetNoticeProcessor */
/* #undef HAVE_POSTGRESQL_PQSETNOTICEPROCESSOR */

/* Do we have readline */
/* #undef HAVE_READLINE */

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

/* Some compliers don't support the inline keyword */
#define INLINE inline

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Mingw32 environment */
#define MINGW32 1

/* Some versions of glibc-2.3 need a fixup */
/* #undef NEED_REDHAT_9_GLIBC_2_3_2_HACK */

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

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Some systems use SQLLEN * in SQLBindCol */
/* #undef SQLBINDCOL_SQLLEN */

/* Some systems use SQLLEN * in SQLBINDPARAMETER */
/* #undef SQLBINDPARAMETER_SQLLEN */

/* Some systems use SQLLEN * in SQLColAttribute */
/* #undef SQLCOLATTRIBUTE_SQLLEN */

/* Some versions of sqlite are transactional */
/* #undef SQLITE_TRANSACTIONAL */

/* Platform supports shared libraries */
#define SQLRELAY_ENABLE_SHARED 1

/* Some systems have sys/vnode.h */
/* #undef SQLRELAY_HAVE_SYS_VNODE_H */

/* Suffix for loadable modules */
#define SQLRELAY_MODULESUFFIX "dll"

/* Some systems use SQLLEN * in SQLRowCount */
/* #undef SQLROWCOUNT_SQLLEN */

/* Version */
#define SQLR_VERSION "0.53"

/* Define to 1 if you have the ANSI C header files. */
/* #undef STDC_HEADERS */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */
