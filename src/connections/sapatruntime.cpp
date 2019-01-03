// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <rudiments/dynamiclib.h>
#include <rudiments/file.h>


// types...
typedef void		CS_VOID;
typedef	unsigned char	CS_BYTE;
typedef char		CS_CHAR;
typedef	short		CS_SMALLINT;
typedef	int		CS_INT;
typedef	int		CS_BOOL;
typedef	int		CS_RETCODE;
typedef unsigned int	CS_MSGNUM;


// structs...
struct CS_CONTEXT;
struct CS_CONNECTION;
struct CS_COMMAND;
struct CS_LOCALE;

struct CS_DATAFMT {
	CS_CHAR		name[256];
	CS_INT		namelen;
	CS_INT		datatype;
	CS_INT		format;
	CS_INT		maxlength;
	CS_INT		scale;
	CS_INT		precision;
	CS_INT		status;
	CS_INT		count;
	CS_INT		usertype;
	CS_LOCALE	*locale;
};

struct CS_DATEREC {
	CS_INT		dateyear;
	CS_INT		datemonth;
	CS_INT		datedmonth;
	CS_INT		datedyear;
	CS_INT		datedweek;
	CS_INT		datehour;
	CS_INT		dateminute;
	CS_INT		datesecond;
	CS_INT		datemsecond;
	CS_INT		datetzone;
	CS_INT		datesecfrac;
	CS_INT		datesecprec;
};

struct CS_DATETIME {
	CS_INT		dtdays;
	CS_INT		dttime;
};

struct CS_CLIENTMSG {
	CS_INT		severity;
	CS_MSGNUM	msgnumber;
	CS_CHAR		msgstring[1024];
	CS_INT		msgstringlen;
	CS_INT		osnumber;
	CS_CHAR		osstring[1024];
	CS_INT		osstringlen;
	CS_INT		status;
	CS_BYTE		sqlstate[8];
	CS_INT		sqlstatelen;
};

struct CS_SERVERMSG {
	CS_MSGNUM	msgnumber;
	CS_INT		state;
	CS_INT		severity;
	CS_CHAR		text[1024];
	CS_INT		textlen;
	CS_CHAR		svrname[256];
	CS_INT		svrnlen;
	CS_CHAR		proc[256];
	CS_INT		proclen;
	CS_INT		line;
	CS_INT		status;
	CS_BYTE		sqlstate[8];
	CS_INT		sqlstatelen;
};

#define CS_LAYER(L)	(CS_MSGNUM) (((L) >> 24) & 0xff)
#define CS_ORIGIN(O)	(CS_MSGNUM) (((O) >> 16) & 0xff)
#define CS_SEVERITY(S)	(CS_MSGNUM) (((S) >> 8) & 0xff)
#define CS_NUMBER(N)	(CS_MSGNUM) ((N) & 0xff)


// create aliases to prevent collisions
// eg. cs_ctx_alloc calls cs_config and if my function pointer is named
// cs_config, then when cs_ctx_alloc calls cs_config it ends up jumping to the
// location of the variable (not the location stored in the variable), rather
// than the location of the function.
#define ct_init ct_init_ptr
#define ct_callback ct_callback_ptr
#define ct_con_alloc ct_con_alloc_ptr
#define ct_con_props ct_con_props_ptr
#define ct_connect ct_connect_ptr
#define ct_cancel ct_cancel_ptr
#define ct_con_drop ct_con_drop_ptr
#define ct_exit ct_exit_ptr
#define ct_close ct_close_ptr
#define ct_cmd_alloc ct_cmd_alloc_ptr
#define ct_cmd_drop ct_cmd_drop_ptr
#define ct_cursor ct_cursor_ptr
#define ct_command ct_command_ptr
#define ct_param ct_param_ptr
#define ct_send ct_send_ptr
#define ct_results ct_results_ptr
#define ct_res_info ct_res_info_ptr
#define ct_bind ct_bind_ptr
#define ct_describe ct_describe_ptr
#define ct_fetch ct_fetch_ptr
#define cs_ctx_alloc cs_ctx_alloc_ptr
#define cs_config cs_config_ptr
#define cs_locale cs_locale_ptr
#define cs_loc_alloc cs_loc_alloc_ptr
#define cs_loc_drop cs_loc_drop_ptr
#define cs_ctx_drop cs_ctx_drop_ptr
#define cs_dt_crack cs_dt_crack_ptr


