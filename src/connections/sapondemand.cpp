// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <rudiments/dynamiclib.h>


// types...
typedef void		CS_VOID;
typedef	unsigned char	CS_BYTE;
typedef char		CS_CHAR;
typedef	short		CS_SMALLINT;
typedef	int		CS_INT;
typedef	int		CS_BOOL;
typedef	int		CS_RETCODE;
typedef void		CS_CONTEXT;
typedef void		CS_CONNECTION;
typedef void		CS_COMMAND;
typedef void		CS_LOCALE;
typedef unsigned int	CS_MSGNUM;


// structs...
struct CS_DATAFMT {
	CS_CHAR		name[255];
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
	CS_CHAR		svrname[255];
	CS_INT		svrnlen;
	CS_CHAR		proc[255];
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


// function pointers...
CS_RETCODE (*ct_init)(
		CS_CONTEXT *context,
		CS_INT version);

CS_RETCODE (*ct_callback)(
		CS_CONTEXT *context,
		CS_CONNECTION *connection,
		CS_INT action,
		CS_INT type,
		CS_VOID *func);

CS_RETCODE (*ct_con_alloc)(
		CS_CONTEXT *context,
		CS_CONNECTION **connection);

CS_RETCODE (*ct_con_props)(
		CS_CONNECTION *connection,
		CS_INT action,
		CS_INT property,
		CS_VOID *buf,
		CS_INT buflen,
		CS_INT *outlen);

CS_RETCODE (*ct_connect)(
		CS_CONNECTION *connection,
		CS_CHAR *server_name,
		CS_INT snamelen);

CS_RETCODE (*ct_cancel)(
		CS_CONNECTION *connection,
		CS_COMMAND *cmd,
		CS_INT type);

CS_RETCODE (*ct_con_drop)(
		CS_CONNECTION *connection);

CS_RETCODE (*ct_exit)(
		CS_CONTEXT *context,
		CS_INT option);

CS_RETCODE (*ct_close)(
		CS_CONNECTION *connection,
		CS_INT option);

CS_RETCODE (*ct_cmd_alloc)(
		CS_CONNECTION *connection,
		CS_COMMAND **cmdptr);

CS_RETCODE (*ct_cmd_drop)(
		CS_COMMAND *cmd);

CS_RETCODE (*ct_cursor)(
		CS_COMMAND *cmd,
		CS_INT type,
		CS_CHAR *name,
		CS_INT namelen,
		CS_CHAR *text,
		CS_INT tlen,
		CS_INT option);

CS_RETCODE (*ct_command)(
		CS_COMMAND *cmd,
		CS_INT type,
		CS_CHAR *buf,
		CS_INT buflen,
		CS_INT option);

CS_RETCODE (*ct_param)(
		CS_COMMAND *cmd,
		CS_DATAFMT *datafmt,
		CS_VOID *data,
		CS_INT datalen,
		CS_SMALLINT indicator);

CS_RETCODE (*ct_send)(
		CS_COMMAND *cmd);

CS_RETCODE (*ct_results)(
		CS_COMMAND *cmd,
		CS_INT *result_type);

CS_RETCODE (*ct_res_info)(
		CS_COMMAND *cmd,
		CS_INT operation,
		CS_VOID *buf,
		CS_INT buflen,
		CS_INT *outlen);

CS_RETCODE (*ct_bind)(
		CS_COMMAND *cmd,
		CS_INT item,
		CS_DATAFMT *datafmt,
		CS_VOID *buf,
		CS_INT *outputlen,
		CS_SMALLINT *indicator);

CS_RETCODE (*ct_describe)(
		CS_COMMAND *cmd,
		CS_INT item,
		CS_DATAFMT *datafmt);

CS_RETCODE (*ct_fetch)(
		CS_COMMAND *cmd,
		CS_INT type,
		CS_INT offset,
		CS_INT option,
		CS_INT *count);

CS_RETCODE (*cs_ctx_alloc)(
		CS_INT version,
		CS_CONTEXT **outptr);

CS_RETCODE (*cs_config)(
		CS_CONTEXT *context,
		CS_INT action,
		CS_INT property,
		CS_VOID *buf,
		CS_INT buflen,
		CS_INT *outlen);

CS_RETCODE (*cs_locale)(
		CS_CONTEXT *context,
		CS_INT action,
		CS_LOCALE *locale,
		CS_INT type,
		CS_CHAR *buffer,
		CS_INT buflen,
		CS_INT *outlen);

CS_RETCODE (*cs_loc_alloc)(
		CS_CONTEXT *context,
		CS_LOCALE **loc_pointer);

CS_RETCODE (*cs_loc_drop)(
		CS_CONTEXT *context,
		CS_LOCALE *locale);

CS_RETCODE (*cs_ctx_drop)(
		CS_CONTEXT *context);

CS_RETCODE (*cs_dt_crack)(
		CS_CONTEXT *context,
		CS_INT datetype,
		CS_VOID *dateval,
		CS_DATEREC *daterec);


// constants...
#define	CS_VERSION_100		(CS_INT)113

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
static dynamiclib	lib;
static const char	*module="sap";
static const char	*libname="libsybct64.so";
static const char	*pathnames[]={
	"/opt/sap/OCS-16_0/lib",
	NULL
};

static bool openOnDemand() {

	// buffer to store any errors we might get
	stringbuffer	err;

	// look for the library
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
		err.append("\nFailed to load ")->append(module);
		err.append(" libraries.\n");
		err.append(libname)->append(" was not found in any "
						"of these paths:\n");
		path=pathnames;
		while (*path) {
			err.append('	')->append(*path)->append('\n');
			path++;
		}
		stdoutput.write(err.getString());
		return false;
	}