// function pointers...
CS_RETCODE (*ct_init)(
		CS_CONTEXT *,
		CS_INT);

CS_RETCODE (*ct_callback)(
		CS_CONTEXT *,
		CS_CONNECTION *,
		CS_INT,
		CS_INT,
		CS_VOID *);

CS_RETCODE (*ct_con_alloc)(
		CS_CONTEXT *,
		CS_CONNECTION **);

CS_RETCODE (*ct_con_props)(
		CS_CONNECTION *,
		CS_INT,
		CS_INT,
		CS_VOID *,
		CS_INT,
		CS_INT *);

CS_RETCODE (*ct_connect)(
		CS_CONNECTION *,
		CS_CHAR *,
		CS_INT);

CS_RETCODE (*ct_cancel)(
		CS_CONNECTION *,
		CS_COMMAND *,
		CS_INT);

CS_RETCODE (*ct_con_drop)(
		CS_CONNECTION *);

CS_RETCODE (*ct_exit)(
		CS_CONTEXT *,
		CS_INT);

CS_RETCODE (*ct_close)(
		CS_CONNECTION *,
		CS_INT);

CS_RETCODE (*ct_cmd_alloc)(
		CS_CONNECTION *,
		CS_COMMAND **);

CS_RETCODE (*ct_cmd_drop)(
		CS_COMMAND *);

CS_RETCODE (*ct_cursor)(
		CS_COMMAND *,
		CS_INT,
		CS_CHAR *,
		CS_INT,
		CS_CHAR *,
		CS_INT,
		CS_INT);

CS_RETCODE (*ct_command)(
		CS_COMMAND *,
		CS_INT,
		CS_CHAR *,
		CS_INT,
		CS_INT);

CS_RETCODE (*ct_param)(
		CS_COMMAND *,
		CS_DATAFMT *,
		CS_VOID *,
		CS_INT,
		CS_SMALLINT);

CS_RETCODE (*ct_send)(
		CS_COMMAND *);

CS_RETCODE (*ct_results)(
		CS_COMMAND *,
		CS_INT *);

CS_RETCODE (*ct_res_info)(
		CS_COMMAND *,
		CS_INT,
		CS_VOID *,
		CS_INT,
		CS_INT *);

CS_RETCODE (*ct_bind)(
		CS_COMMAND *,
		CS_INT,
		CS_DATAFMT *,
		CS_VOID *,
		CS_INT *,
		CS_SMALLINT *);

CS_RETCODE (*ct_describe)(
		CS_COMMAND *,
		CS_INT,
		CS_DATAFMT *);

CS_RETCODE (*ct_fetch)(
		CS_COMMAND *,
		CS_INT,
		CS_INT,
		CS_INT,
		CS_INT *);

CS_RETCODE (*cs_ctx_alloc)(
		CS_INT,
		CS_CONTEXT **);

CS_RETCODE (*cs_config)(
		CS_CONTEXT *,
		CS_INT,
		CS_INT,
		CS_VOID *,
		CS_INT,
		CS_INT *);

CS_RETCODE (*cs_locale)(
		CS_CONTEXT *,
		CS_INT,
		CS_LOCALE *,
		CS_INT,
		CS_CHAR *,
		CS_INT,
		CS_INT *);

CS_RETCODE (*cs_loc_alloc)(
		CS_CONTEXT *,
		CS_LOCALE **);

CS_RETCODE (*cs_loc_drop)(
		CS_CONTEXT *,
		CS_LOCALE *);

CS_RETCODE (*cs_ctx_drop)(
		CS_CONTEXT *);

CS_RETCODE (*cs_dt_crack)(
		CS_CONTEXT *,
		CS_INT,
		CS_VOID *,
		CS_DATEREC *);


// constants...
#define CS_VERSION_100		(CS_INT)113

#define	CS_SET			(CS_INT)34

#define	CS_MESSAGE_CB		(CS_INT)9119

#define	CS_UNUSED		(CS_INT)(-99999)

#define	CS_SUCCEED		(CS_RETCODE)1
#define CS_FAIL			(CS_RETCODE)0

#define	CS_SERVERMSG_CB		(CS_INT)2
#define	CS_CLIENTMSG_CB		(CS_INT)3

#define CS_USERNAME		(CS_INT)9100
#define CS_PASSWORD		(CS_INT)9101
#define CS_APPNAME		(CS_INT)9102
#define CS_HOSTNAME		(CS_INT)9103
#define CS_PACKETSIZE		(CS_INT)9107
#define CS_LOC_PROP		(CS_INT)9125
#define CS_SEC_ENCRYPTION	(CS_INT)9135

#define CS_TRUE			(CS_BOOL)1
#define CS_FALSE		(CS_BOOL)0

#define CS_SYB_LANG		(CS_INT)8
#define CS_SYB_CHARSET		(CS_INT)9

#define CS_LANG_CMD		(CS_INT)148
#define CS_RPC_CMD		(CS_INT)149

#define CS_ROW_RESULT		(CS_INT)4040
#define CS_CURSOR_RESULT	(CS_INT)4041
#define CS_PARAM_RESULT		(CS_INT)4042
#define CS_COMPUTE_RESULT	(CS_INT)4045
#define CS_CMD_SUCCEED		(CS_INT)4047
#define CS_CMD_FAIL		(CS_INT)4048

#define CS_ROW_COUNT		(CS_INT)800
#define CS_NUMDATA		(CS_INT)803

#define CS_CANCEL_CURRENT	(CS_INT)6000
#define CS_CANCEL_ALL		(CS_INT)6001

#define CS_ILLEGAL_TYPE		(CS_INT)(-1)
#define CS_CHAR_TYPE		(CS_INT)0
#define CS_BINARY_TYPE		(CS_INT)1
#define CS_LONGCHAR_TYPE	(CS_INT)2
#define CS_LONGBINARY_TYPE	(CS_INT)3
#define CS_TEXT_TYPE		(CS_INT)4
#define CS_IMAGE_TYPE		(CS_INT)5
#define CS_TINYINT_TYPE		(CS_INT)6
#define CS_SMALLINT_TYPE	(CS_INT)7
#define CS_INT_TYPE		(CS_INT)8
#define CS_REAL_TYPE		(CS_INT)9
#define CS_FLOAT_TYPE		(CS_INT)10
#define CS_BIT_TYPE		(CS_INT)11
#define CS_DATETIME_TYPE	(CS_INT)12
#define CS_DATETIME4_TYPE	(CS_INT)13
#define CS_MONEY_TYPE		(CS_INT)14
#define CS_MONEY4_TYPE		(CS_INT)15
#define CS_NUMERIC_TYPE		(CS_INT)16
#define CS_DECIMAL_TYPE		(CS_INT)17
#define CS_VARCHAR_TYPE		(CS_INT)18
#define CS_VARBINARY_TYPE	(CS_INT)19
#define CS_LONG_TYPE		(CS_INT)20
#define CS_SENSITIVITY_TYPE	(CS_INT)21
#define CS_BOUNDARY_TYPE	(CS_INT)22
#define CS_VOID_TYPE		(CS_INT)23
#define CS_USHORT_TYPE		(CS_INT)24
#define CS_BIGINT_TYPE		(CS_INT)30	
#define CS_UBIGINT_TYPE		(CS_INT)33	
#define CS_XML_TYPE		(CS_INT)34

#define CS_FMT_NULLTERM		(CS_INT)0x1

#define CS_CURSOR_DECLARE	(CS_INT)700
#define CS_CURSOR_OPEN		(CS_INT)701
#define CS_CURSOR_ROWS		(CS_INT)703
#define CS_CURSOR_CLOSE		(CS_INT)706

#define CS_DEALLOC		(CS_INT)711

#define CS_READ_ONLY		(CS_INT)0x0002

#define CS_EXTERNAL_ERR		(CS_RETCODE)(-200)
#define CS_ROW_FAIL		(CS_RETCODE)(CS_EXTERNAL_ERR - 3)
#define CS_END_RESULTS		(CS_RETCODE)(CS_EXTERNAL_ERR - 5)

#define CS_FORCE_CLOSE		(CS_INT)301

#define CS_FMT_UNUSED		(CS_INT)0x0

#define CS_KEY			(CS_INT)0x2
#define CS_VERSION_KEY		(CS_INT)0x4
#define CS_CANBENULL		(CS_INT)0x20
#define CS_INPUTVALUE		(CS_INT)0x100
#define CS_IDENTITY		(CS_INT)0x8000

#define CS_RETURN		(CS_INT)0x400

#define CS_LC_ALL		(CS_INT)7

#define CS_SV_RETRY_FAIL	(CS_INT)2


// dlopen infrastructure...
static bool		alreadyopen=false;
static dynamiclib	lib;