	// open the library
	if (!lib.open(libfilename.getString(),true,true)) {
		goto error;
	}

	// get the functions we need
	ct_init=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_INT version))
			lib.getSymbol("ct_init");
	if (!ct_init) {
		goto error;
	}

	ct_callback=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_CONNECTION *connection,
				CS_INT action,
				CS_INT type,
				CS_VOID *func))
			lib.getSymbol("ct_callback");
	if (!ct_callback) {
		goto error;
	}

	ct_con_alloc=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_CONNECTION **connection))
			lib.getSymbol("ct_con_alloc");
	if (!ct_con_alloc) {
		goto error;
	}

	ct_con_props=(CS_RETCODE (*)(
				CS_CONNECTION *connection,
				CS_INT action,
				CS_INT property,
				CS_VOID *buf,
				CS_INT buflen,
				CS_INT *outlen))
			lib.getSymbol("ct_con_props");
	if (!ct_con_props) {
		goto error;
	}

	ct_connect=(CS_RETCODE (*)(
				CS_CONNECTION *connection,
				CS_CHAR *server_name,
				CS_INT snamelen))
			lib.getSymbol("ct_connect");
	if (!ct_connect) {
		goto error;
	}

	ct_cancel=(CS_RETCODE (*)(
				CS_CONNECTION *connection,
				CS_COMMAND *cmd,
				CS_INT type))
			lib.getSymbol("ct_cancel");
	if (!ct_cancel) {
		goto error;
	}

	ct_con_drop=(CS_RETCODE (*)(
				CS_CONNECTION *connection))
			lib.getSymbol("ct_con_drop");
	if (!ct_con_drop) {
		goto error;
	}

	ct_exit=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_INT option))
			lib.getSymbol("ct_exit");
	if (!ct_exit) {
		goto error;
	}

	ct_close=(CS_RETCODE (*)(
				CS_CONNECTION *connection,
				CS_INT option))
			lib.getSymbol("ct_close");
	if (!ct_close) {
		goto error;
	}

	ct_cmd_alloc=(CS_RETCODE (*)(
				CS_CONNECTION *connection,
				CS_COMMAND **cmdptr))
			lib.getSymbol("ct_cmd_alloc");
	if (!ct_cmd_alloc) {
		goto error;
	}

	ct_cmd_drop=(CS_RETCODE (*)(
				CS_COMMAND *cmd))
			lib.getSymbol("ct_cmd_drop");
	if (!ct_cmd_drop) {
		goto error;
	}

	ct_cursor=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT type,
				CS_CHAR *name,
				CS_INT namelen,
				CS_CHAR *text,
				CS_INT tlen,
				CS_INT option))
			lib.getSymbol("ct_cursor");
	if (!ct_cursor) {
		goto error;
	}

	ct_command=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT type,
				CS_CHAR *buf,
				CS_INT buflen,
				CS_INT option))
			lib.getSymbol("ct_command");
	if (!ct_command) {
		goto error;
	}

	ct_param=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_DATAFMT *datafmt,
				CS_VOID *data,
				CS_INT datalen,
				CS_SMALLINT indicator))
			lib.getSymbol("ct_param");
	if (!ct_param) {
		goto error;
	}

	ct_send=(CS_RETCODE (*)(
				CS_COMMAND *cmd))
			lib.getSymbol("ct_send");
	if (!ct_send) {
		goto error;
	}

	ct_results=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT *result_type))
			lib.getSymbol("ct_results");
	if (!ct_results) {
		goto error;
	}

	ct_res_info=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT operation,
				CS_VOID *buf,
				CS_INT buflen,
				CS_INT *outlen))
			lib.getSymbol("ct_res_info");
	if (!ct_res_info) {
		goto error;
	}

	ct_bind=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT item,
				CS_DATAFMT *datafmt,
				CS_VOID *buf,
				CS_INT *outputlen,
				CS_SMALLINT *indicator))
			lib.getSymbol("ct_bind");
	if (!ct_bind) {
		goto error;
	}

	ct_describe=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT item,
				CS_DATAFMT *datafmt))
			lib.getSymbol("ct_describe");
	if (!ct_describe) {
		goto error;
	}

	ct_fetch=(CS_RETCODE (*)(
				CS_COMMAND *cmd,
				CS_INT type,
				CS_INT offset,
				CS_INT option,
				CS_INT *count))
			lib.getSymbol("ct_fetch");
	if (!ct_fetch) {
		goto error;
	}

	cs_ctx_alloc=(CS_RETCODE (*)(
				CS_INT version,
				CS_CONTEXT **outptr))
			lib.getSymbol("cs_ctx_alloc");
	if (!cs_ctx_alloc) {
		goto error;
	}

	cs_config=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_INT action,
				CS_INT property,
				CS_VOID *buf,
				CS_INT buflen,
				CS_INT *outlen))
			lib.getSymbol("cs_config");
	if (!cs_config) {
		goto error;
	}

	cs_locale=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_INT action,
				CS_LOCALE *locale,
				CS_INT type,
				CS_CHAR *buffer,
				CS_INT buflen,
				CS_INT *outlen))
			lib.getSymbol("cs_locale");
	if (!cs_locale) {
		goto error;
	}

	cs_loc_alloc=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_LOCALE **loc_pointer))
			lib.getSymbol("cs_loc_alloc");
	if (!cs_loc_alloc) {
		goto error;
	}

	cs_loc_drop=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_LOCALE *locale))
			lib.getSymbol("cs_loc_drop");
	if (!cs_loc_drop) {
		goto error;
	}

	cs_ctx_drop=(CS_RETCODE (*)(
				CS_CONTEXT *context))
			lib.getSymbol("cs_ctx_drop");
	if (!cs_ctx_drop) {
		goto error;
	}

	cs_dt_crack=(CS_RETCODE (*)(
				CS_CONTEXT *context,
				CS_INT datetype,
				CS_VOID *dateval,
				CS_DATEREC *daterec))
			lib.getSymbol("cs_dt_crack");
	if (!cs_dt_crack) {
		goto error;
	}

	// success
	return true;

	// error
error:
	char	*error=lib.getError();
	err.append("\nFailed to load ")->append(module);
	err.append(" libraries on-demand.\n");
	err.append(error)->append('\n');
	stdoutput.write(err.getString());
	delete[] error;
	return false;
}