static bool loadLibraries(stringbuffer *errormessage) {

	// don't open multiple times...
	if (alreadyopen) {
		return true;
	}
	alreadyopen=true;

	// build path names
	const char	**pathnames=new const char *[12];
	uint16_t	p=0;
	stringbuffer	libdir16;
	stringbuffer	libdir15;
	stringbuffer	libdir125;
	const char	*sybase=environment::getValue("SYBASE");
	if (!charstring::isNullOrEmpty(sybase)) {
		libdir16.append(sybase)->append("/OCS-16_0/lib");
		libdir15.append(sybase)->append("/OCS-15_0/lib");
		libdir125.append(sybase)->append("/OCS-12_5/lib");
		pathnames[p++]=libdir16.getString();
		pathnames[p++]=libdir15.getString();
		pathnames[p++]=libdir125.getString();
	}
	pathnames[p++]="/opt/sap/OCS-16_0/lib";
	pathnames[p++]="/opt/sybase/OCS-15_0/lib";
	pathnames[p++]="/opt/sybase/OCS-12_5/lib";
	pathnames[p++]="/opt/sybase-12.5/OCS-12_5/lib";
	pathnames[p++]="/opt/sybase-11.9.2/lib";
	pathnames[p++]="/opt/sybase-11.0.3.3/lib";
	pathnames[p++]="/opt/sybase/lib";
	pathnames[p++]="/usr/local/sybase/lib";
	pathnames[p++]=NULL;

	// look for the library
	const char	*libname="libsybblk64.so";
	stringbuffer	libfilename;
	const char	**path=pathnames;
	while (*path) {
		libfilename.clear();
		libfilename.append(*path)->append('/')->append(libname);
		if (file::readable(libfilename.getString())) {
			break;
		}
		path++;
	}
	if (!*path) {
		errormessage->clear();
		errormessage->append("\nFailed to load SAP libraries.\n");
		if (charstring::isNullOrEmpty(sybase)) {
			errormessage->append("SYBASE not set and ");
		}
		errormessage->append(libname)->append(" was not found in any "
							"of these paths:\n");
		path=pathnames;
		while (*path) {
			errormessage->append('	')->append(*path)->append('\n');
			path++;
		}
		return false;
	}

	// open the library
	if (!lib.open(libfilename.getString(),true,true)) {
		goto error;
	}

	// get the functions we need
	ct_init=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_INT))
			lib.getSymbol("ct_init");
	if (!ct_init) {
		goto error;
	}

	ct_callback=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_CONNECTION *,
				CS_INT,
				CS_INT,
				CS_VOID *))
			lib.getSymbol("ct_callback");
	if (!ct_callback) {
		goto error;
	}

	ct_con_alloc=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_CONNECTION **))
			lib.getSymbol("ct_con_alloc");
	if (!ct_con_alloc) {
		goto error;
	}

	ct_con_props=(CS_RETCODE (*)(
				CS_CONNECTION *,
				CS_INT,
				CS_INT,
				CS_VOID *,
				CS_INT,
				CS_INT *))
			lib.getSymbol("ct_con_props");
	if (!ct_con_props) {
		goto error;
	}

	ct_connect=(CS_RETCODE (*)(
				CS_CONNECTION *,
				CS_CHAR *,
				CS_INT))
			lib.getSymbol("ct_connect");
	if (!ct_connect) {
		goto error;
	}

	ct_cancel=(CS_RETCODE (*)(
				CS_CONNECTION *,
				CS_COMMAND *,
				CS_INT))
			lib.getSymbol("ct_cancel");
	if (!ct_cancel) {
		goto error;
	}

	ct_con_drop=(CS_RETCODE (*)(
				CS_CONNECTION *))
			lib.getSymbol("ct_con_drop");
	if (!ct_con_drop) {
		goto error;
	}

	ct_exit=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_INT))
			lib.getSymbol("ct_exit");
	if (!ct_exit) {
		goto error;
	}

	ct_close=(CS_RETCODE (*)(
				CS_CONNECTION *,
				CS_INT))
			lib.getSymbol("ct_close");
	if (!ct_close) {
		goto error;
	}

	ct_cmd_alloc=(CS_RETCODE (*)(
				CS_CONNECTION *,
				CS_COMMAND **))
			lib.getSymbol("ct_cmd_alloc");
	if (!ct_cmd_alloc) {
		goto error;
	}

	ct_cmd_drop=(CS_RETCODE (*)(
				CS_COMMAND *))
			lib.getSymbol("ct_cmd_drop");
	if (!ct_cmd_drop) {
		goto error;
	}

	ct_cursor=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT,
				CS_CHAR *,
				CS_INT,
				CS_CHAR *,
				CS_INT,
				CS_INT))
			lib.getSymbol("ct_cursor");
	if (!ct_cursor) {
		goto error;
	}

	ct_command=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT,
				CS_CHAR *,
				CS_INT,
				CS_INT))
			lib.getSymbol("ct_command");
	if (!ct_command) {
		goto error;
	}

	ct_param=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_DATAFMT *,
				CS_VOID *,
				CS_INT,
				CS_SMALLINT))
			lib.getSymbol("ct_param");
	if (!ct_param) {
		goto error;
	}

	ct_send=(CS_RETCODE (*)(
				CS_COMMAND *))
			lib.getSymbol("ct_send");
	if (!ct_send) {
		goto error;
	}

	ct_results=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT *))
			lib.getSymbol("ct_results");
	if (!ct_results) {
		goto error;
	}

	ct_res_info=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT,
				CS_VOID *,
				CS_INT,
				CS_INT *))
			lib.getSymbol("ct_res_info");
	if (!ct_res_info) {
		goto error;
	}

	ct_bind=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT,
				CS_DATAFMT *,
				CS_VOID *,
				CS_INT *,
				CS_SMALLINT *))
			lib.getSymbol("ct_bind");
	if (!ct_bind) {
		goto error;
	}

	ct_describe=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT,
				CS_DATAFMT *))
			lib.getSymbol("ct_describe");
	if (!ct_describe) {
		goto error;
	}

	ct_fetch=(CS_RETCODE (*)(
				CS_COMMAND *,
				CS_INT,
				CS_INT,
				CS_INT,
				CS_INT *))
			lib.getSymbol("ct_fetch");
	if (!ct_fetch) {
		goto error;
	}

	cs_ctx_alloc=(CS_RETCODE (*)(
				CS_INT,
				CS_CONTEXT **))
			lib.getSymbol("cs_ctx_alloc");
	if (!cs_ctx_alloc) {
		goto error;
	}

	cs_config=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_INT,
				CS_INT,
				CS_VOID *,
				CS_INT,
				CS_INT *))
			lib.getSymbol("cs_config");
	if (!cs_config) {
		goto error;
	}

	cs_locale=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_INT,
				CS_LOCALE *,
				CS_INT,
				CS_CHAR *,
				CS_INT,
				CS_INT *))
			lib.getSymbol("cs_locale");
	if (!cs_locale) {
		goto error;
	}

	cs_loc_alloc=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_LOCALE **))
			lib.getSymbol("cs_loc_alloc");
	if (!cs_loc_alloc) {
		goto error;
	}

	cs_loc_drop=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_LOCALE *))
			lib.getSymbol("cs_loc_drop");
	if (!cs_loc_drop) {
		goto error;
	}

	cs_ctx_drop=(CS_RETCODE (*)(
				CS_CONTEXT *))
			lib.getSymbol("cs_ctx_drop");
	if (!cs_ctx_drop) {
		goto error;
	}

	cs_dt_crack=(CS_RETCODE (*)(
				CS_CONTEXT *,
				CS_INT,
				CS_VOID *,
				CS_DATEREC *))
			lib.getSymbol("cs_dt_crack");
	if (!cs_dt_crack) {
		goto error;
	}

	// success
	return true;

	// error
error:
	char	*error=lib.getError();
	errormessage->clear();
	errormessage->append("\nFailed to load SAP libraries.\n");
	errormessage->append(error)->append('\n');
	#ifndef _WIN32
	if (charstring::contains(error,"No such file or directory")) {
		errormessage->append("\n(NOTE: The error message above may "
					"be misleading.  Most likely it means "
					"that a library that ");
		errormessage->append(libname);
		errormessage->append(" depends on could not be located.  ");
		errormessage->append(libname)->append(" was found in ");
		errormessage->append(*path)->append(".  Verify that ");
		errormessage->append(*path);
		errormessage->append(" and directories containing each of "
					"the libraries that ");
		errormessage->append(libname);
		errormessage->append(" depends on are included in your "
					"LD_LIBRARY_PATH, /etc/ld.so.conf, "
					"or /etc/ld.so.conf.d.  Try using "
					"ldd to show ");
		errormessage->append(*path)->append('/')->append(libname);
		errormessage->append("'s dependencies.)\n");
	}
	#endif
	delete[] error;
	return false;
}
