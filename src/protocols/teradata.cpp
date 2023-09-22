// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/inetsocketclient.h>
#include <rudiments/bytestring.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/snooze.h>
#include <rudiments/error.h>
#include <rudiments/dynamicarray.h>
#include <rudiments/process.h>
#include <rudiments/character.h>
#include <rudiments/sys.h>
#include <rudiments/sha256.h>

//#define DECRYPT 1

#ifdef DECRYPT
	#include <openssl/conf.h>
	#include <openssl/dh.h>
	#include <openssl/evp.h>
	#include <openssl/err.h>
#endif

// passthrough modes
enum passthroughmode_t {
	PASSTHROUGHMODE_ENABLED,
	PASSTHROUGHMODE_HYBRID,
	PASSTHROUGHMODE_DISABLED
};


// auth mechs
#define	MECH_NONE	0
#define	MECH_TD2	1
#define	MECH_TDNEGO	2
#define	MECH_LDAP	3
#define	MECH_KRB	4
#define	MECH_KRBCOMPAT	5
const char	*mechstr[]={
	"none",
	"td2",
	"tdnego",
	"ldap",
	"krb",
	"krbcompat"
};
byte_t	td2mech[]={
	0x2B, 0x06, 0x01, 0x04, 0x01, 0x81, 0x3F, 0x01,
	0x87, 0x74, 0x01, 0x01, 0x09
};
byte_t	tdnegomech[]={
	0x2B, 0x06, 0x01, 0x04, 0x01, 0x81, 0x3F, 0x01,
	0x87, 0x74, 0x01, 0x14
};
byte_t	ldapmech[]={
	0x2A, 0x86, 0x48, 0x86, 0xF7, 0x12, 0x01, 0x02,
	0x02
};
byte_t	krbmech[]={
	0x2B, 0x06, 0x01, 0x05, 0x05, 0x02
};
byte_t	krbcompatmech[]={
	0x2B, 0x06, 0x01, 0x04, 0x01, 0x81, 0xE0, 0x1A,
	0x04, 0x82, 0x2E, 0x01, 0x03,
};


// encryption
#define	SUPPORTED_ALGORITHMS	0xE1
#define	SUPPORTED_ALGORITHM	0xE2

#define CONF_ALG		0xD0
#define INT_ALG			0xD1
#define KEX_ALG			0xD2
#define CONF_ALG_MODE		0xD3
#define CONF_ALG_PADDING	0xD4
#define CONF_ALG_KEY_SIZE	0xD5
#define KEX_ALG_KEY_SIZE	0xD6
const char	*algfieldname[]={
	"conf alg",		// 0xD0
	"int alg",		// 0xD1
	"kex alg",		// 0xD2
	"mode",			// 0xD3
	"padding",		// 0xD4
	"conf alg key size",	// 0xD5
	"kex alg key size"	// 0xD6
};


// (values correspond to <LegalValues> section of TdgssLibraryConfigFile.xml)
#define	ALG_NONE	0x00
#define	ALG_BLOWFISH	0x01
#define	ALG_AES		0x02
#define	ALG_MD5		0x03
#define	ALG_SHA1	0x04
#define	ALG_DH		0x05
#define	ALG_SHA256	0x06
#define	ALG_SHA512	0x07
const char	*algstr[]={
	"none",
	"Blowfish",
	"AES",
	"MD5",
	"SHA1",
	"DH",
	"SHA256",
	"SHA512"
};
#define	CONF_ALG_MODE_NONE	0x00
#define	CONF_ALG_MODE_CBC	0x01
#define	CONF_ALG_MODE_CFB	0x02
#define	CONF_ALG_MODE_ECB	0x03
#define	CONF_ALG_MODE_OFB	0x04
#define	CONF_ALG_MODE_GCM	0x05
#define	CONF_ALG_MODE_CCM	0x06
#define	CONF_ALG_MODE_CTR	0x07
const char	*confalgmodestr[]={
	"none",
	"CBC",
	"CFB",
	"ECB",
	"OFB",
	"GCM",
	"CCM",
	"CTR"
};
#define	CONF_ALG_PADDING_NONE		0x00
#define	CONF_ALG_PADDING_OAEP		0x01
#define	CONF_ALG_PADDING_UNKNOWN	0x02
#define	CONF_ALG_PADDING_PKCS1		0x03
#define	CONF_ALG_PADDING_PKCS5		0x04
#define	CONF_ALG_PADDING_SSL3		0x05
const char	*confalgpaddingstr[]={
	"none",
	"OAEP with digest and MGF padding",
	"???",
	"PKCS1 padding",
	"PKCS5 padding",
	"SSL3 padding"
};
#define	QOP_NONE				0
#define	QOP_AES_K128_GCM_PKCS5Padding_SHA2	1
#define	QOP_AES_K128_CBC_PKCS5Padding_SHA1	2
#define	QOP_AES_K192_GCM_PKCS5Padding_SHA2	3
#define	QOP_AES_K192_CBC_PKCS5Padding_SHA1	4
#define	QOP_AES_K256_GCM_PKCS5Padding_SHA2	5
#define	QOP_AES_K256_CBC_PKCS5Padding_SHA1	6
const char	*qopstr[]={
	"NONE",
	"AES_K128_GCM_PKCS5Padding_SHA2",
	"AES_K128_CBC_PKCS5Padding_SHA1",
	"AES_K192_GCM_PKCS5Padding_SHA2",
	"AES_K192_CBC_PKCS5Padding_SHA1",
	"AES_K256_GCM_PKCS5Padding_SHA2",
	"AES_K256_CBC_PKCS5Padding_SHA1"
};


#define LAN_HEADER_SIZE	52


// kinds of messages
#define COPKIND_ASSIGN		1
#define COPKIND_REASSIGN	2
#define COPKIND_CONNECT		3
#define COPKIND_RECONNECT	4
#define COPKIND_START		5
#define COPKIND_CONTINUE	6
#define COPKIND_ABORT		7
#define COPKIND_LOGOFF		8
#define COPKIND_TEST		9
#define COPKIND_CFG		10
#define COPKIND_AUTHMETHODS	11
#define COPKIND_SSOREQ		12
#define COPKIND_ELICITDATA	13
#define COPKIND_DEFAULTCONNECT	254
#define COPKIND_DIRECT		255


// activity types
#define NOT_AVAILABLE 0
#define SQL_SELECT 1
#define SQL_INSERT 2
#define SQL_UPDATE 3
#define UPDATE__RETRIEVING 4
#define SQL_DELETE 5
#define SQL_CREATE_TABLE 6
#define SQL_ALTER_TABLE 7
#define SQL_CREATE_VIEW 8
#define SQL_CREATE_MACRO 9
#define SQL_DROP_TABLE 10
#define SQL_DROP_VIEW 11
#define SQL_DROP_MACRO 12
#define SQL_DROP_INDEX 13
#define SQL_RENAME_TABLE 14
#define SQL_RENAME_VIEW 15
#define SQL_RENAME_MACRO 16
#define SQL_CREATE_INDEX 17
#define SQL_CREATE_DATABASE 18
#define SQL_CREATE_USER 19
#define SQL_GRANT 20
#define SQL_REVOKE 21
#define GIVE 22
#define SQL_DROP_DATABASE 23
#define SQL_MODIFY_DATABASE 24
#define SQL_DATABASE 25
#define SQL_BEGIN_TRANSACTION 26
#define SQL_END_TRANSACTION 27
#define SQL_ABORT 28
#define SQL_NULL 29
#define SQL_EXECUTE 30
#define SQL_COMMENT_SET 31
#define SQL_COMMENT 32
#define SQL_ECHO 33
#define REPLACE_VIEW 34
#define REPLACE_MACRO 35
#define SQL_CHECKPOINT 36
#define DELETE_JOURNAL 37
#define SQL_ROLLBACK 38
#define RELEASE_LOCK 39
#define HUT_CONFIG 40
#define VERIFYCHECKPOINT 41
#define DUMP_JOURNAL 42
#define DUMP 43
#define RESTORE 44
#define ROLLFORWARD 45
#define SQL_DELETE_DATABASE 46
#define INTERNAL_USE_ONLY_FOR_CRASH_DUMPS1 47
#define INTERNAL_USE_ONLY_FOR_CRASH_DUMPS2 48
#define SQL_SHOW 49
#define SQL_HELP 50
#define BEGIN_LOADING 51
#define CHECK_POINT_LOAD 52
#define END_LOADING 53
#define INSERT 54
#define GRANT_LOGON 55
#define REVOKE_LOGON 56
#define BEGIN_ACCESS_LOG 57
#define END_ACCESS_LOG 58
#define COLLECT_STATISTICS 59
#define DROP_STATISTICS 60
#define SESSION_SET 61
#define BEGIN_MULTILOAD 62
#define MULTILOAD 63
#define EXECUTE_MULTILOAD 64
#define END_MULTILOAD 65
#define RELEASE_MULTILOAD 66
#define MULTILOAD_DELETE 67
#define MULTILOAD_INSERT 68
#define MULTILOAD_UPDATE 69
#define BEGIN_DELETE_MULTILOAD 70
#define DATA_STATUS 71
#define RESERVED_FOR_B1_SECURITY_1 72
#define RESERVED_FOR_B1_SECURITY_2 73
#define BEGIN_EXPORT 74
#define END_EXPORT 75
#define _2PC_VOTE_REQUEST 76
#define _2PC_VOTE_AND_TERMINATE_REQUEST 77
#define _2PC_COMMIT 78
#define _2PC_ABORT 79
#define _2PC_YES_DONE_RESPONSE_TO_VOTE_REQUEST 80
#define OBSOLETE_1 81
#define OBSOLETE_2 82
#define SET_SESSION_RATE 83
#define MONITOR_SESSION_1 84
#define IDENTIFY 85
#define ABORT_SESSION 86
#define SET_RESOURCE_RATE 87
#define OBSOLETE_3 88
#define REVALIDATE_RI_REFERENCES 89
#define ANSI_SQL_COMMIT_WORK 90
#define MONITOR_VIRTUAL_CONFIG 91
#define MONITOR_PHYSICAL_CONFIG 92
#define MONITOR_VIRTUAL_SUMMARY 93
#define MONITOR_PHYSICAL_SUMMARY 94
#define MONITOR_VIRTUAL_RESOURCE 95
#define SQL_CREATE_TRIGGER 97
#define SQL_DROP_TRIGGER 98
#define SQL_RENAME_TRIGGER 99
#define REPLACE_TRIGGER 100
#define SQL_ALTER_TRIGGER 101
#define REPLICATION 102
#define DROP_PROCEDURE 103
#define CREATE_PROCEDURE 104
#define CALL 105
#define SQL_RENAME_PROCEDURE 106
#define REPLACE_PROCEDURE 107
#define SET_SESSION_ACCOUNT 108
#define LOCKING_LOGGER 109
#define MONITOR_SESSION_2 110
#define MONITOR_VERSION 111
#define BEGIN_DATABASE_QUERY_LOG 112
#define END_DATABASE_QUERY_LOG 113
#define SQL_CREATE_ROLE 114
#define SQL_DROP_ROLE 115
#define GRANT_ROLE 116
#define REVOKE_ROLE 117
#define SQL_CREATE_PROFILE 118
#define SQL_MODIFY_PROFILE 119
#define SQL_DROP_PROFILE 120
#define SQL_SET_ROLE 121
#define CREATE_UDF 122
#define REPLACE_UDF 123
#define DROP_UDF 124
#define ALTER_UDF 125
#define RENAME_UDF 126
#define SQL_MERGE_INTO_UPDATES_AND_INSERTS 127
#define SQL_MERGE_INTO_UPDATES_NO_INSERTS 128
#define SQL_MERGE_INTO_ALL_INSERTS_NO_UPDATES 129
#define SQL_ALTER_PROCEDURE 130
#define PM_API_REQUEST_TDWM_ENABLE 131
#define PM_API_REQUEST_TDWM_STATISTICS 132
#define TDWM_PERF_GROUPS 133
#define CREATE_UDT 134
#define DROP_UDT 135
#define ALTER_UDT 136
#define REPLACE_UDT 137
#define SQL_CREATE_METHOD 138
#define ALTER_METHOD 139
#define REPLACE_METHOD 140
#define CREATE_CAST 141
#define REPLACE_CAST 142
#define DROP_CAST 143
#define SQL_CREATE_ORDERING 144
#define REPLACE_ORDERING 145
#define DROP_ORDERING 146
#define SQL_CREATE_TRANSFORM 147
#define REPLACE_TRANSFORM 148
#define SQL_DROP_TRANSFORM 149
#define CREATE_AUTHORIZATION 150
#define DROP_AUTHORIZATION 151
#define CREATE_REPLICATION_GROUP 152
#define ALTER_REPLICATION_GROUP 153
#define DROP_REPLICATION_GROUP 154
#define TDWM_DELETE_REQUEST_CHANGE_STMT 155
#define TDWM_SUMMARY_STMT 156
#define TDWM_DYN_RULE_STMT 157
#define TDWM_DYN_OBJ_STMT 158
#define TDWM_WD_ASSIGNMENT_STMT 159
#define TDWM_DYN_BUILD 160
#define TDWM_LIST_WD_STMT 161
#define SET_SESSION_ISOLATION_LEVEL 162
#define INITIATE_INDEX_ANALYSIS 163
#define REPLACE_AUTH_STMT 164
#define SET_QUERY_BAND_STMT 165
#define LOGGING_ONLINE_ARCHIVE_ON 166
#define LOGGING_ONLINE_ARCHIVE_OFF 167
#define MONITOR_QUERYBAND 168
#define CREATE_COLUMN_CORRELATION 169
#define REPLACE_COLUMN_CORRELATION 170
#define DROP_COLUMN_CORRELATION 171
#define ALTER_COLUMN_CORRELATION 172
#define USER_EVENT_CONTROL 173
#define EVENT_STATUS 174
#define MONITOR_AWT_RESOURCE 175
#define SP_DYNAMIC_RESULT_SET 176
#define CREATE_REPLICATION_RULE_SET 177
#define REPLACE_REPLICATION_RULE_SET 178
#define DROP_REPLICATION_RULE_SET 179
#define CREATE_OPERATOR 180
#define REPLACE_OPERATOR 181
#define RENAME_OPERATOR 182
#define DROP_OPERATOR 183
#define GRANT_CONNECT_THROUGH 184
#define REVOKE_CONNECT_THROUGH 185
#define CREATE_GLOP_SET 186
#define DROP_GLOP_SET 187
#define CREATE_SECURITY_CONSTRAINT 188
#define ALTER_SECURITY_RESTRAINT 189
#define DROP_SECURITY_RESTRAINT 190
#define CREATE_INDEX_TYPE 191
#define DROP_INDEX_TYPE 192
#define REPLACE_INDEX_TYPE 193
#define ALTER_INDEX 194
#define CHECK_WORKLOAD_FOR 195
#define FASTEXPORT_NO_SPOOLING 196
#define CHECK_WORKLOAD_END 197
#define FLUSH_DBQL_CACHE 198
#define TDWMEXCEPTION_PM_API 199
#define TDWMTEST_PM_API 200
#define MONITOR_TDWM_RESORCE_PM_API 201
#define MONITOR_WD_PM_API 202
#define REGISTER_XML_SCHEMA_XSLT_STYLESHEET_XQUERY_MODULE 203
#define CALENDAR_CHANGED 204
#define MONITOR_REQUEST_PM_API 205
#define MERGE_INTO_DELETE 206
#define BEGIN_QUERY_CAPTURE 207
#define END_QUERY_CAPTURE 208
#define SHOW_IN_XML_DB_OBJECT_DML 209
#define GTW_HOSTGROUP_PROPERTIES 210
#define PROXYCONFIG___FROM_CLIENT_WHEN_IT_SUPPORTS_PROXY 211
#define SECUREATTRIBUTE_PARCEL 212
#define VH_FSG_CACHE 213
#define UNITY_SQL_STMT 214
#define ASTER_SUPPORT 215
#define CREATE_ZONE 216
#define ALTER_ZONE 217
#define DROP_ZONE 218
#define CREATE_FORE_SRV 219
#define ALTER_FORE_SRV 220
#define DROP_FOREIGN_SRV 221
#define BEGIN_ISO_LOAD 222
#define CHECKPT_ISO_LOAD 223
#define END_ISO_LOAD 224
#define SET_SESS_LOAD 226
#define GRANT_ZONE 225
#define REVOKE_ZONE 227
#define FOR_VIEWPOINT 228
#define SET_SESS_JSON 229
#define CHECKJI_ACT_TYPE 230
#define CREATE_MAP 231
#define SET_SESSION_UPT 232
#define DROP_MAP 233
#define GRANT_MAP 234
#define REVOKE_MAP 235
#define SESS_DOT_NO  236
#define CREATE_SCHEMA 237
#define DROP_SCHEMA 238
#define DEBUG_FUNCTION 239
#define FOREIGN_SERVER 240


class bindtype {
	public:
		const char	*type;
		uint16_t	typelen;
};

class request {
	public:
		request(uint16_t maxbindcount);
		~request();

		sqlrservercursor	*cur;

		char	requestmode;
		char	function;
		char	selectdata;
		char	continuedcharactersstate;
		char	aphresponse;
		char	returnstatementinfo;
		char	udttransformsoff;
		char	maxdecprec;
		char	identitycolumnretrieval;
		char	dynamicresultsets;
		char	spreturnresult;
		char	periodstructon;
		char	columninfo;
		char	trustedsessions;
		char	multistatementerrors;
		char	arraytransformsoff;
		char	xmlresponseformat;
		char	tasmfastfailreq;

		bool		runstartup;

		uint32_t	querylen;
		const char	*query;

		bool		setposition;

		bool		bindvars;
		bindtype	*bindtypes;
		bool		bindvals;

		uint16_t	activity;
		size_t		activitycountpos;
		uint16_t	activitycountsize;
		uint64_t	activitycount;
		uint32_t	fieldcount;
		uint64_t	currentfield;

		dynamicarray<byte_t>	nibuffer;
		bytebuffer		rowbuffer;

		size_t		parcelsizepos;

		bool		resendrow;

		bool		fudgecommitwork;
		bool		fudgeselect;
};

request::request(uint16_t maxbindcount) {

	cur=NULL;

	requestmode=0;
	function=0;
	selectdata=0;
	continuedcharactersstate=0;
	aphresponse=0;
	returnstatementinfo=0;
	udttransformsoff=0;
	maxdecprec=0;
	identitycolumnretrieval=0;
	dynamicresultsets=0;
	spreturnresult=0;
	periodstructon=0;
	columninfo=0;
	trustedsessions=0;
	multistatementerrors=0;
	arraytransformsoff=0;
	xmlresponseformat=0;
	tasmfastfailreq=0;

	runstartup=false;

	querylen=0;
	query=NULL;

	setposition=false;
	bindvars=false;
	bindtypes=new bindtype[maxbindcount];
	bindvals=false;

	activity=0;
	activitycountpos=0;
	activitycountsize=0;
	activitycount=0;
	fieldcount=0;
	currentfield=0;
	parcelsizepos=0;
	resendrow=0;

	fudgecommitwork=false;
	fudgeselect=false;
}

request::~request() {
	delete[] bindtypes;
}

class SQLRSERVER_DLLSPEC sqlrprotocol_teradata : public sqlrprotocol {
	public:
			sqlrprotocol_teradata(sqlrservercontroller *cont,
							sqlrprotocols *ps,
							domnode *parameters);
		virtual	~sqlrprotocol_teradata();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

	private:
		void	init();
		void	free();
		void	reInit();

		bool	initialHandshake();

		bool	copKindCfg();
		bool	copKindAssign();
		bool	copKindSsoReq();
		bool	copKindConnect();

		bool	copKindReAssign();
		bool	copKindReConnect();
		bool	copKindContinue();
		bool	copKindAbort();
		bool	copKindLogoff();
		bool	copKindTest();
		bool	copKindAuthMethods();
		bool	copKindElicitData();
		bool	copKindDefaultConnect();
		bool	copKindStart();
		bool	copKindDirect();

		bool	recvRequestFromClient();
		bool	sendResponseToClient();

		bool	passthrough();
		bool	forwardClientRequestToBackend();
		bool	recvResponseFromBackend();
		bool	forwardBackendResponseToClient();

		void	parseParcelHeader(const byte_t *parcel,
					uint16_t *flavor,
					uint32_t *datalength,
					const byte_t **parcelout);
		bool	parseClientConfigParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseConfigParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseAssignParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseSsoRequestParcel(const byte_t *parcel,
					const byte_t **parcelout);
		void	confAlg(byte_t val);
		void	intAlg(byte_t val);
		void	kexAlg(byte_t val);
		void	confAlgMode(byte_t val);
		void	confAlgPadding(byte_t val);
		void	confAlgKeySize(byte_t conf, uint16_t val);
		void	kexAlgKeySize(byte_t kex, uint16_t val);
		bool	parseSsoParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseLogoffParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseOptionsParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseGenericReqParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseGenericRunStartupParcel(
					const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseGenericRespParcel(
					const byte_t *parcel,
					const byte_t **parcelout);
		bool	isBulkLoadData(const byte_t *parcel);
		bool	parseSetPositionParcel(const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseDataParcel(const byte_t *parcel,
					const byte_t **parcelout);
		void	parseTinyIntBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseSmallIntBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseIntegerBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseBigIntBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseCharBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseVarCharBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseByteBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseVarByteBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseFloatBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseDateBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseTimeBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		void	parseTimestampBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr);
		bool	parseStatementInfoParcel(
					const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseStatementInfoExtensions(
						const byte_t *ext,
						uint32_t extlen);
		bool	parseParameterExtension(const byte_t *ext,
							uint32_t extlen,
							uint16_t ibcount);
		bool	parseStatementInfoEndParcel(
					const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseMultipartIndicDataParcel(
					const byte_t *parcel,
					const byte_t **parcelout);
		bool	parseEndMultipartIndicDataParcel(
					const byte_t *parcel,
					const byte_t **parcelout);
		bool	parse215Parcel(const byte_t *parcel,
					const byte_t **parcelout);
		void	parseUsing();
		void	translateInsertToSelect();
		bool	prepareQuery();
		bool	executeQuery();
		bool	parseCancelParcel(const byte_t *parcel,
					const byte_t **parcelout);
		void	parseGenericParcels(const byte_t *parcel,
					const byte_t *end);
		bool	parseGenericParcel(const byte_t *parcel,
					const byte_t **parcelout);
		void	appendParcelHeader(uint16_t flavor,
						uint32_t datalength);
		void	appendLargeParcelHeader(uint16_t flavor,
						uint32_t datalength);
		void	appendSmallParcelHeader(uint16_t flavor,
						uint32_t datalength);
		void	appendParcelHeader(uint16_t flavor);
		void	endParcel();
		void	appendConfigResponseParcel();
		void	appendConfigResponseParcelHeader();
		void	appendConfigResponseField78();
		void	appendConfigResponseField84();
		void	appendConfigResponseField49();
		void	appendConfigResponseField9();
		void	appendConfigResponseField10();
		void	appendConfigResponseField11();
		void	appendConfigResponseField12();
		void	appendConfigResponseField13();
		void	appendConfigResponseVersions();
		void	appendConfigResponseField14();
		void	appendConfigResponseField15();
		void	appendConfigResponseField16();
		void	appendConfigResponseField6();
		void	appendGatewayConfigParcel();
		void	appendTd2MechanismParcel();
		void	appendTdNegoMechanismParcel();
		void	appendLdapMechanismParcel();
		void	appendKrbMechanismParcel();
		void	appendKrbCompatMechanismParcel();
		void	appendLogonFailureParcel(const char *errorstring);
		void	setSessionNumber();
		void	appendAssignResponseParcel();
		void	appendSsoResponseParcel();
		void	appendSsoParcel();
		void	appendSuccessParcel();
		void	updateActivityCount();
		void	appendStatementStatusParcel(uint32_t statementnumber);
		void	appendStatementStatusParcel();
		void	appendColumnParcels();
		void	appendFieldColumnParcels();
		void	getFieldFormat(bytebuffer *fieldformat, uint16_t col);
		void	appendFieldParcel(const char *data, uint16_t size);
		void	appendDataInfoParcel();
		void	appendStatementInfoParcel();
		void	appendEstimatedProcessingTimeExtension(uint64_t time);
		void	appendEndEstimatedProcessingTimeExtension();
		void	appendQueryExtension(uint16_t col);
		void	appendEndQueryExtension();
		void	appendStatementInfoEndParcel();
		void	appendRowParcels(bool *eors);
		void	backpatchActivityCount();
		void	appendTitleStartParcel();
		void	appendTitleEndParcel();
		void	appendFormatStartParcel();
		void	appendFormatEndParcel();
		void	appendSizeStartParcel();
		void	appendSizeEndParcel();
		void	appendSizeParcel(uint16_t size);
		void	appendRecStartParcel();
		void	appendRecEndParcel();
		void	appendField(uint16_t col, 
					const char *field,
					uint64_t fieldlength,
					bool null);
		void	appendRecordModeField(uint16_t col, 
						const char *field,
						uint64_t fieldlength,
						bool null);
		void	appendIndicatorModeField(uint16_t col, 
						const char *field,
						uint64_t fieldlength,
						bool null);
		void	appendRecordParcel();
		void	appendFailureParcel(const char *errorstring,
						uint16_t errorlength);
		void	appendErrorParcel(const char *errorstring);
		void	appendEndStatementParcel();
		void	appendEndStatementParcel(uint16_t statementnumber);
		void	appendEndRequestParcel();
		void	appendCursorErrorParcel();
		void	appendConnectionErrorParcel();

		void	unexpectedParcel(uint16_t parcelflavor);

		uint16_t	getActivity();
		bool		activityReturnsResults();
		const char	*getColumnTypeName(uint16_t col);
		uint16_t	getColumnTypeNameLength(uint16_t col);
		uint16_t	getColumnType(uint16_t col);

		void	debugParcelStart(const char *direction,
						const char *flavorname,
						uint16_t parcelflavor,
						uint32_t parceldatalength);
		void	debugParcelEnd(const byte_t *parceldata,
						uint32_t parceldatalength);
		void	debugParcelStart(const char *direction,
						const char *flavorname,
						uint16_t parcelflavor);
		void	debugParcelEnd();
		void	debugExtStart(const char *extname);
		void	debugExtEnd();

#ifdef DECRYPT
		bool	decrypt(const byte_t *encdata,
					uint64_t encdatasize,
					bytebuffer *decdata);
		bool	encrypt(const byte_t *decdata,
					uint64_t decdatasize,
					bytebuffer *encdata);
		bool	generateServerPublicKey();
		bool	generateSharedSecret();
#endif

		passthroughmode_t	passthroughmode;

		// request buffers
		filedescriptor	*clientsock;
		memorypool	*clientreqmessagepool;
		byte_t		*clientreqheader;
		byte_t		*clientreqdata;
		uint32_t	clientreqdatalength;

		// passthrough buffers
		memorypool	*backendreqmessagepool;
		byte_t		*backendreqheader;
		byte_t		*backendreqdata;
		uint32_t	backendreqdatalength;

		// response buffers
		bytebuffer	respheader;
		bytebuffer	respdata;

		// message
		byte_t		messagekind;
		uint32_t	sessionno;
		byte_t		requestauth[8];
		uint32_t	requestno;
		byte_t		gtwbyte;
		byte_t		hostcharset;
		datetime	sadt;
		byte_t		responseauth[8];

		// requests
		request		*req;

		// max message size
		uint32_t	maxmessagesize;

		// auth mechs
		bool		td2mechenabled;
		bool		tdnegomechenabled;
		bool		ldapmechenabled;
		bool		krbmechenabled;
		bool		krbcompatmechenabled;

		// encryption
		bool		blowfishsupported;
		bool		aessupported;
		bool		md5supported;
		bool		sha1supported;
		bool		sha256supported;
		bool		sha512supported;
		bool		dhsupported;
		bool		cbcsupported;
		bool		cfbsupported;
		bool		ecbsupported;
		bool		ofbsupported;
		bool		gcmsupported;
		bool		ccmsupported;
		bool		ctrsupported;
		bool		oaepsupported;
		bool		pkcs1supported;
		bool		pkcs5supported;
		bool		ssl3supported;
		bool		aes128supported;
		bool		aes192supported;
		bool		aes256supported;
		bool		dh2048supported;
		uint16_t	negotiatedmech;
		uint16_t	negotiatedqop;
		byte_t		dhp[256];
		byte_t		dhg[256];
		byte_t		serverpubkey[256];
		byte_t		clientpubkey[256];
#ifdef DECRYPT
		DH		*dh;
#endif
		unsigned  char	*sharedsecret;
		uint32_t	sharedsecretsize;
		unsigned  char	sha2sharedsecret[32];
};

sqlrprotocol_teradata::sqlrprotocol_teradata(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	// configure passthrough mode
	if (getDebug()) {
		stdoutput.write("passthrough mode - ");
	}
	if (charstring::compare(cont->identify(),"teradata")) {
		passthroughmode=PASSTHROUGHMODE_DISABLED;
		if (getDebug()) {
			stdoutput.write("disabled...\n");
		}
	} else {
		if (!charstring::compare(
			parameters->getAttributeValue("passthrough"),"yes")) {
			passthroughmode=PASSTHROUGHMODE_ENABLED;
			if (getDebug()) {
				stdoutput.write("enabled...\n");
			}
		} else if (!charstring::compare(
			parameters->getAttributeValue("passthrough"),"no")) {
			passthroughmode=PASSTHROUGHMODE_DISABLED;
			if (getDebug()) {
				stdoutput.write("disabled...\n");
			}
		} else {
			passthroughmode=PASSTHROUGHMODE_HYBRID;
			if (getDebug()) {
				stdoutput.write("hybrid...\n");
			}
		}
	}

	// request buffers
	clientsock=NULL;
	clientreqmessagepool=new memorypool(1024,1024,10240);
	clientreqheader=NULL;
	clientreqdata=NULL;
	clientreqdatalength=0;

	// backend buffers
	backendreqmessagepool=new memorypool(1024,1024,10240);
	backendreqheader=NULL;
	backendreqdata=NULL;
	backendreqdatalength=0;

	// message
	messagekind=0;
	sessionno=0;
	bytestring::zero(requestauth,sizeof(requestauth));
	requestno=0;
	bytestring::zero(responseauth,sizeof(responseauth));

	// request
	req=NULL;

	// max message size
	maxmessagesize=0;

	// auth mechs
	// FIXME: make these configurable (and support non-td2)
	td2mechenabled=true;
	tdnegomechenabled=false;
	ldapmechenabled=false;
	krbmechenabled=false;
	krbcompatmechenabled=false;

	// encryption
	blowfishsupported=false;
	aessupported=false;
	md5supported=false;
	sha1supported=false;
	sha256supported=false;
	sha512supported=false;
	dhsupported=false;
	cbcsupported=false;
	cfbsupported=false;
	ecbsupported=false;
	ofbsupported=false;
	gcmsupported=false;
	ccmsupported=false;
	ctrsupported=false;
	oaepsupported=false;
	pkcs1supported=false;
	pkcs5supported=false;
	ssl3supported=false;
	aes128supported=false;
	aes192supported=false;
	aes256supported=false;
	dh2048supported=false;
	negotiatedmech=MECH_NONE;
	negotiatedqop=QOP_NONE;
	byte_t	dhpdefault[]={
		// DHKeyP2048 from Teradata 2 section in:
		// /opt/teradata/tdgss/etc/TdgssLibraryConfigFile.xml
		// or
		// /opt/teradata/tdgss/etc/TdgssUserConfigFile.xml
		// (DH prime "p")
		// FIXME: make this configurable
		0x8A, 0xB3, 0xF8, 0x6E, 0x8D, 0x37, 0x4B, 0x78,
		0x2F, 0x31, 0xDA, 0xD5, 0xF2, 0x7D, 0x6A, 0xFD,
		0xA3, 0x01, 0x50, 0xC1, 0x1A, 0x20, 0xCF, 0x63,
		0x46, 0x71, 0x2A, 0xE2, 0xD2, 0xC6, 0xB7, 0x0A,
		0x5B, 0x79, 0xD4, 0x5D, 0x4C, 0x0C, 0x23, 0x2A,
		0x06, 0x5B, 0x20, 0x7B, 0x12, 0x1B, 0x2C, 0x33,
		0xE1, 0x47, 0xB5, 0x98, 0x3C, 0x38, 0xA1, 0x08,
		0x7F, 0x27, 0x27, 0x03, 0xB0, 0xB8, 0x39, 0xCB,
		0xA6, 0xF7, 0x1C, 0x5D, 0x0E, 0xB5, 0x1E, 0xC8,
		0x90, 0x93, 0x4E, 0xAC, 0xF2, 0xC7, 0xDD, 0x2A,
		0x1D, 0xF6, 0xF5, 0x5E, 0x89, 0xB1, 0x45, 0xA0,
		0x35, 0x9D, 0x35, 0xEF, 0x8F, 0xB6, 0xC5, 0x61,
		0xE1, 0x57, 0xB1, 0x3F, 0xF9, 0x27, 0xA3, 0x5E,
		0x69, 0x96, 0x36, 0x48, 0x61, 0x49, 0x02, 0xB1,
		0x03, 0x4E, 0xF7, 0x11, 0x97, 0xF5, 0x45, 0xDE,
		0xF3, 0x23, 0x62, 0x44, 0xEA, 0xDA, 0xE0, 0x68,
		0x9E, 0x62, 0x4C, 0xF1, 0x24, 0x59, 0x53, 0x63,
		0x0A, 0xE0, 0x42, 0xBD, 0x79, 0x7C, 0x40, 0x25,
		0xE3, 0x7C, 0x51, 0xD9, 0xF6, 0xCB, 0xDA, 0x0B,
		0x22, 0x78, 0xFA, 0x7D, 0x5C, 0xA2, 0xD9, 0xCA,
		0x93, 0x0B, 0xE2, 0x96, 0x83, 0x30, 0xC8, 0x11,
		0xA4, 0xBA, 0x4D, 0x08, 0x45, 0x33, 0x3C, 0x0D,
		0x62, 0xE3, 0xEE, 0x74, 0x21, 0x54, 0xF6, 0xB6,
		0x2F, 0x29, 0x51, 0xCD, 0x8C, 0x73, 0xC4, 0x3B,
		0x5A, 0xA1, 0xC7, 0x81, 0x9D, 0xEF, 0x1D, 0x7C,
		0x93, 0x14, 0x41, 0x1E, 0x46, 0x5F, 0x8E, 0x47,
		0x96, 0x66, 0x65, 0x94, 0xAA, 0xDE, 0x0A, 0xEB,
		0x3F, 0x12, 0x56, 0xE5, 0x71, 0x9E, 0x7A, 0xE5,
		0x4D, 0xD3, 0x4F, 0xFD, 0xA9, 0x49, 0x63, 0x4E,
		0x4A, 0x29, 0x3C, 0x5B, 0xC6, 0x0A, 0xF2, 0x58,
		0xBB, 0x9F, 0xE5, 0x58, 0x08, 0x6E, 0x83, 0xB3,
		0xDD, 0x3D, 0x74, 0x91, 0x96, 0x6D, 0xEE, 0x93
	};
	bytestring::copy(dhp,dhpdefault,sizeof(dhpdefault));

	byte_t	dhgdefault[]={
		// DHKeyG2048 from Teradata 2 section in:
		// /opt/teradata/tdgss/etc/TdgssLibraryConfigFile.xml
		// or
		// /opt/teradata/tdgss/etc/TdgssUserConfigFile.xml
		// (DH prime root modulo of "p" - "g")
		// FIXME: make this configurable
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05
	};
	bytestring::copy(dhg,dhgdefault,sizeof(dhgdefault));

#ifdef DECRYPT
	dh=NULL;
#endif
	sharedsecret=NULL;
	sharedsecretsize=0;
	bytestring::zero(sha2sharedsecret,sizeof(sha2sharedsecret));

	init();
}

sqlrprotocol_teradata::~sqlrprotocol_teradata() {
	delete[] sharedsecret;
#ifdef DECRYPT
	DH_free(dh);
#endif
	free();
	delete clientreqmessagepool;
	delete backendreqmessagepool;
}

void sqlrprotocol_teradata::init() {
}

void sqlrprotocol_teradata::free() {
	clientreqmessagepool->clear();
	backendreqmessagepool->clear();
}

void sqlrprotocol_teradata::reInit() {
	free();
	init();
}

clientsessionexitstatus_t sqlrprotocol_teradata::clientSession(
						filedescriptor *cs) {

	if (getDebug()) {
		stdoutput.write("starting client session\n");
	}

	clientsock=cs;
	clientsock->setNaglesAlgorithmEnabled(false);
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	reInit();

	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;

	if (initialHandshake()) {

		bool	loop=true;
		do {

			if (!recvRequestFromClient()) {
				status=
				CLIENTSESSIONEXITSTATUS_CLOSED_CONNECTION;
				break;
			}

			switch (messagekind) {
				case COPKIND_REASSIGN:
					loop=copKindReAssign();
					break;
				case COPKIND_RECONNECT:
					loop=copKindReConnect();
					break;
				case COPKIND_CONTINUE:
					loop=copKindContinue();
					break;
				case COPKIND_ABORT:
					loop=copKindAbort();
					break;
				case COPKIND_LOGOFF:
					copKindLogoff();
					loop=false;
					status=
					CLIENTSESSIONEXITSTATUS_ENDED_SESSION;
					break;
				case COPKIND_TEST:
					loop=copKindTest();
					break;
				case COPKIND_AUTHMETHODS:
					loop=copKindAuthMethods();
					break;
				case COPKIND_ELICITDATA:
					loop=copKindElicitData();
					break;
				case COPKIND_DEFAULTCONNECT:
					loop=copKindDefaultConnect();
					break;
				case COPKIND_START:
					loop=copKindStart();
					break;
				case COPKIND_DIRECT:
					loop=copKindDirect();
					break;
				default:
					if (getDebug()) {
						stdoutput.printf("INVALID "
								"MESSAGE\n");
					}
					break;
			}

		} while (loop);
	}

	cont->closeClientConnection(0);
	cont->endSession();

	return status;
}

bool sqlrprotocol_teradata::initialHandshake() {

	if (passthroughmode==PASSTHROUGHMODE_DISABLED) {

		sqlrteradatacredentials	cred;
		cred.setClientFileDescriptor(clientsock);
		return cont->auth(&cred);

	} else {

		for (;;) {
			if (!recvRequestFromClient()) {
				return false;
			}

			switch (messagekind) {
				case COPKIND_CFG:
					if (!copKindCfg()) {
						return false;
					}
					break;
				case COPKIND_ASSIGN:
					if (!copKindAssign()) {
						return false;
					}
					break;
				case COPKIND_SSOREQ:
					if (!copKindSsoReq()) {
						return false;
					}
					break;
				case COPKIND_CONNECT:
					return copKindConnect();
				default:
					return false;
			}
		}
	}
}

bool sqlrprotocol_teradata::copKindCfg() {

	// parse request
	debugStart("copkind_cfg");

	// parse parcels
	const byte_t	*parcel=clientreqdata;
	if (!parseClientConfigParcel(parcel,&parcel)) {
		debugEnd();
		return false;
	}
	parseConfigParcel(parcel,&parcel);

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// respond
	respdata.clear();

	appendConfigResponseParcel();
	appendGatewayConfigParcel();
	appendTd2MechanismParcel();
	appendTdNegoMechanismParcel();
	appendLdapMechanismParcel();
	appendKrbMechanismParcel();
	appendKrbCompatMechanismParcel();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindAssign() {

	// parse request
	debugStart("copkind_assign");

	// parse parcels
	const byte_t	*parcel=clientreqdata;
	parseAssignParcel(parcel,&parcel);
	if (!parseSsoRequestParcel(parcel,&parcel)) {
		debugEnd();
		return false;
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// build response
	respdata.clear();

	if (negotiatedmech==MECH_NONE || negotiatedqop==QOP_NONE) {
		appendLogonFailureParcel(
			"UserId, Password or Account is invalid.");
	} else {
		setSessionNumber();
		appendAssignResponseParcel();
		appendSsoResponseParcel();
	}

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindSsoReq() {

	// parse request
	debugStart("copkind_ssoreq");

	// parse parcels
	const byte_t	*parcel=clientreqdata;
	if (!parseSsoParcel(parcel,&parcel)) {
		debugEnd();
		return false;
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// build response
	respdata.clear();

	appendSsoParcel();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindConnect() {

	// parse request
	debugStart("copkind_connect");

#ifdef DECRYPT
	// FIXME: parse request, it should contain:
	// * logon parcel - 36
	// * session option parcel - 114
	// * connect parcel - 88
	// * data parcel - 3
	// * client attribute parcel - 189
	// * sso username request parcel - 136
	//
	// appears to be encrypted with the server's private key
	// always appears to be 410 bytes for bteq
	// always appears to be 664 bytes for jdbc
	if (getDebug()) {
		stdoutput.printf("	request {\n");
		debugHexDump(clientreqdata,clientreqdatalength);
		stdoutput.printf("	}\n");
	}
	bytebuffer	decdata;
	decrypt(clientreqdata,clientreqdatalength,&decdata);
	if (getDebug()) {
		stdoutput.printf("	decrypted request {\n");
		debugHexDump(decdata.getBuffer(),decdata.getSize());
		stdoutput.printf("	}\n");
	}
#endif

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response, it should contain:
	// * success parcel - 8
	// * sso username response parcel - 137
	// * end request parcel - 12
	//
	// appears to be encrypted with the client's private key
	// always appears to be 205 bytes
	respdata.clear();

	// ideally we'd encrypt this with the same secret that we used to
	// generate serverpubkey, but for now, we'll just use matching canned
	// responses for both (still doesn't work though)
	byte_t	response[]={
		0x18, 0xe1, 0xaf, 0xc0, 0xa6, 0xe8, 0xad, 0x83,
		0xf7, 0x17, 0xa2, 0xf7, 0x18, 0x18, 0x21, 0xfe,
		0xcb, 0xcd, 0xbc, 0x77, 0x17, 0x25, 0xfe, 0x15,
		0xea, 0x89, 0x78, 0xcf, 0x06, 0xb1, 0x45, 0x0a,
		0xba, 0xd3, 0x64, 0xef, 0x94, 0xfc, 0xd4, 0x83,
		0x3d, 0x1f, 0x7b, 0x8c, 0x8e, 0xa5, 0xaf, 0x06,
		0xda, 0x4d, 0xdc, 0x60, 0x03, 0xe5, 0xb1, 0x43,
		0xde, 0xf0, 0x67, 0x16, 0x3e, 0x23, 0x43, 0x67,
		0x50, 0xbd, 0x87, 0x9b, 0x01, 0x7f, 0x39, 0xcb,
		0xcd, 0xa3, 0xc6, 0x86, 0xa9, 0x7d, 0xb0, 0x60,
		0xcd, 0xb7, 0xfa, 0x9f, 0x81, 0xf8, 0x00, 0xd2,
		0x58, 0x49, 0xc1, 0x72, 0xdd, 0x83, 0xa6, 0x8f,
		0x0e, 0x2a, 0x92, 0x43, 0x57, 0x43, 0x2e, 0x20,
		0xed, 0xd8, 0x4a, 0x10, 0x83, 0xb2, 0x0a, 0x3c,
		0x03, 0x21, 0xbd, 0xa9, 0x11, 0x4c, 0x0d, 0x8c,
		0x9a, 0x72, 0x09, 0xb1, 0x1c, 0x2a, 0x1e, 0x11,
		0xb4, 0x74, 0xc4, 0xc4, 0xe1, 0x13, 0xc0, 0x01,
		0x03, 0xc1, 0x01, 0x07, 0xc2, 0x01, 0x84, 0xc3,
		0x01, 0x00, 0xc4, 0x02, 0x00, 0xaa, 0xc5, 0x01,
		0x01, 0xc2, 0x20, 0x74, 0x72, 0x84, 0x8e, 0x31,
		0x7d, 0x16, 0xf6, 0x10, 0xb1, 0x0e, 0x2f, 0xe1,
		0xdb, 0xad, 0xcb, 0xcb, 0x59, 0x45, 0x00, 0xb8,
		0x74, 0x68, 0x0e, 0x6a, 0x10, 0xf5, 0x42, 0xd3,
		0xaa, 0x79, 0x0f, 0xc3, 0x10, 0x18, 0xf0, 0x18,
		0x3f, 0x6a, 0x5d, 0x5e, 0x2c, 0x5c, 0xb4, 0x38,
		0xc8, 0xd6, 0x3c, 0x91, 0x20
	};
	write(&respdata,response,sizeof(response));
	if (getDebug()) {
		stdoutput.printf("	response {\n");
		debugHexDump(response,sizeof(response));
		stdoutput.printf("	}\n");
	}

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindReAssign() {

	// parse request
	debugStart("copkind_reassign");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.write("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindReConnect() {

	// parse request
	debugStart("copkind_reconnect");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.write("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindContinue() {

	// parse request
	debugStart("copkind_continue");

	// parse parcels
	bool		cancel=false;
	const byte_t	*parcel=clientreqdata;
	if (!parseGenericRespParcel(parcel,&parcel)) {
		if (parseCancelParcel(parcel,&parcel)) {
			cancel=true;
		}
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// respond
	respdata.clear();
	bool	eors=true;
	if (cancel) {
		appendEndRequestParcel();
	} else {
		appendRowParcels(&eors);
		if (eors) {
			appendEndStatementParcel();
			appendEndRequestParcel();
		}
	}

	// release request, if appropriate
	if (eors) {
		if (req && req->cur) {
			cont->closeResultSet(req->cur);
			if (getDebug()) {
				stdoutput.printf("	releasing request\n");
			}
			cont->setState(req->cur,SQLRCURSORSTATE_AVAILABLE);
		}
		delete req;
		req=NULL;
	}

	debugEnd();

	// return appropriate result
	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindAbort() {

	// parse request
	debugStart("copkind_abort");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.write("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindLogoff() {

	// parse request
	debugStart("copkind_logoff");

	// parse parcels
	const byte_t	*parcel=clientreqdata;
	if (!parseLogoffParcel(parcel,&parcel)) {
		debugEnd();
		return false;
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// decrement the request number (for some reason)
	if (requestno==2) {
		requestno=0;
	} else {
		requestno--;
	}

	// respond
	respdata.clear();
	appendSuccessParcel();
	debugEnd();
	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindTest() {

	// parse request
	debugStart("copkind_test");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.printf("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindAuthMethods() {

	// parse request
	debugStart("copkind_authmethods");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.printf("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindElicitData() {

	// parse request
	debugStart("copkind_elicitdata");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.printf("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindDefaultConnect() {

	// parse request
	debugStart("copkind_defaultconnect");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.printf("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::copKindStart() {

	// parse request
	debugStart("copkind_start");

	// initialize end-of-result-set flag
	bool	eors=true;

	// parse parcels
	bool		retval=true;
	const byte_t	*parcel=clientreqdata;
	if (parseOptionsParcel(parcel,&parcel)) {

		if (!parseGenericReqParcel(parcel,&parcel) &&
			!parseGenericRunStartupParcel(parcel,&parcel)) {
			retval=false;
			goto end;
		}

		parseSetPositionParcel(parcel,&parcel);


		// check for data...

		// If the query constained a USING clause then it will
		// have defined the bind variables and a data parcel
		// will provide the values.
		//
		// Actually, there are even cases where a data parcel is sent
		// even if the query didn't include a USING clause.
		// (eg. the DELETE FROM SYSADMIN.FASTLOG following a fastload)
		// It's not clear what the data is in these cases, but we need
		// to handle the parcel either way.
		parseDataParcel(parcel,&parcel);

		// if the query doesn't contain USING clause then these
		// statement info parcels will define the bind variables
		parseStatementInfoParcel(parcel,&parcel);
		parseStatementInfoEndParcel(parcel,&parcel);
		// ...and these multipart-indic-data parcels
		// will provide the values
		while (parseMultipartIndicDataParcel(parcel,&parcel) &&
			parseEndMultipartIndicDataParcel(parcel,&parcel)) {
			// FIXME: Currently, successive calls to
			// parseMultipartIndicDataParcel just overwrite
			// the bind values with whatever was provided
			// by the most recent call.
			//
			// Instead, multiple calls are supposed to
			// create an array of bind values.
			// (eg. for a bulk insert).
			//
			// A separate status parcel and end-result
			// should be returned for each set of values
			// too.
			//
			// SQL Relay doesn't currently support
			// array-binds though, so there's no good way
			// to implement this.
		}

		parse215Parcel(parcel,&parcel);

		if (!parseGenericRespParcel(parcel,&parcel)) {
			retval=false;
			goto end;
		}

	} else if (isBulkLoadData(parcel)) {

		if (passthroughmode!=PASSTHROUGHMODE_ENABLED) {

			// handle actual bulk load...

			respdata.clear();

			const byte_t	*parcel=clientreqdata;


			// generate the id
			// FIXME: It's not at all clear how a teradata backend
			// associates fastload sessions.  For now, we'll assume
			// one-fastload-per-client and use the client hostname,
			// but I'm sure this is wrong...
			// query band maybe?
			// (priority)
			char	*hostname=sys::getHostName();

			// join bulk load
			bool	success=cont->bulkLoadJoin(hostname);

			// clean up
			delete[] hostname;

			if (!success) {
				appendConnectionErrorParcel();
				goto end;
			}

			// get a req/cursor so the stuff below will work...
			// (for now, assume that getCursor() succeeds)
			req=new request(cont->getConfig()->getMaxBindCount());
			req->cur=cont->getCursor();

			// bind the data
			for (;;) {
				uint16_t	parcelflavor;
				const byte_t	*parceldata;
				uint32_t	parceldatalength;
				parseParcelHeader(parcel,&parcelflavor,
							&parceldatalength,
							&parceldata);

				if (parcelflavor==3) {
					if (!cont->bulkLoadInputBind(
							parceldata,
							parceldatalength)) {
						appendConnectionErrorParcel();
						goto end;
					}
				} else {
					break;
				}
				parcel=parceldata+parceldatalength;

				debugParcelStart("recv","data",
						parcelflavor,parceldatalength);
				debugParcelEnd();

				req->activitycount++;
			}
			parseGenericRespParcel(parcel,&parcel);

			if (!cont->bulkLoadExecuteQuery()) {
				appendConnectionErrorParcel();
				goto end;
			}

			appendSuccessParcel();
			appendEndRequestParcel();

			goto end;
		}

	} else {
		retval=false;
		goto end;
	}

	// skip everything if passthrough is enabled
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		goto end;
	}

	respdata.clear();

	// get the activity
	req->activity=getActivity();

	if (req->runstartup) {

		// intercept run-startup requests
		appendErrorParcel("No startup string defined for this user.");

	} else if (

		// intercept transaction queries
		req->activity==SQL_BEGIN_TRANSACTION ||
		req->activity==SQL_END_TRANSACTION ||
		req->activity==SQL_ROLLBACK) {

		bool	result=false;
		if (req->activity==SQL_BEGIN_TRANSACTION) {
			result=cont->begin();
		} else if (req->activity==SQL_END_TRANSACTION) {
			result=cont->commit();
		} else if (req->activity==SQL_ROLLBACK) {
			result=cont->rollback();
		}
		if (result) {
			appendStatementStatusParcel();
			appendEndStatementParcel();
			appendEndRequestParcel();
		} else {
			appendConnectionErrorParcel();
		}

	} else if (

		// intercept fastload-begin queries
		req->activity==BEGIN_LOADING) {

		// generate the id
		// FIXME: It's not at all clear how a teradata backend
		// associates fastload sessions.  For now, we'll assume
		// one-fastload-per-client and use the client hostname,
		// but I'm sure this is wrong...
		// query band maybe?
		// (priority)
		char	*hostname=sys::getHostName();

		// copy the query...
		char	*buffer=charstring::duplicate(req->query,req->querylen);

		// get the table name
		char	*table=buffer+14;
		char	*endtable=charstring::findFirst(table," ERRORFILES ");
		if (endtable) {
			*endtable='\0';
		} else {
			// FIXME: error
		}

		// get the first error table
		char	*error1=endtable+12;
		while (*error1 && character::isWhitespace(*error1)) {
			error1++;
		}
		char	*enderror1=error1;
		while (*enderror1 &&
				!character::isWhitespace(*enderror1) &&
				*enderror1!=',') {
			enderror1++;
		}
		if (*enderror1) {
			*enderror1='\0';
		} else {
			// FIXME: error
		}

		// get the second error table
		char	*error2=enderror1+1;
		while (*error2 &&
			(character::isWhitespace(*error2) || *error1==',')) {
			error2++;
		}
		char	*enderror2=error2;
		while (*enderror2 &&
				!character::isWhitespace(*enderror2) &&
				*enderror2!=',') {
			enderror2++;
		}
		if (*enderror2) {
			*enderror2='\0';
		}

		// FIXME: optional NODROP
		uint64_t nodrop=false;

		// FIXME: optional CHECKPOINT <row count>
		// FIXME: optional INDICATORS
		// FIXME: optional DATAENCRYPTION ON|OFF

		// get the max error count
		// FIXME: from where? (priority)
		uint64_t maxerrorcount=25;

		// begin bulk load
		bool	success=cont->bulkLoadBegin(hostname,
							error1,
							error2,
							maxerrorcount,
							!nodrop);

		// clean up
		delete[] hostname;
		delete[] buffer;

		if (success) {

			// statement 1 response
			appendStatementStatusParcel();
			req->nibuffer.clear();
			req->rowbuffer.clear();
			const byte_t unknown1[]={
				0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x30, 0x09, 0x01, 0x00, 0x01, 0x00,
				0x00, 0x00
			};
			write(&req->rowbuffer,unknown1,sizeof(unknown1));
			appendRecordParcel();
			appendEndStatementParcel(1);

			// statement 2 response (for some reason)
			appendStatementStatusParcel();
			req->nibuffer.clear();
			req->rowbuffer.clear();
			const byte_t unknown2[]={
				0x00, 0x00
			};
			write(&req->rowbuffer,unknown2,sizeof(unknown2));
			appendRecordParcel();
			appendEndStatementParcel(2);

			appendEndRequestParcel();
		} else {
			appendConnectionErrorParcel();
		}

	} else if (

		// intercept fastload-checkpoint queries
		req->activity==CHECK_POINT_LOAD) {

		char	*id=charstring::duplicate(req->query+19,
							req->querylen-19);
		bool	success=cont->bulkLoadCheckpoint(id);
		delete[] id;

		if (success) {
			appendStatementStatusParcel();
			appendEndStatementParcel();
			appendEndRequestParcel();
		} else {
			appendConnectionErrorParcel();
		}

	} else if (

		// intercept fastload-end queries
		req->activity==END_LOADING) {

		if (cont->bulkLoadEnd()) {
			appendStatementStatusParcel();
			appendEndStatementParcel();
			appendEndRequestParcel();
		} else {
			appendConnectionErrorParcel();
		}

	} else if (

		// intercept the bulk load query...
		// this will have a using clause (bindvars will be true)
		// but no associated data parcel (bindvals will be false)
		req->bindvars && !req->bindvals) {

		if (cont->bulkLoadPrepareQuery(req->query,
					req->querylen,
					cont->getInputBindCount(req->cur),
					cont->getInputBinds(req->cur))) {
			appendStatementStatusParcel();
			appendEndStatementParcel();
			appendEndRequestParcel();
		} else {
			appendConnectionErrorParcel();
		}

	} else {

		// handle all other queries normally...
		if (req->function=='P' || req->function=='S') {

			if (prepareQuery()) {
				appendStatementStatusParcel();
				if (activityReturnsResults()) {
					appendColumnParcels();
				}
				appendEndStatementParcel();
				appendEndRequestParcel();
			} else {
				appendCursorErrorParcel();
			}

		} else if (req->function=='E' || req->function=='B') {

			if (prepareQuery() && executeQuery()) {

				// FIXME: if we had multiple result sets
				// or did an array-bind, then we need to
				// return sets of these, like:
				// * status, (columns, rows), end-stmt
				// * status, (columns, rows), end-stmt
				// * status, (columns, rows), end-stmt
				// ...
				// * end-req

				appendStatementStatusParcel();
				if (activityReturnsResults()) {
					appendColumnParcels();
					appendRowParcels(&eors);
					backpatchActivityCount();
				}
				if (eors) {
					appendEndStatementParcel();
// FIXME: ODBC doesn't like receiving COMMIT WORK queries but
// Teradata Studio sends one during the initial handshake...
if (req->fudgecommitwork) {
	req->activity=ANSI_SQL_COMMIT_WORK;
	appendStatementStatusParcel(2);
	appendEndStatementParcel(2);
}
					appendEndRequestParcel();
				}
			} else {
				appendCursorErrorParcel();
			}
		}
	}

end:

	// release request, if appropriate
	if (eors) {
		if (req && req->cur) {
			cont->closeResultSet(req->cur);
			if (getDebug()) {
				stdoutput.printf("	releasing request\n");
			}
			cont->setState(req->cur,SQLRCURSORSTATE_AVAILABLE);
		}
		delete req;
		req=NULL;
	}

	debugEnd();

	// return appropriate result
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		return passthrough();
	}
	if (retval) {
		return sendResponseToClient();
	}
	return retval;
}

bool sqlrprotocol_teradata::copKindDirect() {

	// parse request
	debugStart("copkind_direct");

	// FIXME: parse parcels
	if (getDebug()) {
		stdoutput.printf("	...\n");
	}

	// if passthrough is enabled then just do that
	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugEnd();
		return passthrough();
	}

	// FIXME: build response
	respdata.clear();

	debugEnd();

	return sendResponseToClient();
}

bool sqlrprotocol_teradata::recvRequestFromClient() {

	clientreqmessagepool->clear();

	// receive lan header
	clientreqheader=clientreqmessagepool->allocate(LAN_HEADER_SIZE);
	if (clientsock->read(clientreqheader,LAN_HEADER_SIZE)!=
							LAN_HEADER_SIZE) {
		if (getDebug()) {
			stdoutput.write("read header from client failed\n");
		}
		return false;
	}

	// lan header fields
	byte_t		version;
	byte_t		messageclass;
	uint16_t	highordermessagelength;
	byte_t		bytevar;
	uint16_t	wordvar;
	uint16_t	lowordermessagelength;
	uint16_t 	resforexpan[3];
	uint16_t	corrtag[2];
	byte_t		spare[14];

	// copy out values from lan header
	const byte_t	*ptr=clientreqheader;
	read(ptr,&version,&ptr);
	read(ptr,&messageclass,&ptr);
	read(ptr,&messagekind,&ptr);
	readBE(ptr,&highordermessagelength,&ptr);
	read(ptr,&bytevar,&ptr);
	readBE(ptr,&wordvar,&ptr);
	readBE(ptr,&lowordermessagelength,&ptr);
	// FIXME: net-to-host these?
	read(ptr,(byte_t *)resforexpan,sizeof(resforexpan),&ptr);
	// FIXME: net-to-host these?
	read(ptr,(byte_t *)corrtag,sizeof(corrtag),&ptr);
	readBE(ptr,&sessionno,&ptr);
	read(ptr,(byte_t *)requestauth,sizeof(requestauth),&ptr);
	readBE(ptr,&requestno,&ptr);
	read(ptr,&gtwbyte,&ptr);
	read(ptr,&hostcharset,&ptr);
	read(ptr,(byte_t *)spare,sizeof(spare),&ptr);

	clientreqdatalength=(((uint32_t)highordermessagelength)<<16)|
					((uint32_t)lowordermessagelength);

	if (getDebug()) {
		debugStart("client recv header");
		stdoutput.printf("	version: %d\n",(int)version);
		stdoutput.printf("	class: %d\n",(int)messageclass);
		stdoutput.printf("	kind: %d\n",(int)messagekind);
		stdoutput.printf("	high order message length: %d\n",
						(int)highordermessagelength);
		stdoutput.printf("	bytevar: %d\n",(int)bytevar);
		stdoutput.printf("	wordvar: %d\n",(int)wordvar);
		stdoutput.printf("	low order message length: %d\n",
						(int)lowordermessagelength);
		stdoutput.write("	res for expan: ");
		stdoutput.safePrint((byte_t *)resforexpan,sizeof(resforexpan));
		stdoutput.write('\n');
		stdoutput.write("	correlation tag: ");
		stdoutput.safePrint((byte_t *)corrtag,sizeof(corrtag));
		stdoutput.write('\n');
		stdoutput.printf("	session no: %d\n",(int)sessionno);
		stdoutput.printf("	request auth: "
					"%03d.%03d.%03d.%03d."
					"%03d.%03d.%03d.%03d\n",
					requestauth[0],
					requestauth[1],
					requestauth[2],
					requestauth[3],
					requestauth[4],
					requestauth[5],
					requestauth[6],
					requestauth[7]);
		stdoutput.printf("	request auth: "
					"%02x.%02x.%02x.%02x."
					"%02x.%02x.%02x.%02x\n",
					requestauth[0],
					requestauth[1],
					requestauth[2],
					requestauth[3],
					requestauth[4],
					requestauth[5],
					requestauth[6],
					requestauth[7]);
		stdoutput.printf("	request no: %d\n",(int)requestno);
		stdoutput.printf("	gateway byte: %d\n",(int)gtwbyte);
		stdoutput.printf("	host charset: %d\n",(int)hostcharset);
		stdoutput.printf("	clientreqdatalength: %d\n",
						(int)clientreqdatalength);
		stdoutput.write('\n');
		debugHexDump(clientreqheader,LAN_HEADER_SIZE);
		debugEnd();
	}


	// receive lan data
	clientreqdata=clientreqmessagepool->allocate(clientreqdatalength);
	if (clientsock->read(clientreqdata,clientreqdatalength)!=
						(ssize_t)clientreqdatalength) {
		if (getDebug()) {
			stdoutput.write("read data from client failed\n");
		}
		return false;
	}

	if (getDebug()) {
		debugStart("client recv data");
		debugHexDump(clientreqdata,clientreqdatalength);
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_teradata::sendResponseToClient() {

	// lan header fields
	byte_t		version=3;
	byte_t		messageclass=2;
	uint32_t	messagelength=respdata.getSize();
	uint16_t	highordermessagelength=(messagelength>>16);
	// FIXME: There are cfg/assign cases where bytevar should be 8.
	byte_t		bytevar=0;
	uint16_t	wordvar=0;
	uint16_t	lowordermessagelength=((messagelength<<16)>>16);
	uint16_t 	resforexpan[3]={0,0,0};
	uint16_t	corrtag[2]={0,0};

	// calculate response authentication...
	// (if bytes 3-6 are non-zero, otherwise just zero it out)
	uint32_t	time=*((uint32_t *)&requestauth[3]);
	if (time) {

		// FIXME: the below is incorrect for kind == CONNECT

		// [0,1,2]   - always 0
		// [3,4,5,6] - seconds since 1970, little endian
		// [7]       - appears to be a sequence number

		// initialize a datetime from [3,4,5,6]
		sadt.init(leToHost(time));

		// responseauth is:
		// [0] - always 0
		// [1] - always 0
		// [2] - changes every request and appears to be random
		// [3] - changes every request and appaers to be random
		// [4] - minute_of_hour%4*64+second_of_minute
		// 		starts at 0,
		// 		increments each second,
		// 		ranges: 0-59, 64-123, 128-187, 192-251
		//
		// if (kind == START) {
		// 	[5] - minute_of_hour/4+176
		// 		starts at 176
		// 		increments when [4] rolls over (every 4 minutes)
		// 		range: 176-...
		// } else {
		// 	[5] - minute_of_hour/4+128
		// 		starts at 128
		// 		increments when [4] rolls over (every 4 minutes)
		// 		range: 128-142
		// }
		//
		// [6] - hour_of_day%8*32+day_of_month
		// 		starts on day of month
		// 		increments by 32 when [5] rolls over (each hour)
		// 		rolls over every 8 hours
		//
		// if (kind == START) {
		//	[7] - 211 + sequence number
		// 		rolls over automatically
		// } else {
		// 	[7] - 255-day_of_month*2+1
		//		(actually, this could be
		//		255-day_of_month*2-1+[7]
		//		because it's incremented by 1
		//		in the ssoreq message)
		// 		decrements by 2 at midnight
		// 		...
		// 		8/21 (233) - 214
		// 		8/22 (234) - 212
		// 		...
		// }
		responseauth[0]=0;
		responseauth[1]=0;
		responseauth[2]=0x72;
		responseauth[3]=0x77;
		responseauth[4]=sadt.getMinute()%4*64+sadt.getSecond();
		if (messagekind==COPKIND_START) {
			responseauth[5]=sadt.getMinute()/4+176;
		} else {
			responseauth[5]=sadt.getMinute()/4+128;
		}
		responseauth[6]=
			sadt.getHour()%8*32+sadt.getDayOfMonth();
		if (messagekind==COPKIND_START) {
			responseauth[7]=211+requestauth[7];
		} else {
			responseauth[7]=255-sadt.getDayOfMonth()*2+1;
		}

	} else {
		bytestring::zero(responseauth,
					sizeof(responseauth));
	}

	gtwbyte=(messagekind==COPKIND_CFG || messagekind==COPKIND_ASSIGN)?5:0;
	hostcharset=0x7F;
	byte_t	spare[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	// build lan header
	respheader.clear();
	write(&respheader,version);
	write(&respheader,messageclass);
	write(&respheader,messagekind);
	writeBE(&respheader,highordermessagelength);
	write(&respheader,bytevar);
	writeBE(&respheader,wordvar);
	writeBE(&respheader,lowordermessagelength);
	// FIXME: host-to-net this?
	write(&respheader,(byte_t *)resforexpan,sizeof(resforexpan));
	// FIXME: host-to-net this?
	write(&respheader,(byte_t *)corrtag,sizeof(corrtag));
	writeBE(&respheader,sessionno);
	write(&respheader,responseauth,sizeof(responseauth));
	// FIXME: requestno, gtwbyte, and hostcharset
	// are set differently for kind == CONNECT
	writeBE(&respheader,requestno);
	write(&respheader,gtwbyte);
	write(&respheader,hostcharset);
	write(&respheader,spare,sizeof(spare));

	if (getDebug()) {
		debugStart("client send header");
		stdoutput.printf("	version: %d\n",(int)version);
		stdoutput.printf("	class: %d\n",(int)messageclass);
		stdoutput.printf("	kind: %d\n",(int)messagekind);
		stdoutput.printf("	high order message length: %d\n",
						(int)highordermessagelength);
		stdoutput.printf("	bytevar: %d\n",(int)bytevar);
		stdoutput.printf("	wordvar: %d\n",(int)wordvar);
		stdoutput.printf("	low order message length: %d\n",
						(int)lowordermessagelength);
		stdoutput.write("	res for expan: ");
		stdoutput.safePrint((byte_t *)resforexpan,sizeof(resforexpan));
		stdoutput.write('\n');
		stdoutput.write("	correlation tag: ");
		stdoutput.safePrint((byte_t *)corrtag,sizeof(corrtag));
		stdoutput.write('\n');
		stdoutput.printf("	session no: %d\n",(int)sessionno);
		stdoutput.printf("	response auth: "
					"%03d.%03d.%03d.%03d."
					"%03d.%03d.%03d.%03d\n",
					responseauth[0],
					responseauth[1],
					responseauth[2],
					responseauth[3],
					responseauth[4],
					responseauth[5],
					responseauth[6],
					responseauth[7]);
		stdoutput.printf("	response auth: "
					"%02x.%02x.%02x.%02x."
					"%02x.%02x.%02x.%02x\n",
					responseauth[0],
					responseauth[1],
					responseauth[2],
					responseauth[3],
					responseauth[4],
					responseauth[5],
					responseauth[6],
					responseauth[7]);
		stdoutput.printf("	request no: %d\n",(int)requestno);
		stdoutput.printf("	gateway byte: %d\n",(int)gtwbyte);
		stdoutput.printf("	host charset: %d\n",(int)hostcharset);
		stdoutput.printf("	messagelength: %d\n",
						(int)messagelength);
		stdoutput.write('\n');
		debugHexDump(respheader.getBuffer(),respheader.getSize());
		debugEnd();
	}

	// send lan header
	if (clientsock->write(respheader.getBuffer(),
				respheader.getSize())!=
				(ssize_t)respheader.getSize()) {
		if (getDebug()) {
			stdoutput.write("write header to client failed\n");
		}
		return false;
	}

	if (getDebug()) {
		debugStart("client send data");
		stdoutput.printf("	size: %d\n",respdata.getSize());
		debugHexDump(respdata.getBuffer(),respdata.getSize());
		debugEnd();
	}

	if (clientsock->write(respdata.getBuffer(),
				respdata.getSize())!=
				(ssize_t)respdata.getSize()) {
		if (getDebug()) {
			stdoutput.write("write data to client failed\n");
		}
		return false;
	}

	clientsock->flushWriteBuffer(-1,-1);
	return true;
}

bool sqlrprotocol_teradata::passthrough() {
	return forwardClientRequestToBackend() &&
		recvResponseFromBackend() &&
		forwardBackendResponseToClient();
}

bool sqlrprotocol_teradata::forwardClientRequestToBackend() {

	// pass whatever we received from the client through to the backend
	/*if (getDebug()) {
		debugStart("backend send header");
		stdoutput.printf("	length: %d\n",LAN_HEADER_SIZE);
		debugHexDump(clientreqheader,LAN_HEADER_SIZE);
		stdoutput.write("}\n");
		stdoutput.write("backend send data {\n");
		stdoutput.printf("	length: %d\n",clientreqdatalength);
		debugHexDump(clientreqdata,clientreqdatalength);
		debugEnd();
	}*/
	if (!cont->send(clientreqheader,LAN_HEADER_SIZE)) {
		if (getDebug()) {
			stdoutput.write("send client header "
					"to backend failed\n");
		}
		return false;
	}
	if (!cont->send(clientreqdata,clientreqdatalength)) {
		if (getDebug()) {
			stdoutput.write("send client data "
					"to backend failed\n");
		}
		return false;
	}
	return true;
}

bool sqlrprotocol_teradata::recvResponseFromBackend() {

	// receive message
	byte_t	*backendreqmessage=NULL;
	size_t	backendreqmessagesize=0;
	if (!cont->recv(&backendreqmessage,&backendreqmessagesize)) {
		if (getDebug()) {
			stdoutput.write("recv message from backend failed\n");
		}
	}

	// parse lan header...
	backendreqheader=backendreqmessage;

	// lan header fields
	byte_t		version;
	byte_t		messageclass;
	uint16_t	highordermessagelength;
	byte_t		bytevar;
	uint16_t	wordvar;
	uint16_t	lowordermessagelength;
	uint16_t 	resforexpan[3];
	uint16_t	corrtag[2];
	uint32_t	sessionno;
	uint32_t	berequestno;
	byte_t		begtwbyte;
	byte_t		behostcharset;
	byte_t		spare[14];

	// copy out values from lan header
	const byte_t	*ptr=backendreqheader;
	read(ptr,&version,&ptr);
	read(ptr,&messageclass,&ptr);
	read(ptr,&messagekind,&ptr);
	readBE(ptr,&highordermessagelength,&ptr);
	read(ptr,&bytevar,&ptr);
	readBE(ptr,&wordvar,&ptr);
	readBE(ptr,&lowordermessagelength,&ptr);
	// FIXME: net-to-host this?
	read(ptr,(byte_t *)resforexpan,sizeof(resforexpan),&ptr);
	// FIXME: net-to-host this?
	read(ptr,(byte_t *)corrtag,sizeof(corrtag),&ptr);
	readBE(ptr,&sessionno,&ptr);
	read(ptr,(byte_t *)responseauth,sizeof(responseauth),&ptr);
	readBE(ptr,&berequestno,&ptr);
	read(ptr,&begtwbyte,&ptr);
	read(ptr,&behostcharset,&ptr);
	read(ptr,(byte_t *)spare,sizeof(spare),&ptr);

	backendreqdatalength=(((uint32_t)highordermessagelength)<<16)|
				((uint32_t)lowordermessagelength);

	if (getDebug()) {
		debugStart("backend recv header");
		stdoutput.printf("	version: %d\n",(int)version);
		stdoutput.printf("	class: %d\n",(int)messageclass);
		stdoutput.printf("	kind: %d\n",(int)messagekind);
		stdoutput.printf("	high order message length: %d\n",
						(int)highordermessagelength);
		stdoutput.printf("	bytevar: %d\n",(int)bytevar);
		stdoutput.printf("	wordvar: %d\n",(int)wordvar);
		stdoutput.printf("	low order message length: %d\n",
						(int)lowordermessagelength);
		stdoutput.write("	res for expan: ");
		stdoutput.safePrint((byte_t *)resforexpan,sizeof(resforexpan));
		stdoutput.write('\n');
		stdoutput.write("	correlation tag: ");
		stdoutput.safePrint((byte_t *)corrtag,sizeof(corrtag));
		stdoutput.write('\n');
		stdoutput.printf("	session no: %d\n",(int)sessionno);
		stdoutput.printf("	response auth: "
					"%03d.%03d.%03d.%03d."
					"%03d.%03d.%03d.%03d\n",
					responseauth[0],
					responseauth[1],
					responseauth[2],
					responseauth[3],
					responseauth[4],
					responseauth[5],
					responseauth[6],
					responseauth[7]);
		stdoutput.printf("	response auth: "
					"%02x.%02x.%02x.%02x."
					"%02x.%02x.%02x.%02x\n",
					responseauth[0],
					responseauth[1],
					responseauth[2],
					responseauth[3],
					responseauth[4],
					responseauth[5],
					responseauth[6],
					responseauth[7]);
		stdoutput.printf("	request no: %d\n",(int)berequestno);
		stdoutput.printf("	gateway byte: %d\n",(int)begtwbyte);
		stdoutput.printf("	host charset: %d\n",(int)behostcharset);
		stdoutput.printf("	backendreqdatalength: %d\n",
						(int)backendreqdatalength);
		stdoutput.write('\n');
		debugHexDump(backendreqheader,LAN_HEADER_SIZE);
		debugEnd();
	}


	// receive lan data
	backendreqdata=backendreqmessage+LAN_HEADER_SIZE;

	if (getDebug()) {
		debugStart("backend recv data");
		debugHexDump(backendreqdata,backendreqdatalength);
		if (messagekind!=COPKIND_CONNECT) {
			parseGenericParcels(backendreqdata,
					backendreqdata+backendreqdatalength);
		}
		debugEnd();
	}

	return true;
}

bool sqlrprotocol_teradata::forwardBackendResponseToClient() {

	// send whatever we received from the backend to the client
	/*if (getDebug()) {
		debugStart("client send header");
		stdoutput.printf("	length: %d\n",LAN_HEADER_SIZE);
		debugHexDump(backendreqheader,LAN_HEADER_SIZE);
		stdoutput.write("}\n");
		stdoutput.write("client send data {\n");
		stdoutput.printf("	length: %d\n",backendreqdatalength);
		debugHexDump(backendreqdata,backendreqdatalength);
		debugEnd();
	}*/
	if (clientsock->write(backendreqheader,
				LAN_HEADER_SIZE)!=LAN_HEADER_SIZE) {
		if (getDebug()) {
			stdoutput.write("clientsock write failed\n");
		}
		return false;
	}
	if (clientsock->write(backendreqdata,backendreqdatalength)!=
						(ssize_t)backendreqdatalength) {
		if (getDebug()) {
			stdoutput.write("clientsock write failed\n");
		}
		return false;
	}
	clientsock->flushWriteBuffer(-1,-1);
	return true;
}

void sqlrprotocol_teradata::parseParcelHeader(const byte_t *parcel,
					uint16_t *flavor,
					uint32_t *datalength,
					const byte_t **parcelout) {
	
	// get the parcel flavor
	const byte_t	*start=parcel;
	read(parcel,flavor,&parcel);
	// if it's invalid, then try the other endianness
	if ((*flavor&0x7fff)>512) {
		parcel=start;
		setProtocolIsBigEndian(!getProtocolIsBigEndian());
		read(parcel,flavor,&parcel);
	}

	// get the total parcel length...
	// * if the leftmost bit of the flavor is 0,
	//   then the length is stored in the 2 bytes following the flavor
	// * if the leftmost bit of the flavor is 0,
	//   then the length is stored in the 4 bytes following 2 unused bytes
	//   after the flavor, and we need to remove the leftmost bit
	// and subtract the length of the flavor and length-bytes themselves to
	// get the length of the parcel data
	*datalength=0;
	if (*flavor&0x8000) {
		*flavor&=0x7fff;
		parcel+=sizeof(uint16_t);
		read(parcel,datalength,&parcel);
		*datalength=*datalength-sizeof(uint16_t)-
					sizeof(uint16_t)-
					sizeof(uint32_t);
	} else {
		uint16_t	temp;
		read(parcel,&temp,&parcel);
		*datalength=temp-sizeof(uint16_t)-sizeof(uint16_t);
	}

	*parcelout=parcel;
}

bool sqlrprotocol_teradata::parseClientConfigParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=166) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","client config",parcelflavor,parceldatalength);

	// parse parcel data
	const byte_t	*ptr=parceldata;
	const byte_t	*end=parceldata+parceldatalength;

	uint32_t	unknown;
	read(ptr,&unknown,&ptr);
	if (getDebug()) {
		stdoutput.printf("		unknown: %d\n\n",unknown);
	}
	while (ptr!=end) {
		uint16_t	field;
		uint16_t	length;
		read(ptr,&field,&ptr);
		read(ptr,&length,&ptr);
		if (getDebug()) {
			stdoutput.printf("		field: %d\n",field);
			stdoutput.printf("		length: %d\n",length);
			stdoutput.printf("		data:\n");
			debugHexDump(ptr,length,2);
			stdoutput.write('\n');
		}
		ptr+=length;
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseConfigParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=42) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","config",parcelflavor,parceldatalength);

	// no parcel data to parse (may not always be the case though)

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseAssignParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=100) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","assign",parcelflavor,parceldatalength);

	// parse parcel data
	const char	*username=(const char *)parceldata;
	uint32_t	usernamelength=parceldatalength;

	// debug
	if (getDebug()) {
		stdoutput.printf("		username: %.*s\n\n",
							usernamelength,
							username);
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseSsoRequestParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=132) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","sso request",parcelflavor,parceldatalength);

	// parse parcel data
	const byte_t	*ptr=parceldata;
	const byte_t	*end=parceldata+parceldatalength;

	uint16_t	marker;
	uint16_t	postmarkerlength;
	const byte_t	*unknown1;
	byte_t		bodylength;
	const byte_t	*unknown2;
	const byte_t	*unknown3;
	//const byte_t	*padding1;
	byte_t		fieldslength;
	//const byte_t	*padding2;
	read(ptr,&marker,&ptr);
	read(ptr,&postmarkerlength,&ptr);
	unknown1=ptr;
	ptr+=7;
	read(ptr,&bodylength,&ptr);
	unknown2=ptr;
	ptr+=10;
	unknown3=ptr;
	ptr+=2;
	//padding1=ptr;
	ptr+=19;
	read(ptr,&fieldslength,&ptr);
	//padding2=ptr;
	ptr+=40;

	// fudge lengths
	bodylength+=8;
	fieldslength--;

	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		post-marker length: %d\n",
							postmarkerlength);
		stdoutput.printf("		unknown1:\n");
		debugHexDump(unknown1,7,2);
		stdoutput.printf("		body length: %d\n",
							bodylength);
		stdoutput.printf("		unknown2:\n");
		debugHexDump(unknown2,10,2);
		stdoutput.printf("		unknown3:\n");
		debugHexDump(unknown3,2,2);
		stdoutput.printf("		fields length: %d\n",
							fieldslength);
	}

	// parse supported algorithms...
	const byte_t	*fieldsend=ptr+fieldslength;
	while (ptr<fieldsend) {

		// get supported algorithms field
		byte_t	algs;
		read(ptr,&algs,&ptr);
		if (algs!=SUPPORTED_ALGORITHMS) {
			break;
		}
		byte_t	algslen;
		read(ptr,&algslen,&ptr);

		if (getDebug()) {
			stdoutput.printf("		"
					"supported algorithms {\n");
		}

		// parse each supported algorithm
		const byte_t	*algsend=ptr+algslen;
		while (ptr<algsend) {

			// get supported algorithm
			byte_t	alg;
			read(ptr,&alg,&ptr);
			if (alg!=SUPPORTED_ALGORITHM) {
				break;
			}
			byte_t	alglen;
			read(ptr,&alglen,&ptr);

			if (getDebug()) {
				stdoutput.printf("			"
						"supported algorithm {\n");
			}

			// get algorithm details
			const byte_t	*algend=ptr+alglen;
			byte_t		currentconf=ALG_NONE;
			byte_t		currentkex=ALG_NONE;
			while (ptr<algend) {

				// get algorithm details field
				byte_t	algfield;
				read(ptr,&algfield,&ptr);
				byte_t	dlen;
				read(ptr,&dlen,&ptr);

				if (getDebug()) {
					stdoutput.printf(
						"		"
						"		"
						"%s: ",
						algfieldname[algfield-0xd0]);
				}

				if (dlen==1) {

					byte_t	val;
					read(ptr,&val,&ptr);

					switch (algfield) {
						case CONF_ALG:
							currentconf=val;
							confAlg(val);
							break;
						case INT_ALG:
							intAlg(val);
							break;
						case KEX_ALG:
							currentkex=val;
							kexAlg(val);
							break;
						case CONF_ALG_MODE:
							confAlgMode(val);
							break;
						case CONF_ALG_PADDING:
							confAlgPadding(val);
							break;
						default:
							// error
							break;
					}

				} else if (dlen==2) {

					uint16_t	val;
					readBE(ptr,&val,&ptr);

					switch (algfield) {
						case CONF_ALG_KEY_SIZE:
							confAlgKeySize(
								currentconf,
								val);
							break;
						case KEX_ALG_KEY_SIZE:
							kexAlgKeySize(
								currentkex,
								val);
							break;
						default:
							// error
							break;
					}
				}
			}
			// FIXME: sanity check on location...

			if (getDebug()) {
				stdoutput.write("		"
						"	}\n");
			}
		}
		// FIXME: sanity check on location...

		if (getDebug()) {
			stdoutput.write("		}\n");
		}
	}
	// FIXME: sanity check on location...

	// parse requested mech...
	byte_t		reqmechfield;
	byte_t		reqmechlength;
	const byte_t	*reqmech;
	read(ptr,&reqmechfield,&ptr);
	read(ptr,&reqmechlength,&ptr);
	reqmech=ptr;
	ptr+=reqmechlength;
	if (getDebug()) {
		stdoutput.write("		requested mech:\n");
		debugHexDump(reqmech,reqmechlength,2);
	}

	// negotiate mech
	// (for now we only support TD2)
	negotiatedmech=MECH_NONE;
	if (reqmechlength==sizeof(td2mech) &&
			!bytestring::compare(reqmech,
				td2mech,sizeof(td2mech))) {
		negotiatedmech=MECH_TD2;
	} else if (reqmechlength==sizeof(tdnegomech) &&
			!bytestring::compare(reqmech,
				tdnegomech,sizeof(tdnegomech))) {
		negotiatedmech=MECH_TDNEGO;
	} else if (reqmechlength==sizeof(ldapmech) &&
			!bytestring::compare(reqmech,
				ldapmech,sizeof(ldapmech))) {
		negotiatedmech=MECH_LDAP;
	} else if (reqmechlength==sizeof(krbmech) &&
			!bytestring::compare(reqmech,
				krbmech,sizeof(krbmech))) {
		negotiatedmech=MECH_KRB;
	} else if (reqmechlength==sizeof(krbcompatmech) &&
			!bytestring::compare(reqmech,
				krbcompatmech,sizeof(krbcompatmech))) {
		negotiatedmech=MECH_KRBCOMPAT;
	}
	if (getDebug()) {
		stdoutput.printf("		negotiated mech: %s\n",
						mechstr[negotiatedmech]);
	}
	if (negotiatedmech!=MECH_NONE && negotiatedmech!=MECH_TD2) {
		if (getDebug()) {
			stdoutput.write("			"
						"(unsupported)\n");
		}
		negotiatedmech=MECH_NONE;
	}

	// negotiate qop
	// (for now, we require dh2048, aes, and pkcs5 padding)
	negotiatedqop=QOP_NONE;
	if (dhsupported && dh2048supported && aessupported && pkcs5supported) {
		if (aes128supported) {
			if (gcmsupported && sha256supported) {
				negotiatedqop=
				QOP_AES_K128_GCM_PKCS5Padding_SHA2;
			} else if (cbcsupported && sha1supported) {
				negotiatedqop=
				QOP_AES_K128_CBC_PKCS5Padding_SHA1;
			}
		} else if (aes192supported) {
			if (gcmsupported && sha256supported) {
				negotiatedqop=
				QOP_AES_K192_GCM_PKCS5Padding_SHA2;
			} else if (cbcsupported && sha1supported) {
				negotiatedqop=
				QOP_AES_K192_CBC_PKCS5Padding_SHA1;
			}
		} else if (aes256supported) {
			if (gcmsupported && sha256supported) {
				negotiatedqop=
				QOP_AES_K256_GCM_PKCS5Padding_SHA2;
			} else if (cbcsupported && sha1supported) {
				negotiatedqop=
				QOP_AES_K256_CBC_PKCS5Padding_SHA1;
			}
		}
	}
	if (getDebug()) {
		stdoutput.printf("		negotiated qop: %s\n",
						qopstr[negotiatedqop]);
	}

	// trailer...
	const byte_t	*trailer=ptr;
	uint16_t	trailerlength=end-trailer;
	if (getDebug()) {
		stdoutput.write("		trailer:\n");
		debugHexDump(trailer,trailerlength);
		stdoutput.write('\n');
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

void sqlrprotocol_teradata::confAlg(byte_t val) {
	switch (val) {
		case ALG_BLOWFISH:
			blowfishsupported=true;
			break;
		case ALG_AES:
			aessupported=true;
			break;
	}
	if (getDebug()) {
		stdoutput.printf("%s\n",algstr[val]);
	}
}

void sqlrprotocol_teradata::intAlg(byte_t val) {
	switch (val) {
		case ALG_MD5:
			md5supported=true;
			break;
		case ALG_SHA1:
			sha1supported=true;
			break;
		case ALG_SHA256:
			sha256supported=true;
			break;
		case ALG_SHA512:
			sha512supported=true;
			break;
	}
	if (getDebug()) {
		stdoutput.printf("%s\n",algstr[val]);
	}
}

void sqlrprotocol_teradata::kexAlg(byte_t val) {
	switch (val) {
		case ALG_DH:
			dhsupported=true;
			break;
	}
	if (getDebug()) {
		stdoutput.printf("%s\n",algstr[val]);
	}
}

void sqlrprotocol_teradata::confAlgMode(byte_t val) {
	switch (val) {
		case CONF_ALG_MODE_CBC:
			cbcsupported=true;
			break;
		case CONF_ALG_MODE_CFB:
			cfbsupported=true;
			break;
		case CONF_ALG_MODE_ECB:
			ecbsupported=true;
			break;
		case CONF_ALG_MODE_OFB:
			ofbsupported=true;
			break;
		case CONF_ALG_MODE_GCM:
			gcmsupported=true;
			break;
		case CONF_ALG_MODE_CCM:
			ccmsupported=true;
			break;
		case CONF_ALG_MODE_CTR:
			ctrsupported=true;
			break;
	}
	if (getDebug()) {
		stdoutput.printf("%s\n",confalgmodestr[val]);
	}
}

void sqlrprotocol_teradata::confAlgPadding(byte_t val) {
	switch (val) {
		case CONF_ALG_PADDING_OAEP:
			oaepsupported=true;
			break;
		case CONF_ALG_PADDING_PKCS1:
			pkcs1supported=true;
			break;
		case CONF_ALG_PADDING_PKCS5:
			pkcs5supported=true;
			break;
		case CONF_ALG_PADDING_SSL3:
			ssl3supported=true;
			break;
	}
	if (getDebug()) {
		stdoutput.printf("%s\n",confalgpaddingstr[val]);
	}
}

void sqlrprotocol_teradata::confAlgKeySize(byte_t conf, uint16_t val) {
	if (conf==ALG_AES) {
		switch (val) {
			case 128:
				aes128supported=true;
				break;
			case 192:
				aes192supported=true;
				break;
			case 256:
				aes256supported=true;
				break;
		}
	}
	if (getDebug()) {
		stdoutput.printf("%d\n",val);
	}
}

void sqlrprotocol_teradata::kexAlgKeySize(byte_t kex, uint16_t val) {
	if (kex==ALG_DH) {
		switch (val) {
			case 2048:
				dh2048supported=true;
		}
	}
	if (getDebug()) {
		stdoutput.printf("%d\n",val);
	}
}

bool sqlrprotocol_teradata::parseSsoParcel(const byte_t *parcel,
						const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=132) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","sso",parcelflavor,parceldatalength);

	// parse parcel data
	const byte_t	*ptr=parceldata;
	//const byte_t	*end=parceldata+parceldatalength;

	uint16_t	unknown1;
	byte_t		unknown2;
	const byte_t	*token;
	readBE(ptr,&unknown1,&ptr);
	read(ptr,&unknown2,&ptr);
	token=ptr;
	ptr+=17;

	uint16_t	clientpubkeysize=sizeof(clientpubkey);
	if (clientpubkeysize>parceldata+parceldatalength-ptr) {
		clientpubkeysize=parceldata+parceldatalength-ptr;
	}
	read(ptr,clientpubkey,clientpubkeysize,&ptr);

	if (passthroughmode!=PASSTHROUGHMODE_ENABLED) {
#ifdef ENCRYPT
		if (!generateSharedSecret()) {
			// FIXME: fail somehow
		}
#endif
	}

	if (getDebug()) {
		stdoutput.printf("		unknown1: %d\n",unknown1);
		stdoutput.printf("		unknown2: %d\n",unknown2);
		stdoutput.printf("		token:\n");
		debugHexDump(token,17);
		stdoutput.printf("		client public key:\n");
		if (clientpubkeysize<sizeof(clientpubkey)) {
			stdoutput.printf("		"
					"(shorter than expected)\n");
		}
		debugHexDump(clientpubkey,clientpubkeysize);
		if (ptr!=parceldata+parceldatalength) {
			stdoutput.printf("		trailing bytes:\n");
			debugHexDump(ptr,parceldata+parceldatalength-ptr);
		}
		if (passthroughmode!=PASSTHROUGHMODE_ENABLED) {
			stdoutput.printf("		"
						"shared secret:\n");
			debugHexDump(sharedsecret,sharedsecretsize);
			stdoutput.printf("		"
						"sha2 of shared secret:\n");
			debugHexDump(sha2sharedsecret,sizeof(sha2sharedsecret));
		}
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseLogoffParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=37) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","logoff",parcelflavor,parceldatalength);

	// no parcel data to parse

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseOptionsParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=85) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","options",parcelflavor,parceldatalength);

	// get a request
	req=new request(cont->getConfig()->getMaxBindCount());

	// get a cursor
	req->cur=cont->getCursor();
	if (!req->cur) {
		if (getDebug()) {
			stdoutput.printf("		"
					"failed to get a cursor\n");
		}
		debugEnd(2);
		// FIXME: return an error to the client if this happens
		return false;
	}
	// FIXME: kludgy...
	cont->setInputBindCount(req->cur,0);

	// parse parcel data
	const byte_t	*ptr=parceldata;
	read(ptr,&req->requestmode,&ptr);
	read(ptr,&req->function,&ptr);
	read(ptr,&req->selectdata,&ptr);
	read(ptr,&req->continuedcharactersstate,&ptr);
	read(ptr,&req->aphresponse,&ptr);
	read(ptr,&req->returnstatementinfo,&ptr);
	read(ptr,&req->udttransformsoff,&ptr);
	read(ptr,&req->maxdecprec,&ptr);
	read(ptr,&req->identitycolumnretrieval,&ptr);
	read(ptr,&req->dynamicresultsets,&ptr);
	if (parceldatalength>10) {
		read(ptr,&req->spreturnresult,&ptr);
	}
	if (parceldatalength>11) {
		read(ptr,&req->periodstructon,&ptr);
	}
	if (parceldatalength>12) {
		read(ptr,&req->columninfo,&ptr);
	}
	if (parceldatalength>13) {
		read(ptr,&req->trustedsessions,&ptr);
	}
	if (parceldatalength>14) {
		read(ptr,&req->multistatementerrors,&ptr);
	}
	if (parceldatalength>15) {
		read(ptr,&req->arraytransformsoff,&ptr);
	}
	if (parceldatalength>16) {
		read(ptr,&req->xmlresponseformat,&ptr);
	}
	if (parceldatalength>17) {
		read(ptr,&req->tasmfastfailreq,&ptr);
	}

	// debug
	if (getDebug()) {
		stdoutput.printf("		flavor: %d\n",
							parcelflavor);
		stdoutput.printf("		data length: %d\n",
							parceldatalength);
		stdoutput.printf("		cursor id: %d\n",
							(req->cur)?
							req->cur->getId():-1);
		stdoutput.printf("		request mode: %c\n",
						(req->requestmode)?
						req->requestmode:'0');
		stdoutput.printf("		function: %c\n",
						(req->function)?
						req->function:'0');
		stdoutput.printf("		select data: %c\n",
						(req->selectdata)?
						req->selectdata:'0');
		stdoutput.printf("		continued characters state: "
							"%c\n",
					(req->continuedcharactersstate)?
					req->continuedcharactersstate:'0');
		stdoutput.printf("		aph response: %c\n",
						(req->aphresponse)?
						req->aphresponse:'0');
		stdoutput.printf("		return statement info: %c\n",
						(req->returnstatementinfo)?
						req->returnstatementinfo:'0');
		stdoutput.printf("		UDT transforms off: %c\n",
						(req->udttransformsoff)?
						req->udttransformsoff:'0');
		stdoutput.printf("		maximum decimal precision: "
							"%d\n",req->maxdecprec);
		stdoutput.printf("		identity column retrieval: "
							"%c\n",
					(req->identitycolumnretrieval)?
					req->identitycolumnretrieval:'0');
		stdoutput.printf("		dynamic result sets: %c\n",
						(req->dynamicresultsets)?
						req->dynamicresultsets:'0');
		stdoutput.printf("		sp return result: %d\n",
						req->spreturnresult);
		stdoutput.printf("		period struct on: %c\n",
						(req->periodstructon)?
						req->periodstructon:'0');
		stdoutput.printf("		column info: %d\n",
						req->columninfo);
		stdoutput.printf("		trusted sessions: %c\n",
						(req->trustedsessions)?
						req->trustedsessions:'0');
		stdoutput.printf("		multi statement errors: %c\n",
						(req->multistatementerrors)?
						req->multistatementerrors:'0');
		stdoutput.printf("		array transforms off: %c\n",
						(req->arraytransformsoff)?
						req->arraytransformsoff:'0');
		stdoutput.printf("		xml response format: %c\n",
						(req->xmlresponseformat)?
						req->xmlresponseformat:'0');
		stdoutput.printf("		tasm fast fail req: %c\n",
						(req->tasmfastfailreq)?
						req->tasmfastfailreq:'0');
	}

	// override some values
	if (!req->function) {
		req->function='E';
	}
	if (!req->selectdata) {
		req->selectdata='I';
	}
	if (!req->maxdecprec) {
		req->maxdecprec=15;
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseGenericReqParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=1 && parcelflavor!=13 &&
			parcelflavor!=69 && parcelflavor!=148) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv",
			((parcelflavor==1)?"req":
			((parcelflavor==13)?"fmreq":
			((parcelflavor==69)?"indicreq":"multipartrequest"))),
			parcelflavor,parceldatalength);

	// parse parcel data
	req->querylen=parceldatalength;
	req->query=(char *)parceldata;

	// debug
	if (getDebug()) {
		stdoutput.printf("		raw query length: %d\n",
							req->querylen);
		stdoutput.printf("		raw query: (%d) %.*s\n",
							process::getProcessId(),
							req->querylen,
							req->query);
	}

	// parse the "using" of the query (if there is one)
	parseUsing();

	// translate insert to select in some cases
	translateInsertToSelect();

// FIXME: ODBC doesn't like receiving COMMIT WORK queries but
// Teradata Studio sends one during the initial handshake...
if (req->querylen>11 && !charstring::compareIgnoringCase(
					req->query+req->querylen-11,
					"COMMIT WORK")) {
	req->querylen-=11;
	req->fudgecommitwork=true;
}

	// debug
	if (getDebug()) {
		stdoutput.printf("		query length: %d\n",
							req->querylen);
		stdoutput.printf("		query: %.*s\n",
							req->querylen,
							req->query);
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

void sqlrprotocol_teradata::parseUsing() {

	// if the query contained a using clause then this data should
	// be bind values corresponding to the variables defined in that
	// clause...

	// bail if the query didn't contain a using clause
	const char	*bv=cont->skipWhitespaceAndComments(req->query);
	if (charstring::compareIgnoringCase(bv,"using",5)) {
		return;
	}

	// skip "using" and whitespace
	bv+=5;
	bv=cont->skipWhitespaceAndComments(bv);

	const char	*queryend=req->query+req->querylen;

	debugStart("bind variables",2);

	sqlrserverbindvar	*inbinds=cont->getInputBinds(req->cur);
	memorypool		*bindpool=cont->getBindPool(req->cur);

	const char	*ptr;
	uint16_t	ibcount=0;

	for (;;) {

		// reinit
		sqlrserverbindvar	*inbind=&(inbinds[ibcount]);
		bindtype		*inbindtype=&(req->bindtypes[ibcount]);
		inbind->variablesize=0;
		inbind->valuesize=0;
		inbindtype->typelen=0;

		// skip whitespace
		bv=cont->skipWhitespaceAndComments(bv);
		if (bv==queryend) {
			break;
		}

		// get variable name
		ptr=bv;
		while (bv!=queryend && *bv!='(') {
			inbind->variablesize++;
			bv++;
		}
		inbind->variablesize++;
		inbind->variable=
			(char *)bindpool->allocate(inbind->variablesize+1);
		inbind->variable[0]=cont->bindFormat()[0];
		charstring::copy(inbind->variable+1,ptr,inbind->variablesize);
		inbind->variable[inbind->variablesize]='\0';
		if (bv==queryend) {
			break;
		}

		// skip (
		bv++;
		if (bv==queryend) {
			break;
		}

		// get type name
		inbindtype->type=bv;
		while (bv!=queryend && *bv!='(' && *bv!=')') {
			inbindtype->typelen++;
			bv++;
		}
		if (bv==queryend) {
			break;
		}

		// translate type name
		// (parseDataParcel also translates the type name, but 
		// for bulk loads, it will never get called, so we have
		// to make sure that the type is populated here)
		if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"TINYINT",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"SMALLINT",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"INTEGER",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"BIGINT",
						inbindtype->typelen)) {
			inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"CHAR",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"VARCHAR",
						inbindtype->typelen)) {
			inbind->type=SQLRSERVERBINDVARTYPE_STRING;
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"BYTE",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"VARBYTE",
						inbindtype->typelen)) {
			inbind->type=SQLRSERVERBINDVARTYPE_BLOB;
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"DECIMAL",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"NUMBER",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"FLOAT",
						inbindtype->typelen)) {
			inbind->type=SQLRSERVERBINDVARTYPE_DOUBLE;
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"DATE",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"TIME",
						inbindtype->typelen) ||
			!charstring::compareIgnoringCase(
						inbindtype->type,
						"TIMESTAMP",
						inbindtype->typelen)) {
			inbind->type=SQLRSERVERBINDVARTYPE_DATE;
		}

		// override some types...
		if (inbind->variablesize>=6 &&
				!charstring::compareIgnoringCase(
					inbind->variable+1,"DELIM",5)) {
			inbind->type=SQLRSERVERBINDVARTYPE_DELIMITER;
		} else if (inbind->variablesize>=12 &&
				!charstring::compareIgnoringCase(
					inbind->variable+1,"NEWLINECHAR",11)) {
			inbind->type=SQLRSERVERBINDVARTYPE_NEWLINE;
		}

		// get length
		if (*bv=='(') {

			// skip (
			bv++;
			if (bv==queryend) {
				break;
			}

			inbind->valuesize=charstring::convertToInteger(bv);

			while (bv!=queryend && *bv!=')') {
				bv++;
			}
			if (bv==queryend) {
				break;
			}

			// skip )
			bv++;
			if (bv==queryend) {
				break;
			}
		}

		// skip )
		bv++;
		if (bv==queryend) {
			break;
		}

		// debug
		if (getDebug()) {
			stdoutput.write("			");
			if (inbind->valuesize) {
				stdoutput.printf("%d: %s(%.*s(%d))\n",
							ibcount,
							inbind->variable,
							inbindtype->typelen,
							inbindtype->type,
							inbind->valuesize);
			} else {
				stdoutput.printf("%d: %s(%.*s)\n",
							ibcount,
							inbind->variable,
							inbindtype->typelen,
							inbindtype->type);
			}
		}

		// bump bind count
		ibcount++;

		// bail if there are no more variables
		if (*bv!=',') {
			break;
		}

		// skip ,
		bv++;
		if (bv==queryend) {
			break;
		}
	}

	debugEnd(2);

	// we have bind variables
	req->bindvars=true;

	// set input bind count
	cont->setInputBindCount(req->cur,ibcount);

	// move query pointer to after using clause
	// (and update querylen accordingly)
	bv=cont->skipWhitespaceAndComments(bv);
	req->querylen-=(bv-req->query);
	req->query=bv;
}

void sqlrprotocol_teradata::translateInsertToSelect() {
}

bool sqlrprotocol_teradata::parseGenericRunStartupParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=2 && parcelflavor!=14) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv",
			(parcelflavor==2)?"runstartup":"fmrunstartup",
			parcelflavor,parceldatalength);

	// no parcel data to parse
	req->runstartup=true;

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseGenericRespParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=4 && parcelflavor!=153 && parcelflavor!=154) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv",
			(parcelflavor==4)?"resp":
			((parcelflavor==153)?"bigpresp":"bigkeepresp"),
			parcelflavor,parceldatalength);

	// parse parcel data...
	if (parcelflavor==4) {
		maxmessagesize=leToHost(*((uint16_t *)parceldata));
	} else {
		maxmessagesize=leToHost(*((uint32_t *)parceldata));
	}

	// debug
	if (getDebug()) {
		stdoutput.printf("		max message size: %d\n",
							maxmessagesize);
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::isBulkLoadData(const byte_t *parcel) {

	// if we find a data parcel, then (in the context that this
	// method is called) the message must be data for a bulk load
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	return (parcelflavor==3);
}

bool sqlrprotocol_teradata::parseSetPositionParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=157) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","setposition",parcelflavor,parceldatalength);

	// we may want to set the position later
	req->setposition=true;

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd();

	return true;
}

bool sqlrprotocol_teradata::parseDataParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=3) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","data",parcelflavor,parceldatalength);

	if (passthroughmode==PASSTHROUGHMODE_ENABLED) {
		debugHexDump(parceldata,parceldatalength);
		debugParcelEnd();
		*parcelout=parceldata+parceldatalength;
		return true;
	}

	// parse bind values
	const byte_t	*bd=parceldata;

	sqlrserverbindvar	*inbinds=cont->getInputBinds(req->cur);
	uint16_t		inbindcount=cont->getInputBindCount(req->cur);

	debugStart("bind values",2);

	for (uint16_t count=0; count<inbindcount; count++) {

		sqlrserverbindvar	*inbind=&(inbinds[count]);
		bindtype		*inbindtype=&(req->bindtypes[count]);

		// debug
		if (getDebug()) {
			stdoutput.write("			");
			if (inbind->valuesize) {
				stdoutput.printf("%s(%.*s(%d)) = ",
							inbind->variable,
							inbindtype->typelen,
							inbindtype->type,
							inbind->valuesize);
			} else {
				stdoutput.printf("%s(%.*s) = ",
							inbind->variable,
							inbindtype->typelen,
							inbindtype->type);
			}
		}

		// get value from data
		inbind->isnull=cont->nonNullBindValue();
		if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"TINYINT",
						inbindtype->typelen)) {
			parseTinyIntBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"SMALLINT",
						inbindtype->typelen)) {
			parseSmallIntBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"INTEGER",
						inbindtype->typelen)) {
			parseIntegerBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"BIGINT",
						inbindtype->typelen)) {
			parseBigIntBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"CHAR",
						inbindtype->typelen)) {
			parseCharBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"VARCHAR",
						inbindtype->typelen)) {
			parseVarCharBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"BYTE",
						inbindtype->typelen)) {
			parseByteBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"VARBYTE",
						inbindtype->typelen)) {
			parseVarByteBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"DECIMAL",
						inbindtype->typelen)) {
			// FIXME: support this
			// no obvious way to test this, ODBC maybe?
			inbind->type=SQLRSERVERBINDVARTYPE_DOUBLE;
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"NUMBER",
						inbindtype->typelen)) {
			// FIXME: support this
			// no obvious way to test this, ODBC maybe?
			inbind->type=SQLRSERVERBINDVARTYPE_DOUBLE;
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"FLOAT",
						inbindtype->typelen)) {
			// FIXME: no obvious way to test this, ODBC maybe?
			parseFloatBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"DATE",
						inbindtype->typelen)) {
			// FIXME: no obvious way to test this, ODBC maybe?
			parseDateBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"TIME",
						inbindtype->typelen)) {
			// FIXME: no obvious way to test this, ODBC maybe?
			parseTimeBind(bd,inbind,&bd);
		} else if (!charstring::compareIgnoringCase(
						inbindtype->type,
						"TIMESTAMP",
						inbindtype->typelen)) {
			// FIXME: no obvious way to test this, ODBC maybe?
			parseTimestampBind(bd,inbind,&bd);
		}
	}

	debugEnd(2);

	// we have bind values
	req->bindvals=true;

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd();

	return true;
}

void sqlrprotocol_teradata::parseTinyIntBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	char	val;
	read(ptr,&val,outptr);
	inbind->value.integerval=val;
	inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
	if (getDebug()) {
		stdoutput.printf("%d\n",val);
	}
}

void sqlrprotocol_teradata::parseSmallIntBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	uint16_t	val;
	read(ptr,&val,outptr);
	inbind->value.integerval=val;
	inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
	if (getDebug()) {
		stdoutput.printf("%d\n",val);
	}
}

void sqlrprotocol_teradata::parseIntegerBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	int32_t	val;
	read(ptr,(uint32_t *)&val,outptr);
	inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
	inbind->value.integerval=val;
	if (getDebug()) {
		stdoutput.printf("%d\n",val);
	}
}

void sqlrprotocol_teradata::parseBigIntBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	int64_t	val;
	read(ptr,(uint64_t *)&val,outptr);
	inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
	inbind->value.integerval=val;
	if (getDebug()) {
		stdoutput.printf("%lld\n",val);
	}
}

void sqlrprotocol_teradata::parseCharBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	char	*val=(char *)ptr;
	*outptr=ptr+inbind->valuesize;
	inbind->type=SQLRSERVERBINDVARTYPE_STRING;
	inbind->value.stringval=val;
	if (getDebug()) {
		stdoutput.printf("%.*s\n",inbind->valuesize,val);
	}
}

void sqlrprotocol_teradata::parseVarCharBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	uint16_t	len;
	read(ptr,&len,&ptr);
	char	*val=(char *)ptr;
	*outptr=ptr+len;
	inbind->type=SQLRSERVERBINDVARTYPE_STRING;
	inbind->valuesize=len;
	inbind->value.stringval=val;
	if (getDebug()) {
		stdoutput.printf("%.*s\n",len,val);
	}
}

void sqlrprotocol_teradata::parseByteBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	char	*val=(char *)ptr;
	*outptr=ptr+inbind->valuesize;
	inbind->type=SQLRSERVERBINDVARTYPE_BLOB;
	inbind->value.stringval=val;
	if (getDebug()) {
		stdoutput.write('\n');
		debugHexDump((byte_t *)val,inbind->valuesize);
	}
}

void sqlrprotocol_teradata::parseVarByteBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	uint16_t	len;
	read(ptr,&len,&ptr);
	char	*val=(char *)ptr;
	ptr+=len;
	inbind->type=SQLRSERVERBINDVARTYPE_BLOB;
	inbind->valuesize=len;
	inbind->value.stringval=val;
	if (getDebug()) {
		stdoutput.write('\n');
		debugHexDump((byte_t *)val,len);
	}
}

void sqlrprotocol_teradata::parseFloatBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {
	uint64_t	val;
	read(ptr,&val,outptr);
	inbind->value.doubleval.value=*((double *)&val);
	// set precision/scale to max values
	// (NOTE: these were determined experimentally, and work with a
	// teradata backend, it's possible that other backends might not
	// like these values)
	inbind->value.doubleval.precision=38;
	inbind->value.doubleval.scale=37;
	inbind->type=SQLRSERVERBINDVARTYPE_DOUBLE;
	if (getDebug()) {
		stdoutput.printf("%f\n",inbind->value.doubleval.value);
	}
}

void sqlrprotocol_teradata::parseDateBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {

	memorypool		*bindpool=cont->getBindPool(req->cur);

	// copy out the date string
	char	*tmp=charstring::duplicate((char *)ptr,10);
	if (getDebug()) {
		stdoutput.printf("%s\n",tmp);
	}
	*outptr=ptr+10;

	// init the dateval
	inbind->type=SQLRSERVERBINDVARTYPE_DATE;
	inbind->value.dateval.year=-1;
	inbind->value.dateval.month=-1;
	inbind->value.dateval.day=-1;
	inbind->value.dateval.hour=-1;
	inbind->value.dateval.minute=-1;
	inbind->value.dateval.second=-1;
	inbind->value.dateval.microsecond=-1;
	inbind->value.dateval.tz=NULL;
	inbind->value.dateval.isnegative=false;
	inbind->value.dateval.buffersize=64;
	inbind->value.dateval.buffer=
		(char *)bindpool->allocate(inbind->value.dateval.buffersize);

	// parse the date/time/timestamp
	int16_t	year=0;
	int16_t	month=0;
	int16_t	day=0;
	int16_t	hour=0;
	int16_t	minute=0;
	int16_t	second=0;
	int32_t	fraction=0;
	bool	isnegative=false;
	if (datetime::parse(tmp,false,false,"-",
				&year,&month,&day,&hour,&minute,&second,
				&fraction,&isnegative)) {

		// set the dateval components
		inbind->value.dateval.year=year;
		inbind->value.dateval.month=month;
		inbind->value.dateval.day=day;
	}

	// clean up
	delete[] tmp;
}

void sqlrprotocol_teradata::parseTimeBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {

	memorypool		*bindpool=cont->getBindPool(req->cur);

	// copy out the time string
	char	*tmp=charstring::duplicate((char *)ptr,8);
	if (getDebug()) {
		stdoutput.printf("%s\n",tmp);
	}
	*outptr=ptr+8;

	// init the dateval
	inbind->type=SQLRSERVERBINDVARTYPE_DATE;
	inbind->value.dateval.year=-1;
	inbind->value.dateval.month=-1;
	inbind->value.dateval.day=-1;
	inbind->value.dateval.hour=-1;
	inbind->value.dateval.minute=-1;
	inbind->value.dateval.second=-1;
	inbind->value.dateval.microsecond=-1;
	inbind->value.dateval.tz=NULL;
	inbind->value.dateval.isnegative=false;
	inbind->value.dateval.buffersize=64;
	inbind->value.dateval.buffer=
		(char *)bindpool->allocate(inbind->value.dateval.buffersize);

	// parse the date/time/timestamp
	int16_t	year=0;
	int16_t	month=0;
	int16_t	day=0;
	int16_t	hour=0;
	int16_t	minute=0;
	int16_t	second=0;
	int32_t	fraction=0;
	bool	isnegative=false;
	if (datetime::parse(tmp,false,false,"-",
				&year,&month,&day,&hour,&minute,&second,
				&fraction,&isnegative)) {

		// set the dateval components
		inbind->value.dateval.hour=hour;
		inbind->value.dateval.minute=minute;
		inbind->value.dateval.second=second;
		inbind->value.dateval.microsecond=fraction;
	}

	// clean up
	delete[] tmp;
}

void sqlrprotocol_teradata::parseTimestampBind(const byte_t *ptr,
						sqlrserverbindvar *inbind,
						const byte_t **outptr) {

	memorypool		*bindpool=cont->getBindPool(req->cur);

	// copy out the timestamp string
	char	*tmp=charstring::duplicate((char *)ptr,19);
	if (getDebug()) {
		stdoutput.printf("%s\n",tmp);
	}
	*outptr=ptr+19;

	// init the dateval
	inbind->type=SQLRSERVERBINDVARTYPE_DATE;
	inbind->value.dateval.year=-1;
	inbind->value.dateval.month=-1;
	inbind->value.dateval.day=-1;
	inbind->value.dateval.hour=-1;
	inbind->value.dateval.minute=-1;
	inbind->value.dateval.second=-1;
	inbind->value.dateval.microsecond=-1;
	inbind->value.dateval.tz=NULL;
	inbind->value.dateval.isnegative=false;
	inbind->value.dateval.buffersize=64;
	inbind->value.dateval.buffer=
		(char *)bindpool->allocate(inbind->value.dateval.buffersize);

	// parse the date/time/timestamp
	int16_t	year=0;
	int16_t	month=0;
	int16_t	day=0;
	int16_t	hour=0;
	int16_t	minute=0;
	int16_t	second=0;
	int32_t	fraction=0;
	bool	isnegative=false;
	if (datetime::parse(tmp,false,false,"-",
				&year,&month,&day,&hour,&minute,&second,
				&fraction,&isnegative)) {

		// set the dateval components
		inbind->value.dateval.year=year;
		inbind->value.dateval.month=month;
		inbind->value.dateval.day=day;
		inbind->value.dateval.hour=hour;
		inbind->value.dateval.minute=minute;
		inbind->value.dateval.second=second;
		inbind->value.dateval.microsecond=fraction;
	}

	// clean up
	delete[] tmp;
}

bool sqlrprotocol_teradata::parseStatementInfoParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=169) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","169",parcelflavor,parceldatalength);

	parseStatementInfoExtensions(parceldata,parceldatalength);

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseStatementInfoExtensions(
					const byte_t *ext,
					uint32_t extlen) {

	// The spec doesn't even mention that we could receive a
	// StatementInfo request parcel.  Apparently we can, and they appear
	// to be composed of extensions, just like StatementInfo response
	// parcels.  However, so far, I've only seen extension number 8, which
	// isn't defined in the spec, but appears to describe a parameter.

	debugStart("bind variables",2);

	// input bind count
	uint16_t	ibcount=0;

	while (extlen) {

		// PBTILOUT - layout
		// PBTIID - extension
		// PBTILEN - length
		uint16_t	pbtilout;
		uint16_t	pbtiid;
		uint16_t	pbtilen;
		read(ext,&pbtilout,&ext);
		read(ext,&pbtiid,&ext);
		read(ext,&pbtilen,&ext);
		extlen-=(sizeof(uint16_t)*3);

		if (pbtiid==8) {
			parseParameterExtension(ext,pbtilen,ibcount);
			if (pbtilen) {
				ibcount++;
			}
		} else {
			debugStart("unhandled extension",2);
			stdoutput.printf("			"
					"layout: %d\n",pbtilout);
			stdoutput.printf("			"
					"extension: %d\n",pbtiid);
			stdoutput.printf("			"
					"length: %d\n",pbtilen);
			debugHexDump(ext,pbtilen);
			debugEnd(2);
		}

		// move on to the next extension
		ext+=pbtilen;
		extlen-=pbtilen;
	}

	debugEnd(2);

	// set input bind count
	cont->setInputBindCount(req->cur,ibcount);

	return true;
}

bool sqlrprotocol_teradata::parseParameterExtension(
						const byte_t *ext,
						uint32_t extlen,
						uint16_t ibcount) {

	// apparently one of these with a zero-length
	// means that its the end of the parameters
	if (!extlen) {
		return true;
	}

	memorypool		*bindpool=cont->getBindPool(req->cur);
	sqlrserverbindvar	*inbinds=cont->getInputBinds(req->cur);
	sqlrserverbindvar	*inbind=&(inbinds[ibcount]);
	bindtype		*inbindtype=&(req->bindtypes[ibcount]);

	// parse the extension
	uint16_t	type;
	uint16_t	valuesize;
	read(ext,&type,&ext);
	// always zeros - padding?
	ext+=6;
	read(ext,&valuesize,&ext);
	// always zeros - padding?
	ext+=6;

	// we have bind variables
	req->bindvars=true;

	// set bind variable name
	char	*var=charstring::parseNumber(ibcount+1);
	inbind->variablesize=charstring::getLength(var)+1;
	inbind->variable=(char *)bindpool->allocate(inbind->variablesize+1);
	inbind->variable[0]=cont->bindFormat()[0];
	charstring::copy(inbind->variable+1,var,inbind->variablesize);
	inbind->variable[inbind->variablesize]='\0';
	delete[] var;

	// set bind variable type (and size, if applicable)
	const char	*typestr="";
	switch (type) {
		case 756:
		case 757:
			typestr="TINYINT";
			inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
			inbind->valuesize=0;
			break;
		case 500:
		case 501:
			typestr="SMALLINT";
			inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
			inbind->valuesize=0;
			break;
		case 496:
		case 497:
			typestr="INTEGER";
			inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
			inbind->valuesize=0;
			break;
		case 600:
		case 601:
			typestr="BIGINT";
			inbind->type=SQLRSERVERBINDVARTYPE_INTEGER;
			inbind->valuesize=0;
			break;
		case 452:
		case 453:
			typestr="CHAR";
			inbind->type=SQLRSERVERBINDVARTYPE_STRING;
			inbind->valuesize=valuesize;
			break;
		case 448:
		case 449:
			typestr="VARCHAR";
			inbind->type=SQLRSERVERBINDVARTYPE_STRING;
			inbind->valuesize=valuesize;
			break;
		case 692:
		case 693:
			typestr="BYTE";
			inbind->type=SQLRSERVERBINDVARTYPE_BLOB;
			inbind->valuesize=valuesize;
			break;
		case 688:
		case 689:
			typestr="VARBYTE";
			inbind->type=SQLRSERVERBINDVARTYPE_BLOB;
			inbind->valuesize=valuesize;
			break;
		case 484:
		case 485:
			typestr="DECIMAL";
			inbind->type=SQLRSERVERBINDVARTYPE_DOUBLE;
			inbind->valuesize=0;
			// FIXME: set these somehow...
			// no obvious way to test this, ODBC maybe?
			inbind->value.doubleval.precision=0;
			inbind->value.doubleval.scale=0;
			break;
		case 748:
		case 749:
		case 752:
		case 753:
			typestr="DATE";
			inbind->type=SQLRSERVERBINDVARTYPE_DATE;
			inbind->valuesize=0;
			break;
		case 760:
		case 761:
			typestr="TIME";
			inbind->type=SQLRSERVERBINDVARTYPE_DATE;
			inbind->valuesize=0;
			break;
		case 764:
		case 765:
			typestr="DATETIME";
			inbind->type=SQLRSERVERBINDVARTYPE_DATE;
			inbind->valuesize=0;
			break;
		case 480:
		case 481:
			typestr="FLOAT";
			inbind->type=SQLRSERVERBINDVARTYPE_DOUBLE;
			inbind->valuesize=0;
			// set precision/scale to max values
			// (NOTE: these were determined experimentally, and
			// work with a teradata backend, it's possible that
			// other backends might not like these values)
			inbind->value.doubleval.precision=38;
			inbind->value.doubleval.scale=37;
			break;
		default:
			if (getDebug()) {
				stdoutput.printf("unknown bind type: %d\n",
									type);
			}
			typestr="VARBYTE";
			inbind->type=SQLRSERVERBINDVARTYPE_STRING;
			inbind->valuesize=valuesize;
			break;
	}
	inbindtype->type=typestr;
	inbindtype->typelen=charstring::getLength(inbindtype->type);
	inbind->valuesize=0;

	if (getDebug()) {
		stdoutput.write("			");
		if (inbind->valuesize) {
			stdoutput.printf("%d: %s(%.*s(%d))\n",
						ibcount,
						inbind->variable,
						inbindtype->typelen,
						inbindtype->type,
						inbind->valuesize);
		} else {
			stdoutput.printf("%d: %s(%.*s)\n",
						ibcount,
						inbind->variable,
						inbindtype->typelen,
						inbindtype->type);
		}
	}

	return true;
}

bool sqlrprotocol_teradata::parseStatementInfoEndParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=170) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","170",parcelflavor,parceldatalength);

	// no parcel data to parse

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseMultipartIndicDataParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=142) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","142",parcelflavor,parceldatalength);

	// parse parcel data...
	const byte_t	*ptr=parceldata;

	sqlrserverbindvar	*inbinds=cont->getInputBinds(req->cur);
	uint16_t		ibcount=cont->getInputBindCount(req->cur);

	// parse null indicator
	uint32_t	nisize=ibcount/8+1;
	byte_t		ni=*ptr;
	if (getDebug()) {
		stdoutput.printf("		null indicator:\n");
		for (uint16_t i=0; i<nisize; i++) {
			stdoutput.printf("			");
			stdoutput.printBits(*(ptr+i));
			stdoutput.printf(" (%02x)\n",*(ptr+i));
		}
		stdoutput.write("\n");
	}
	for (uint16_t i=0; i<ibcount; i++) {

		// set the null indicator
		sqlrserverbindvar	*inbind=&(inbinds[i]);
		inbind->isnull=(ni&0x80)?
				cont->nullBindValue():
				cont->nonNullBindValue();

		// move on
		ni<<=1;
		if (i%8==7) {
			ptr++;
			ni=*ptr;
		}
	}
	if (ibcount%8) {
		ptr++;
	}

	debugStart("bind values",2);

	// parse bind values...
	for (uint16_t i=0; i<ibcount; i++) {

		sqlrserverbindvar	*inbind=&(inbinds[i]);
		bindtype		*inbindtype=&(req->bindtypes[i]);

		// debug
		if (getDebug()) {
			stdoutput.write("			");
			if (inbind->valuesize) {
				stdoutput.printf("%s(%.*s(%d)) = ",
							inbind->variable,
							inbindtype->typelen,
							inbindtype->type,
							inbind->valuesize);
			} else {
				stdoutput.printf("%s(%.*s) = ",
							inbind->variable,
							inbindtype->typelen,
							inbindtype->type);
			}
		}

		// get value from data
		if (!charstring::compare(inbindtype->type,"CHAR")) {
			parseCharBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"VARCHAR")) {
			parseVarCharBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"TINYINT")) {
			parseTinyIntBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"SMALLINT")) {
			parseSmallIntBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"INTEGER")) {
			parseIntegerBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"BIGINT")) {
			parseBigIntBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"DECIMAL")) {
			// FIXME: no obvious way to test this, ODBC maybe?
			parseFloatBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"FLOAT")) {
			parseFloatBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"BYTE")) {
			parseByteBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"VARBYTE")) {
			parseVarByteBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"DATE")) {
			parseDateBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"TIME")) {
			parseTimeBind(ptr,inbind,&ptr);
		} else if (!charstring::compare(inbindtype->type,"DATETIME")) {
			parseTimestampBind(ptr,inbind,&ptr);
		}

		// override type for nulls...
		// Arguably the controller should do this, or the backend
		// (or backend driver) should handle it correctly.  Some
		// backends (eg. ODBC+teradata) don't necessarily handle the
		// null indicator correctly for all bind types.
		// Eg. SQLBindParameter(type=DATE,isnull=is-a-null) fails.
		// But, SQLBindParameter(type=STRING,isnull=is-a-null) works,
		// which is what gets called if
		// inbind->type=SQLRSERVERBINDVARTYPE_NULL.
		// So we'll go with that for now.
		if (inbind->isnull==cont->nullBindValue()) {
			inbind->type=SQLRSERVERBINDVARTYPE_NULL;
		}
	}

	debugEnd(2);

	// we have bind values
	req->bindvals=true;

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parseEndMultipartIndicDataParcel(
					const byte_t *parcel,
					const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=143) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","143",parcelflavor,parceldatalength);

	// no parcel data to parse

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::parse215Parcel(const byte_t *parcel,
						const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=215) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","215",parcelflavor,parceldatalength);

	// FIXME: parse parcel data
	if (getDebug()) {
		stdoutput.write("	...\n");
	}

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

bool sqlrprotocol_teradata::prepareQuery() {

	uint32_t	querylen=req->querylen;
	const char	*query=req->query;

	// Apparently if we pass the database an options parcel with
	// function='S', followed by a request parcel containing an insert
	// query, then it will return column metadata for the table!
	//
	// JDBC-style fastload depends on this.
	//
	// The only way to do this without passing raw packets to a Teradata
	// backend is to translate the query to a select, and prepare that
	// instead.
	stringbuffer	s;
	if (req->function=='S') {

		// FIXME: this is rough, and assumes that we're inserting into
		// all columns, in order...
		const char	*end=query+querylen;
		const char	*q=cont->skipWhitespaceAndComments(query);
		bool		isinsert=
				!charstring::compareIgnoringCase(q,"insert",6);
		if (isinsert) {
			q+=6;
			isinsert=character::isWhitespace(*q);
		}
		if (isinsert) {
			while (character::isWhitespace(*q) && q!=end) {
				q++;
			}
			isinsert=(q!=end);
		}
		if (isinsert) {
			isinsert=!charstring::compareIgnoringCase(q,"into",4);
		}
		if (isinsert) {
			q+=4;
			isinsert=character::isWhitespace(*q);
		}
		if (isinsert) {
			while (character::isWhitespace(*q) && q!=end) {
				q++;
			}
			isinsert=(q!=end);
		}
		const char	*table=q;
		if (isinsert) {
			while (!character::isWhitespace(*q) && q!=end) {
				q++;
			}
			isinsert=(q!=end);
		} 
		if (isinsert) {
			const char	*space=q;
			s.append("select * from ")->append(table,space-table);
			query=s.getString();
			querylen=s.getSize();

			req->fudgeselect=true;
		}
	}

	// prepare the query
	debugStart("prepare query",1);
	bool	retval=cont->prepareQuery(req->cur,
					query,querylen,
					true,true,true);
	if (getDebug()) {
		stdoutput.printf("		result: %s\n",
					(retval)?"success":"error");
	}
	debugEnd(1);
	return retval;
}

bool sqlrprotocol_teradata::executeQuery() {

	debugStart("execute query",1);

	// execute the query
	bool	retval=cont->executeQuery(req->cur,true,true,true,true);

	if (getDebug()) {
		stdoutput.printf("		result: %s\n",
					(retval)?"success":"error");
	}
	debugEnd(1);

	return retval;
}

bool sqlrprotocol_teradata::parseCancelParcel(const byte_t *parcel,
						const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);
	if (parcelflavor!=7) {
		unexpectedParcel(parcelflavor);
		*parcelout=parcel;
		return false;
	}

	debugParcelStart("recv","cancel",parcelflavor,parceldatalength);

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

void sqlrprotocol_teradata::parseGenericParcels(const byte_t *parcel,
							const byte_t *end) {
	while (parcel!=end) {
		parseGenericParcel(parcel,&parcel);
	}
}

bool sqlrprotocol_teradata::parseGenericParcel(const byte_t *parcel,
						const byte_t **parcelout) {

	// parse parcel header
	uint16_t	parcelflavor;
	const byte_t	*parceldata;
	uint32_t	parceldatalength;
	parseParcelHeader(parcel,&parcelflavor,
					&parceldatalength,
					&parceldata);

	debugParcelStart("recv","generic",parcelflavor,parceldatalength);

	// return next parcel
	*parcelout=parceldata+parceldatalength;

	debugParcelEnd(parceldata,parceldatalength);

	return true;
}

void sqlrprotocol_teradata::appendParcelHeader(uint16_t flavor,
						uint32_t datalength) {
	if (datalength>=65536) {
		appendLargeParcelHeader(flavor,datalength);
	} else {
		appendSmallParcelHeader(flavor,datalength);
	}
}

void sqlrprotocol_teradata::appendLargeParcelHeader(uint16_t flavor,
							uint32_t datalength) {
	write(&respdata,(uint16_t)(flavor|0x8000));
	write(&respdata,(uint16_t)0);
	write(&respdata,(uint32_t)(2+2+4+datalength));
}

void sqlrprotocol_teradata::appendSmallParcelHeader(uint16_t flavor,
							uint32_t datalength) {
	write(&respdata,flavor);
	write(&respdata,(uint16_t)(2+2+datalength));
}

void sqlrprotocol_teradata::appendParcelHeader(uint16_t flavor) {
	write(&respdata,(uint16_t)(flavor|0x8000));
	write(&respdata,(uint16_t)0);
	req->parcelsizepos=respdata.getPosition();
	write(&respdata,(uint32_t)0);
}

void sqlrprotocol_teradata::endParcel() {
	size_t	originalpos=respdata.getPosition();
	respdata.setPositionRelativeToBeginning(req->parcelsizepos);
	respdata.write(hostTo((uint32_t)(2+2+4+
					originalpos-
					req->parcelsizepos-
					sizeof(uint32_t))));
	respdata.setPositionRelativeToBeginning(originalpos);
}

void sqlrprotocol_teradata::appendConfigResponseParcel() {

	debugParcelStart("send","config response",43);

	appendSmallParcelHeader(43,538);

	appendConfigResponseParcelHeader();
	appendConfigResponseField78();
	appendConfigResponseField84();
	appendConfigResponseField49();
	appendConfigResponseField9();
	appendConfigResponseField10();
	appendConfigResponseField11();
	appendConfigResponseField12();
	appendConfigResponseField13();
	appendConfigResponseVersions();
	appendConfigResponseField14();
	appendConfigResponseField15();
	appendConfigResponseField16();
	appendConfigResponseField6();

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendConfigResponseParcelHeader() {

	// header...
	uint32_t	unknown1=1000;
	uint32_t	unknown2=1000;
	uint16_t	unknown3=120;
	uint16_t	unknown4=1;
	// maybe?  0-30719 is the range for vprocids
	uint16_t	vprocid=30719;
	uint16_t	unknown5=0;
	uint16_t	unknown6=2;
	uint16_t	unknown7=0;
	uint16_t	unknown8=1;
	byte_t		unknown9=(!getProtocolIsBigEndian())?0x7F:0xFF;
	byte_t		unknown10=0;
	write(&respdata,unknown1);
	write(&respdata,unknown2);
	write(&respdata,unknown3);
	write(&respdata,unknown4);
	write(&respdata,vprocid);
	write(&respdata,unknown5);
	write(&respdata,unknown6);
	write(&respdata,unknown7);
	write(&respdata,unknown8);
	write(&respdata,unknown9);
	write(&respdata,unknown10);
	if (getDebug()) {
		stdoutput.printf("		header:\n");
		stdoutput.printf("			"
					"unknown1: %d\n",unknown1);
		stdoutput.printf("			"
					"unknown2: %d\n",unknown2);
		stdoutput.printf("			"
					"unknown3: %d\n",unknown3);
		stdoutput.printf("			"
					"unknown4: %d\n",unknown4);
		stdoutput.printf("			"
					"vprocid: %d\n",vprocid);
		stdoutput.printf("			"
					"unknown5: %d\n",unknown5);
		stdoutput.printf("			"
					"unknown6: %d\n",unknown6);
		stdoutput.printf("			"
					"unknown7: %d\n",unknown7);
		stdoutput.printf("			"
					"unknown8: %d\n",unknown8);
		stdoutput.printf("			"
					"unknown9: %d (0x%02x)\n",
							unknown9,unknown9);
		stdoutput.printf("			"
					"unknown10: %d\n",unknown10);
	}

	// supported character sets...
	uint16_t	count=4;
	byte_t		codes1[]={
		0x3e, 0x3f, 0x7f, 0x40
	};
	byte_t		codes2[]={
		0xbe, 0xbf, 0xff, 0xc0
	};
	const byte_t		*codes=
				(!getProtocolIsBigEndian())?codes1:codes2;
	byte_t		marker=0;
	const char	*strings[]={
		"UTF16                         ",
		"UTF8                          ",
		"ASCII                         ",
		"EBCDIC                        "
	};
	write(&respdata,count);
	for (uint16_t i=0; i<count; i++) {
		write(&respdata,codes[i]);
		write(&respdata,marker);
		write(&respdata,strings[i],30);
	}
	if (getDebug()) {
		stdoutput.printf("		charsets:\n");
		for (uint16_t i=0; i<count; i++) {
			stdoutput.printf("			"
						"0x%02x - '%s'\n",
						codes[i],strings[i]);
		}
	}
}

void sqlrprotocol_teradata::appendConfigResponseField78() {

	// ???
	byte_t		field=78;
	byte_t		unknown1=1;
	uint16_t	unknown2=1;
	uint16_t	unknown3=1;
	write(&respdata,field);
	write(&respdata,unknown1);
	write(&respdata,unknown2);
	write(&respdata,unknown3);
	if (getDebug()) {
		stdoutput.printf("		"
					"field: %d (0x%02x)\n",field,field);
		stdoutput.printf("			"
					"unknown1: %d\n",unknown1);
		stdoutput.printf("			"
					"unknown2: %d\n",unknown2);
		stdoutput.printf("			"
					"unknown3: %d\n",unknown3);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField84() {

	// ???
	byte_t		field=84;
	uint16_t	unknown1=7;
	uint16_t	unknown2=140;
	write(&respdata,field);
	write(&respdata,unknown1);
	write(&respdata,unknown2);
	if (getDebug()) {
		stdoutput.printf("		"
					"field: %d (0x%02x)\n",field,field);
		stdoutput.printf("			"
					"unknown1: %d\n",unknown1);
		stdoutput.printf("			"
					"unknown2: %d\n",unknown2);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField49() {

	// ???
	byte_t		field=49;
	byte_t		unknown1=0;
	uint16_t	unknown2=100;
	byte_t		unknown3=0;
	uint16_t	unknown4=250;
	byte_t		unknown5=0;
	byte_t		unknown6=(!getProtocolIsBigEndian())?64:0;
	uint16_t	unknown7=3906;
	byte_t		unknown8_1[]={
		0x00, 0x00, 0x06
	};
	byte_t		unknown8_2[]={
		0x40, 0x00, 0x00, 0x00, 0x00
	};
	const byte_t	*unknown8=(!getProtocolIsBigEndian())?
							unknown8_1:unknown8_2;
	uint16_t	unknown8size=(!getProtocolIsBigEndian())?
							sizeof(unknown8_1):
							sizeof(unknown8_2);
	uint16_t	unknown9=31999;
	byte_t		unknown10_1[]={
		0x00, 0x00, 0x00, 0x00, 0x00
	};
	byte_t		unknown10_2[]={
		0x06, 0x00, 0x00
	};
	const byte_t	*unknown10=(!getProtocolIsBigEndian())?
							unknown10_1:unknown10_2;
	uint16_t	unknown10size=(!getProtocolIsBigEndian())?
							sizeof(unknown10_1):
							sizeof(unknown10_2);
	uint16_t	unknown11=28672;
	uint16_t	unknown12=0;
	uint16_t	unknown13=65528;
	uint16_t	unknown14=0;
	uint16_t	unknown15=1;
	byte_t		unknown16=0;
	uint32_t	unknown17=191;
	uint32_t	unknown18=16;
	uint16_t	unknown19=0;
	uint16_t	unknown20=65535;
	uint16_t	unknown21=0;
	uint16_t	unknown22=2048;
	uint16_t	unknown23=0;
	uint16_t	unknown24=128;
	uint16_t	unknown25=0;
	uint16_t	unknown26=64;
	uint16_t	unknown27=0;

	uint16_t	unknown28=2536;
	byte_t		unknown29=0;
	uint32_t	unknown30=262144000;
	byte_t		unknown31=0;
	uint16_t	unknown32=62000;
	uint16_t	unknown32a=0;
	uint16_t	unknown33=31000;
	uint16_t	unknown34=0;
	uint16_t	unknown35=38;
	uint16_t	unknown35a=0;
	byte_t		unknown36=0;
	uint16_t	unknown37=250;
	uint16_t	unknown38=0;
	uint16_t	unknown39=250;
	uint16_t	unknown40=0;
	uint16_t	unknown41=250;
	uint16_t	unknown42=0;
	uint16_t	unknown43=125;
	uint16_t	unknown44=0;
	uint16_t	unknown45=125;
	uint16_t	unknown46=0;
	uint16_t	unknown47=250;
	uint16_t	unknown48=0;
	uint16_t	unknown49=250;
	byte_t		unknown50=0;
	uint32_t	unknown51=2536;
	uint32_t	unknown52=6;
	uint32_t	unknown53=6;
	uint32_t	unknown54=6;
	uint32_t	unknown55=1000;
	byte_t		unknown56=0;
	uint16_t	unknown57=4000;
	uint16_t	unknown58=0;
	uint16_t	unknown59=65532;
	byte_t		unknown60=0;
	uint32_t	unknown61=1048500;
	byte_t		unknown62=0;
	uint16_t	unknown63=250;
	byte_t		unknown64=0;

	write(&respdata,field);
	write(&respdata,unknown1);
	write(&respdata,unknown2);
	write(&respdata,unknown3);
	write(&respdata,unknown4);
	write(&respdata,unknown5);
	write(&respdata,unknown6);
	write(&respdata,unknown7);
	write(&respdata,unknown8,unknown8size);
	write(&respdata,unknown9);
	write(&respdata,unknown10,unknown10size);
	write(&respdata,unknown11);
	write(&respdata,unknown12);
	write(&respdata,unknown13);
	write(&respdata,unknown14);
	write(&respdata,unknown15);
	write(&respdata,unknown16);
	write(&respdata,unknown17);
	write(&respdata,unknown18);
	if (getProtocolIsBigEndian()) {
		write(&respdata,unknown19);
	}
	write(&respdata,unknown20);
	write(&respdata,unknown21);
	write(&respdata,unknown22);
	write(&respdata,unknown23);
	write(&respdata,unknown24);
	write(&respdata,unknown25);
	write(&respdata,unknown26);
	write(&respdata,unknown27);
	write(&respdata,unknown28);
	write(&respdata,unknown29);
	write(&respdata,unknown30);
	write(&respdata,unknown31);
	write(&respdata,unknown32);
	write(&respdata,unknown32a);
	write(&respdata,unknown33);
	write(&respdata,unknown34);
	write(&respdata,unknown35);
	if (!getProtocolIsBigEndian()) {
		write(&respdata,unknown35a);
	}
	write(&respdata,unknown36);
	write(&respdata,unknown37);
	write(&respdata,unknown38);
	write(&respdata,unknown39);
	write(&respdata,unknown40);
	write(&respdata,unknown41);
	write(&respdata,unknown42);
	write(&respdata,unknown43);
	write(&respdata,unknown44);
	write(&respdata,unknown45);
	write(&respdata,unknown46);
	write(&respdata,unknown47);
	write(&respdata,unknown48);
	write(&respdata,unknown49);
	write(&respdata,unknown50);
	write(&respdata,unknown51);
	write(&respdata,unknown52);
	write(&respdata,unknown53);
	write(&respdata,unknown54);
	write(&respdata,unknown55);
	write(&respdata,unknown56);
	write(&respdata,unknown57);
	write(&respdata,unknown58);
	write(&respdata,unknown59);
	write(&respdata,unknown60);
	write(&respdata,unknown61);
	write(&respdata,unknown62);
	write(&respdata,unknown63);
	write(&respdata,unknown64);
	if (getDebug()) {
		stdoutput.printf("		"
					"field: %d (0x%02x)\n",field,field);
		stdoutput.printf("			"
					"unknown1: %d\n",unknown1);
		stdoutput.printf("			"
					"unknown2: %d\n",unknown2);
		stdoutput.printf("			"
					"unknown3: %d\n",unknown3);
		stdoutput.printf("			"
					"unknown4: %d\n",unknown4);
		stdoutput.printf("			"
					"unknown5: %d\n",unknown5);
		stdoutput.printf("			"
					"unknown6: %d\n",unknown6);
		stdoutput.printf("			"
					"unknown7: %d (0x%04x)\n",
							unknown7,unknown7);
		stdoutput.printf("			"
					"unknown8:\n");
		debugHexDump(unknown8,unknown8size,3);
		stdoutput.printf("			"
					"unknown9: %d (0x%04x)\n",
							unknown9,unknown9);
		stdoutput.printf("			"
					"unknown10:\n");
		debugHexDump(unknown10,unknown10size,3);
		stdoutput.printf("			"
					"unknown11: %d (0x%04x)\n",
							unknown11,unknown11);
		stdoutput.printf("			"
					"unknown12: %d\n",unknown12);
		stdoutput.printf("			"
					"unknown13: %d (0x%04x)\n",
							unknown13,unknown13);
		stdoutput.printf("			"
					"unknown14: %d\n",unknown14);
		stdoutput.printf("			"
					"unknown15: %d\n",unknown15);
		stdoutput.printf("			"
					"unknown16: %d\n",unknown16);
		stdoutput.printf("			"
					"unknown17: %d\n",unknown17);
		stdoutput.printf("			"
					"unknown18: %d\n",unknown18);
		if (getProtocolIsBigEndian()) {
			stdoutput.printf("			"
					"unknown19: %d\n",unknown19);
		} else {
			stdoutput.printf("			"
					"unknown19: omitted\n");
		}
		stdoutput.printf("			"
					"unknown20: %d\n",unknown20);
		stdoutput.printf("			"
					"unknown21: %d\n",unknown21);
		stdoutput.printf("			"
					"unknown22: %d\n",unknown22);
		stdoutput.printf("			"
					"unknown23: %d\n",unknown23);
		stdoutput.printf("			"
					"unknown24: %d\n",unknown24);
		stdoutput.printf("			"
					"unknown25: %d\n",unknown25);
		stdoutput.printf("			"
					"unknown26: %d\n",unknown26);
		stdoutput.printf("			"
					"unknown27: %d\n",unknown27);
		stdoutput.printf("			"
					"unknown28: %d (0x%04x)\n",
							unknown28,unknown28);
		stdoutput.printf("			"
					"unknown29: %d\n",unknown29);
		stdoutput.printf("			"
					"unknown30: %d (0x%04x)\n",
							unknown30,unknown30);
		stdoutput.printf("			"
					"unknown31: %d\n",unknown31);
		stdoutput.printf("			"
					"unknown32: %d\n",unknown32);
		stdoutput.printf("			"
					"unknown32a: %d\n",unknown32a);
		stdoutput.printf("			"
					"unknown33: %d\n",unknown33);
		stdoutput.printf("			"
					"unknown34: %d\n",unknown34);
		stdoutput.printf("			"
					"unknown35: %d\n",unknown35);
		if (!getProtocolIsBigEndian()) {
			stdoutput.printf("			"
					"unknown35a: %d\n",unknown35a);
		} else {
			stdoutput.printf("			"
					"unknown35a: omitted\n");
		}
		stdoutput.printf("			"
					"unknown36: %d\n",unknown36);
		stdoutput.printf("			"
					"unknown37: %d\n",unknown37);
		stdoutput.printf("			"
					"unknown38: %d\n",unknown38);
		stdoutput.printf("			"
					"unknown39: %d\n",unknown39);
		stdoutput.printf("			"
					"unknown40: %d\n",unknown40);
		stdoutput.printf("			"
					"unknown41: %d\n",unknown41);
		stdoutput.printf("			"
					"unknown42: %d\n",unknown42);
		stdoutput.printf("			"
					"unknown43: %d\n",unknown43);
		stdoutput.printf("			"
					"unknown44: %d\n",unknown44);
		stdoutput.printf("			"
					"unknown45: %d\n",unknown45);
		stdoutput.printf("			"
					"unknown46: %d\n",unknown46);
		stdoutput.printf("			"
					"unknown47: %d\n",unknown47);
		stdoutput.printf("			"
					"unknown48: %d\n",unknown48);
		stdoutput.printf("			"
					"unknown49: %d\n",unknown49);
		stdoutput.printf("			"
					"unknown50: %d\n",unknown50);
		stdoutput.printf("			"
					"unknown51: %d\n",unknown51);
		stdoutput.printf("			"
					"unknown52: %d\n",unknown52);
		stdoutput.printf("			"
					"unknown53: %d\n",unknown53);
		stdoutput.printf("			"
					"unknown54: %d\n",unknown54);
		stdoutput.printf("			"
					"unknown55: %d\n",unknown55);
		stdoutput.printf("			"
					"unknown56: %d\n",unknown56);
		stdoutput.printf("			"
					"unknown57: %d\n",unknown57);
		stdoutput.printf("			"
					"unknown58: %d\n",unknown58);
		stdoutput.printf("			"
					"unknown59: %d\n",unknown59);
		stdoutput.printf("			"
					"unknown60: %d\n",unknown60);
		stdoutput.printf("			"
					"unknown61: %d\n",unknown61);
		stdoutput.printf("			"
					"unknown62: %d\n",unknown62);
		stdoutput.printf("			"
					"unknown63: %d\n",unknown63);
		stdoutput.printf("			"
					"unknown64: %d\n",unknown64);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField9() {

	uint16_t	field=9;
	uint16_t	unknown1=1;
	byte_t		unknown2=1;
	write(&respdata,field);
	write(&respdata,unknown1);
	write(&respdata,unknown2);
	if (getDebug()) {
		stdoutput.printf("		"
					"field: %d (0x%02x)\n",field,field);
		stdoutput.printf("			"
					"unknown1: %d\n",unknown1);
		stdoutput.printf("			"
					"unknown2: %d\n",unknown2);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField10() {

	uint16_t	field=10;
	byte_t		data[]={
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
		0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
		0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x02
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)sizeof(data));
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data:\n");
		debugHexDump(data,sizeof(data));
	}
}

void sqlrprotocol_teradata::appendConfigResponseField11() {

	uint16_t	field=11;
	byte_t		data[]={
		0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x01
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)sizeof(data));
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data:\n");
		debugHexDump(data,sizeof(data));
	}
}

void sqlrprotocol_teradata::appendConfigResponseField12() {

	uint16_t	field=12;
	byte_t		data[]={
		0x01, 0x01, 0x01, 0x02, 0x01, 0x01
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)sizeof(data));
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data:\n");
		debugHexDump(data,sizeof(data),3);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField13() {

	uint16_t	field=13;
	write(&respdata,field);
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
	}
}

void sqlrprotocol_teradata::appendConfigResponseVersions() {

	uint16_t	field=62;
	const char	*version1="16.20.12.01                   ";
	const char	*version2="16.20.12.01                     ";
	write(&respdata,field);
	write(&respdata,version1,30);
	write(&respdata,version2,32);
	if (getDebug()) {
		stdoutput.printf("		versions:\n");
		stdoutput.printf("			field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			version1: '%s'\n",
								version1);
		stdoutput.printf("			version2: '%s'\n",
								version2);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField14() {

	uint16_t	field=14;
	byte_t		data[]={
		0x03, 0x03, 0x02, 0x03
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)sizeof(data));
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data:\n");
		debugHexDump(data,sizeof(data),3);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField15() {

	uint16_t	field=15;
	byte_t		data[]={
		0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
		0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
		0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)sizeof(data));
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data:\n");
		debugHexDump(data,sizeof(data));
	}
}

void sqlrprotocol_teradata::appendConfigResponseField16() {

	uint16_t	field=16;
	byte_t		data1[]={
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00
	};
	uint16_t	unknown=32770;
	byte_t		data2[]={
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)20);
	write(&respdata,data1,sizeof(data1));
	write(&respdata,unknown);
	write(&respdata,data2,sizeof(data2));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data1:\n");
		debugHexDump(data1,sizeof(data1),3);
		stdoutput.printf("			unknown: %d (0x%02x)\n",
							unknown,unknown);
		stdoutput.printf("			data2:\n");
		debugHexDump(data2,sizeof(data2),3);
	}
}

void sqlrprotocol_teradata::appendConfigResponseField6() {

	uint16_t	field=6;
	byte_t		data[]={
		0x01, 0x49
	};
	write(&respdata,field);
	write(&respdata,(uint16_t)sizeof(data));
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		field: %d (0x%02x)\n",
								field,field);
		stdoutput.printf("			data:\n");
		debugHexDump(data,sizeof(data),3);
	}
}

void sqlrprotocol_teradata::appendGatewayConfigParcel() {

	debugParcelStart("send","gateway config",43);

	appendSmallParcelHeader(165,66);

	uint32_t	marker=1;
	uint16_t	field1=1;
	byte_t		data1[]={
		0x01
	};
	uint16_t	releasefield=2;
	byte_t		releasedata[]={
		// 16.20.12.01
		0x10, 0x14, 0x0C, 0x01
	};
	uint16_t	field3=3;
	uint16_t	field4=4;
	uint16_t	data4=33;
	uint16_t	field5=5;
	uint16_t	field6=6;
	uint16_t	field7=7;
	uint16_t	field8=8;
	uint16_t	field9=9;
	uint16_t	field10=10;
	byte_t		data10[]={
		0x01
	};
	uint16_t	field11=11;
	byte_t		data11[]={
		0x01
	};
	uint16_t	field12=12;
	byte_t		data12[]={
		0x01
	};
	// no field 13?
	uint16_t	field14=14;
	write(&respdata,marker);
	write(&respdata,field1);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						sizeof(data1)));
	write(&respdata,data1,sizeof(data1));
	write(&respdata,releasefield);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						sizeof(releasedata)));
	write(&respdata,releasedata,sizeof(releasedata));
	// field 3 always empty?
	write(&respdata,field3);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	write(&respdata,field4);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						sizeof(data4)));
	write(&respdata,data4);
	// note that 5 and 6 are reversed
	// field 6 always empty?
	write(&respdata,field6);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	// field 5 always empty?
	write(&respdata,field5);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	// field 7 always empty?
	write(&respdata,field7);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	// field 8 always empty?
	write(&respdata,field8);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	// field 9 always empty?
	write(&respdata,field9);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	write(&respdata,field10);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						sizeof(data10)));
	write(&respdata,data10,sizeof(data10));
	write(&respdata,field11);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						sizeof(data11)));
	write(&respdata,data11,sizeof(data11));
	write(&respdata,field12);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						sizeof(data12)));
	write(&respdata,data12,sizeof(data12));
	// field 14 always empty?
	write(&respdata,field14);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+sizeof(uint16_t)));
	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		field 1:\n");
		debugHexDump(data1,sizeof(data1),3);
		stdoutput.printf("		release:\n");
		debugHexDump(releasedata,sizeof(releasedata),3);
		stdoutput.printf("		field 3:\n");
		stdoutput.printf("		field 4:\n");
		stdoutput.printf("			%d\n",data4);
		stdoutput.printf("		field 6:\n");
		stdoutput.printf("		field 5:\n");
		stdoutput.printf("		field 7:\n");
		stdoutput.printf("		field 8:\n");
		stdoutput.printf("		field 9:\n");
		stdoutput.printf("		field 10:\n");
		debugHexDump(data10,sizeof(data10),3);
		stdoutput.printf("		field 11:\n");
		debugHexDump(data11,sizeof(data11),3);
		stdoutput.printf("		field 12:\n");
		debugHexDump(data12,sizeof(data12),3);
		stdoutput.printf("		field 14:\n");
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendTd2MechanismParcel() {

	// bail if disabled
	if (!td2mechenabled) {
		return;
	}

	// teradata2 mechanism
	debugParcelStart("send","auth mechanism (td2)",167);

	appendSmallParcelHeader(167,45);

	uint32_t	marker=1;
	uint16_t	field16=16;
	uint32_t	field16data1=3;
	uint32_t	field16data2=1;
	uint16_t	field17=17;
	uint32_t	field17data1=1;
	uint32_t	field17data2=20;
	write(&respdata,marker);
	write(&respdata,(uint32_t)sizeof(td2mech));
	write(&respdata,td2mech,sizeof(td2mech));
	write(&respdata,field16);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						2*sizeof(uint32_t)));
	write(&respdata,field16data1);
	write(&respdata,field16data2);
	write(&respdata,field17);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						2*sizeof(uint32_t)));
	write(&respdata,field17data1);
	write(&respdata,field17data2);
	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		mech:\n");
		debugHexDump(td2mech,sizeof(td2mech),2);
		stdoutput.printf("		field 16: %d,%d\n",
						field16data1,field16data2);
		stdoutput.printf("		field 17: %d,%d\n",
						field17data1,field17data2);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendTdNegoMechanismParcel() {

	// bail if disabled
	if (!tdnegomechenabled) {
		return;
	}

	// teradata negotiation mechanism
	debugParcelStart("send","auth mechanism (tdnego)",167);

	appendSmallParcelHeader(167,32);

	uint32_t	marker=1;
	uint16_t	field17=17;
	uint32_t	field17data1=1;
	uint32_t	field17data2=70;
	write(&respdata,marker);
	write(&respdata,(uint32_t)sizeof(tdnegomech));
	write(&respdata,tdnegomech,sizeof(tdnegomech));
	write(&respdata,field17);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						2*sizeof(uint32_t)));
	write(&respdata,field17data1);
	write(&respdata,field17data2);
	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		mech:\n");
		debugHexDump(tdnegomech,sizeof(tdnegomech),2);
		stdoutput.printf("		field 17: %d,%d\n",
						field17data1,field17data2);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendLdapMechanismParcel() {

	// bail if disabled
	if (!ldapmechenabled) {
		return;
	}

	// ldap mechanism
	debugParcelStart("send","auth mechanism (ldap)",167);

	appendSmallParcelHeader(167,29);

	uint32_t	marker=1;
	uint16_t	field17=17;
	uint32_t	field17data1=1;
	uint32_t	field17data2=40;
	write(&respdata,marker);
	write(&respdata,(uint32_t)sizeof(ldapmech));
	write(&respdata,ldapmech,sizeof(ldapmech));
	write(&respdata,field17);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						2*sizeof(uint32_t)));
	write(&respdata,field17data1);
	write(&respdata,field17data2);
	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		mech:\n");
		debugHexDump(ldapmech,sizeof(ldapmech),2);
		stdoutput.printf("		field 17: %d,%d\n",
						field17data1,field17data2);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendKrbMechanismParcel() {

	// bail if disabled
	if (!krbmechenabled) {
		return;
	}

	// kerberos mechanism
	debugParcelStart("send","auth mechanism (krb)",167);

	appendSmallParcelHeader(167,26);

	uint32_t	marker=1;
	uint16_t	field17=17;
	uint32_t	field17data1=1;
	uint32_t	field17data2=65;
	write(&respdata,marker);
	write(&respdata,(uint32_t)sizeof(krbmech));
	write(&respdata,krbmech,sizeof(krbmech));
	write(&respdata,field17);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						2*sizeof(uint32_t)));
	write(&respdata,field17data1);
	write(&respdata,field17data2);
	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		mech:\n");
		debugHexDump(krbmech,sizeof(krbmech),2);
		stdoutput.printf("		field 17: %d,%d\n",
						field17data1,field17data2);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendKrbCompatMechanismParcel() {

	// bail if disabled
	if (!krbcompatmechenabled) {
		return;
	}

	// kerberos compatibility mechanism
	debugParcelStart("send","auth mechanism (krbcompat)",167);

	appendSmallParcelHeader(167,33);

	uint32_t	marker=1;
	uint16_t	field17=17;
	uint32_t	field17data1=1;
	uint32_t	field17data2=10;
	write(&respdata,marker);
	write(&respdata,(uint32_t)sizeof(krbcompatmech));
	write(&respdata,krbcompatmech,sizeof(krbcompatmech));
	write(&respdata,field17);
	write(&respdata,(uint16_t)(sizeof(uint16_t)+
						sizeof(uint16_t)+
						2*sizeof(uint32_t)));
	write(&respdata,field17data1);
	write(&respdata,field17data2);
	if (getDebug()) {
		stdoutput.printf("		marker: %d\n",marker);
		stdoutput.printf("		mech:\n");
		debugHexDump(krbcompatmech,sizeof(krbcompatmech),2);
		stdoutput.printf("		field 17: %d,%d\n",
						field17data1,field17data2);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendLogonFailureParcel(const char *errorstring) {

	debugParcelStart("send","failure",9);
	if (getDebug()) {
		stdoutput.printf("		error: %s\n",errorstring);
	}
	debugParcelEnd();

	// failure parcel
	appendParcelHeader(9,2+2+charstring::getLength(errorstring));

	// statement number
	write(&respdata,(uint16_t)0);

	// info ???
	write(&respdata,(uint16_t)0);

	// msg
	write(&respdata,errorstring);
}

void sqlrprotocol_teradata::setSessionNumber() {

	// FIXME: get this from a file or something...
	sessionno=1234;
}

void sqlrprotocol_teradata::appendAssignResponseParcel() {

	debugParcelStart("send","assign response",101);

	appendSmallParcelHeader(101,94);

	const char	*unknownspaces1="        ";
	byte_t		unknowndata1[]={
		0x01, 0x04, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	const char	*unknownspaces2="                "
					"                ";
	const char	*releaseandversion="16.20.16.20.12.01   ";
	uint16_t	hostid=(!getProtocolIsBigEndian())?1025:2049;
	write(&respdata,unknownspaces1);
	write(&respdata,unknowndata1,sizeof(unknowndata1));
	write(&respdata,unknownspaces2);
	write(&respdata,releaseandversion);
	write(&respdata,hostid);
	if (getDebug()) {
		stdoutput.printf("		release and version: %s\n",
							releaseandversion);
		stdoutput.printf("		host id: %d\n",hostid);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendSsoResponseParcel() {

	debugParcelStart("send","sso response",134);

	appendSmallParcelHeader(134,956);

	byte_t		marker[]={
		0x00, 0x00, 0x01, 0x00
	};
	uint16_t	postmarkerlength=950;
	byte_t		unknown[]={
		0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0x03, 0xA6,
		0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x14, 0x0C, 0x01, 0x00, 0x00, 0x01, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
#ifdef DECRYPT
	if (!generateServerPublicKey()) {
		// FIXME: fail somehow
	}
#endif

	write(&respdata,marker,sizeof(marker));
	write(&respdata,postmarkerlength);
	write(&respdata,unknown,sizeof(unknown));
	write(&respdata,dhp,sizeof(dhp));
	write(&respdata,dhg,sizeof(dhg));
	write(&respdata,serverpubkey,sizeof(serverpubkey));
	if (getDebug()) {
		stdoutput.printf("		marker:\n");
		debugHexDump(marker,sizeof(marker),2);
		stdoutput.printf("		unknown:\n");
		debugHexDump(unknown,sizeof(unknown));
		stdoutput.printf("		dh \"p\":\n");
		debugHexDump(dhp,sizeof(dhp));
		stdoutput.printf("		dh \"g\":\n");
		debugHexDump(dhg,sizeof(dhg));
		stdoutput.printf("		server public key:\n");
		debugHexDump(serverpubkey,sizeof(serverpubkey));
	}

	// get qop parameters
	byte_t		confalg=ALG_AES;
	byte_t		mode=CONF_ALG_MODE_GCM;
	byte_t		padding=CONF_ALG_PADDING_PKCS5;
	uint16_t	confalgkeysize=128;
	byte_t		intalg=ALG_SHA256;
	byte_t		kexalg=ALG_DH;
	uint16_t	kexalgkeysize=2048;
	switch (negotiatedqop) {
		case QOP_AES_K128_GCM_PKCS5Padding_SHA2:
			break;
		case QOP_AES_K128_CBC_PKCS5Padding_SHA1:
			mode=CONF_ALG_MODE_CBC;
			intalg=ALG_SHA1;
			break;
		case QOP_AES_K192_GCM_PKCS5Padding_SHA2:
			confalgkeysize=192;
			break;
		case QOP_AES_K192_CBC_PKCS5Padding_SHA1:
			mode=CONF_ALG_MODE_CBC;
			confalgkeysize=192;
			intalg=ALG_SHA1;
			break;
		case QOP_AES_K256_GCM_PKCS5Padding_SHA2:
			confalgkeysize=256;
			break;
		case QOP_AES_K256_CBC_PKCS5Padding_SHA1:
			mode=CONF_ALG_MODE_CBC;
			confalgkeysize=256;
			intalg=ALG_SHA1;
			break;
	}

	// supported QOPs (Quality of Protection)
	write(&respdata,(byte_t)0xe3);
	write(&respdata,(byte_t)100);
	if (getDebug()) {
		stdoutput.printf("		supported QOPs {\n");
	}

	// the server sends 4 QOPs, and for some reason all 4 are the same...
	byte_t	qops[]={
		0xe4, 0xe5, 0xe6, 0xe7
	};
	for (byte_t i=0; i<4; i++) {

		// QOP
		write(&respdata,qops[i]);
		write(&respdata,(byte_t)23);

		// confidentiality algorithm
		write(&respdata,(byte_t)CONF_ALG);
		write(&respdata,(byte_t)1);
		write(&respdata,confalg);

		// mode
		write(&respdata,(byte_t)CONF_ALG_MODE);
		write(&respdata,(byte_t)1);
		write(&respdata,mode);

		// padding
		write(&respdata,(byte_t)CONF_ALG_PADDING);
		write(&respdata,(byte_t)1);
		write(&respdata,padding);

		// confidentiality algorithm key size
		write(&respdata,(byte_t)CONF_ALG_KEY_SIZE);
		write(&respdata,(byte_t)2);
		writeBE(&respdata,confalgkeysize);

		// integrity algorithm
		write(&respdata,(byte_t)INT_ALG);
		write(&respdata,(byte_t)1);
		write(&respdata,intalg);

		// key exchange algorithm
		write(&respdata,(byte_t)KEX_ALG);
		write(&respdata,(byte_t)1);
		write(&respdata,kexalg);

		// key exchange algorithm key size
		write(&respdata,(byte_t)KEX_ALG_KEY_SIZE);
		write(&respdata,(byte_t)2);
		writeBE(&respdata,kexalgkeysize);

		if (getDebug()) {
			stdoutput.printf("			QOP %d {\n",i);
			stdoutput.printf("				"
				"conf alg: %s\n",algstr[confalg]);
			stdoutput.printf("				"
				"mode: %s\n",confalgmodestr[mode]);
			stdoutput.printf("				"
				"padding: %s\n",confalgpaddingstr[padding]);
			stdoutput.printf("				"
				"conf alg key size: %d\n",confalgkeysize);
			stdoutput.printf("				"
				"int alg: %s\n",algstr[intalg]);
			stdoutput.printf("				"
				"kex alg: %s\n",algstr[kexalg]);
			stdoutput.printf("				"
				"kex alg key size: %d\n",kexalgkeysize);
			stdoutput.printf("			}\n");
		}
	}
	if (getDebug()) {
		stdoutput.printf("		}\n");
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendSsoParcel() {

	debugParcelStart("send","sso",134);

	appendSmallParcelHeader(134,7);

	byte_t	data[]={
		0x00, 0x01, 0x03, 0x00, 0x00, 0x00
	};
	write(&respdata,data,sizeof(data));
	if (getDebug()) {
		stdoutput.printf("		data:\n");
		debugHexDump(data,sizeof(data),2);
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendSuccessParcel() {

	// statement number (FIXME: see note in appendEndStatementParcel)
	uint16_t	statementnumber=(req)?1:0;

	// handle the activity count
	updateActivityCount();
	uint32_t	activitycount=(req)?req->activitycount:0;

	// warning code (always 0 as SQL Relay doesn't support warnings)
	uint16_t	warningcode=0;

	// field count
	uint16_t	fieldcount=0;
	if (req) {
		req->fieldcount=cont->colCount(req->cur);
		fieldcount=req->fieldcount;
	}

	// activity type
	uint16_t	activity=(req)?req->activity:0;

	// warning length (always 0 as SQL Relay doesn't support warnings)
	uint16_t	warninglength=0;

	// warning message (always empty as SQL Relay doesn't support warnings)
	const char	*warning="";

	debugParcelStart("send","success",8);
	if (getDebug()) {
		stdoutput.printf("		statement number: %d\n",
							statementnumber);
		stdoutput.printf("		activity count: %d\n",
							activitycount);
		stdoutput.printf("		warning code: %d\n",
							warningcode);
		stdoutput.printf("		field count: %d\n",
							fieldcount);
		stdoutput.printf("		activity: %d\n",
							activity);
		stdoutput.printf("		warning length: %d\n",
							warninglength);
		stdoutput.printf("		warning: %.*s\n",
							warninglength,warning);
	}
	debugParcelEnd();

	// success parcel...
	appendParcelHeader(8,16);

	write(&respdata,statementnumber);
	if (req) {
		req->activitycountpos=respdata.getPosition();
		req->activitycountsize=4;
	}
	write(&respdata,activitycount);
	write(&respdata,warningcode);
	write(&respdata,fieldcount);
	write(&respdata,activity);
	write(&respdata,warninglength);
	write(&respdata,warning,warninglength);

	// "slack" bytes
	write(&respdata,(uint16_t)0);
}

void sqlrprotocol_teradata::updateActivityCount() {

	if (!req) {
		return;
	}

	// set the activity count from the affected rows
	// (unless it's already been set)
	if (!req->activitycount) {
		req->activitycount=cont->affectedRows(req->cur);
	}

	// fudge some particular activity counts
	if ((int64_t)req->activitycount==-1) {
		req->activitycount=0;
	}
	switch (req->activity) {
		case SQL_DATABASE:
			req->activitycount=1;
			break;
		case SQL_DROP_TABLE:
			req->activitycount=26;
			break;
	}
}

void sqlrprotocol_teradata::appendStatementStatusParcel() {
	appendStatementStatusParcel(1);
}

void sqlrprotocol_teradata::appendStatementStatusParcel(
						uint32_t statementnumber) {

	bool	includeext=(req->activity==SQL_SELECT && req->requestmode!='R');

	// statement status ???
	byte_t	statementstatus=0;

	// response mode
	byte_t	responsemode=0;
	switch (req->requestmode) {
		case 'F':
			responsemode=1;
			break;
		case 'R':
			responsemode=2;
			break;
		case 'I':
			responsemode=3;
			break;
		case 'M':
			responsemode=4;
			break;
	}

	// code (FIXME: ???)
	uint16_t	code=0;

	// handle the activity count
	updateActivityCount();

	// get the field count
	req->fieldcount=cont->colCount(req->cur);

	debugParcelStart("send","statement status",205);
	if (getDebug()) {
		stdoutput.printf("		statement status: %d\n",
							statementstatus);
		stdoutput.printf("		response mode: %d\n",
							responsemode);
		stdoutput.printf("		statement number: %d\n",
							statementnumber);
		stdoutput.printf("		code: %d\n",code);
		stdoutput.printf("		activity: %d\n",
							req->activity);
		stdoutput.printf("		activity count: %d\n",
							req->activitycount);
		stdoutput.printf("		field count: %d\n",
							req->fieldcount);
	}
	debugParcelEnd();

	// statement status parcel...
	appendParcelHeader(205,(includeext)?46:28);

	write(&respdata,statementstatus);
	write(&respdata,responsemode);
	// reserved
	write(&respdata,(uint16_t)0);
	write(&respdata,statementnumber);
	write(&respdata,code);
	write(&respdata,req->activity);
	req->activitycountpos=respdata.getPosition();
	req->activitycountsize=8;
	write(&respdata,req->activitycount);
	write(&respdata,req->fieldcount);
	// reserved
	write(&respdata,(uint32_t)0);

	if (includeext) {

		// ess extension header
		// or
		// warning message extension
		// or
		// merge activity counts extension
		// or
		// multiload activity counts extension

		// FIXME: I don't know which of those this is...
		uint16_t	ext1=32;
		uint32_t	ext2=12;
		byte_t		ext[]={
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00
		};
		write(&respdata,ext1);
		write(&respdata,ext2);
		write(&respdata,ext,sizeof(ext));
	}
}

void sqlrprotocol_teradata::appendColumnParcels() {

	switch (req->requestmode) {
		case 'F':
			appendFieldColumnParcels();
			break;
		case 'R':
			// in record mode we don't actually send anything
			return;
		case 'I':
		case 'M':
			if (req->returnstatementinfo=='Y') {
				appendStatementInfoParcel();
				appendStatementInfoEndParcel();
			} else {
				appendDataInfoParcel();
			}
			return;
	}
}

void sqlrprotocol_teradata::appendFieldColumnParcels() {

	uint16_t	colcount=cont->colCount(req->cur);

	// column names...
	appendTitleStartParcel();
	for (uint16_t i=0; i<colcount; i++) {
		appendFieldParcel(
			cont->getColumnName(req->cur,i),
			cont->getColumnNameLength(req->cur,i));
	}
	appendTitleEndParcel();

	// column formats...
	appendFormatStartParcel();
	stringbuffer	fieldformat;
	for (uint16_t i=0; i<colcount; i++) {
		getFieldFormat(&fieldformat,i);
		appendFieldParcel(fieldformat.getString(),
					fieldformat.getSize());
		fieldformat.clear();
	}
	appendFormatEndParcel();

	// column lengths...
	appendSizeStartParcel();
	for (uint16_t i=0; i<colcount; i++) {
		appendSizeParcel(cont->getColumnLength(req->cur,i));
	}
	appendSizeEndParcel();
}

void sqlrprotocol_teradata::getFieldFormat(bytebuffer *fieldformat,
							uint16_t col) {

	const char	*type=getColumnTypeName(col);

	if (!charstring::compare(type,"TINYINT") ||
		!charstring::compare(type,"SMALLINT") ||
		!charstring::compare(type,"INTEGER")) {
		fieldformat->printf("-(%d)9",
				cont->getColumnLength(req->cur,col));
	} else if (!charstring::compare(type,"DECIMAL")) {
		uint16_t	prec=cont->getColumnPrecision(req->cur,col);
		uint16_t	scale=cont->getColumnScale(req->cur,col);
		for (uint16_t i=0; i<prec-scale; i++) {
			fieldformat->write('-');
		}
		if (scale) {
			fieldformat->write('.');
		}
		for (uint16_t i=0; i<scale; i++) {
			fieldformat->write('9');
		}
	} else if (!charstring::compare(type,"NUMBER")) {
		fieldformat->write("FN9");
	} else if (!charstring::compare(type,"FLOAT")) {
		fieldformat->write("-9.99999999999999E-999");
	} else if (!charstring::compare(type,"DATE")) {
		fieldformat->write("YY/MM/DD");
	} else if (!charstring::compare(type,"TIME")) {
		// FIXME: HH24?
		fieldformat->write("HH:MI:SS.S(6)");
	} else if (!charstring::compare(type,"TIMESTAMP") ||
			!charstring::compare(type,"DATETIME")) {
		// FIXME: HH24?
		fieldformat->write("YYYY-MM-DDBHH:MI:SS.S(6)");
	} else {
		// fall back to char/varchar
		fieldformat->printf("X(%d)",
				cont->getColumnLength(req->cur,col));
	}
}

void sqlrprotocol_teradata::appendFieldParcel(const char *data, uint16_t size) {
	debugParcelStart("send","field",18);
	debugHexDump((const byte_t *)data,size,2);
	debugParcelEnd();
	appendParcelHeader(18,size);
	write(&respdata,data,size);
}

void sqlrprotocol_teradata::appendDataInfoParcel() {

	uint16_t	fieldcount=cont->colCount(req->cur);

	appendParcelHeader(71,2+fieldcount*(2+2));

	// field count
	write(&respdata,fieldcount);

	debugParcelStart("send","datainfo",71);
	if (getDebug()) {
		stdoutput.printf("		field count: %d\n",fieldcount);
	}

	for (uint16_t i=0; i<fieldcount; i++) {

		// type
		uint16_t	type=getColumnType(i);
		write(&respdata,type);

		// length
		uint16_t	length=cont->getColumnLength(req->cur,i);
		write(&respdata,length);

		if (getDebug()) {
			stdoutput.printf("		field %d {\n",i);
			stdoutput.printf("		"
					"	type: %d\n",type);
			stdoutput.printf("		"
					"	length: %d\n",length);
			stdoutput.printf("		}\n");
		}
	}

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendStatementInfoParcel() {

	appendParcelHeader(169);
	debugParcelStart("send","statementinformation",169);

	appendEstimatedProcessingTimeExtension(0);
	appendEndEstimatedProcessingTimeExtension();
	uint16_t	colcount=cont->colCount(req->cur);
	for (uint16_t i=0; i<colcount; i++) {
		appendQueryExtension(i);
	}
	appendEndQueryExtension();

	// FIXME - extensions include:
	// 1 - parameter
	// 2 - query
	// 3 - summary
	// 4 - identity column
	// 5 - stored-proc output
	// 6 - stored-proc resultset
	// 7 - estimated processing time
	// when should we send the other ones, and in what order?

	endParcel();

	debugParcelEnd();
}

void sqlrprotocol_teradata::appendEstimatedProcessingTimeExtension(
							uint64_t time) {

	debugExtStart("estimated processing time");
	if (getDebug()) {
		stdoutput.printf("				"
						"time: %lld",time);
	}
	debugExtEnd();

	// PBTILOUT - statistics layout
	write(&respdata,(uint16_t)3);
	// PBTIID - estimated processing time
	write(&respdata,(uint16_t)7);
	// PBTILEN
	write(&respdata,(uint16_t)8);
	// data
	write(&respdata,(uint64_t)time);
}

void sqlrprotocol_teradata::appendEndEstimatedProcessingTimeExtension() {

	debugExtStart("end estimated processing time");
	debugExtEnd();

	// PBTILOUT - end-info
	write(&respdata,(uint16_t)4);
	// PBTIID - estimated processing time
	write(&respdata,(uint16_t)7);
	// PBTILEN
	write(&respdata,(uint16_t)0);
}

void sqlrprotocol_teradata::appendQueryExtension(uint16_t col) {

	debugExtStart((req->function=='E')?"query (limited)":"query (full)");
	
	// PBTILOUT - limited/full layout
	write(&respdata,(uint16_t)((req->function=='E')?2:1));
	// PBTIID - query
	write(&respdata,(uint16_t)2);
	// PBTILEN
	size_t	lengthpos=respdata.getPosition();
	write(&respdata,(uint16_t)0);

	// data...
	size_t		startpos=respdata.getPosition();

	// PBTIFDB
	// db name (FIXME: we don't know this)
	uint16_t	pbtifdblen=0;
	const char	*pbtifdb="";

	// PBTIFTB
	// table name
	uint16_t	pbtiftblen=cont->getColumnTableLength(req->cur,col);
	const char	*pbtiftb=cont->getColumnTable(req->cur,col);

	// PBTIFCN
	// column name
	uint16_t	pbtifcnlen=cont->getColumnNameLength(req->cur,col);
	const char	*pbtifcn=cont->getColumnName(req->cur,col);

	// PBTIFCP
	// position in table (0 if not in a table (or unknown?))
	uint16_t	pbtifcp=0;

	// PBTIFAN
	// alias (FIXME: we don't know this, as opposed to name)
	uint16_t	pbtifanlen=cont->getColumnNameLength(req->cur,col);
	const char	*pbtifan=cont->getColumnName(req->cur,col);

	// PBTIFT
	// column "title" (FIXME: we don't know this, as opposed to name)
	uint16_t	pbtiftlen=cont->getColumnNameLength(req->cur,col);
	const char	*pbtift=cont->getColumnName(req->cur,col);

	// PBTIFF
	// column format
	bytebuffer	fieldformat;
	getFieldFormat(&fieldformat,col);
	uint16_t	pbtifflen=fieldformat.getSize();
	const byte_t	*pbtiff=fieldformat.getBuffer();

	// PBTIFDV
	// default (FIXME: we don't know this)
	uint16_t	pbtifdvlen=0;
	const char	*pbtifdv="";

	// PBTIFIC
	// identity
	char	pbtific=(cont->getColumnIsAutoIncrement(req->cur,col))?'Y':'N';

	// PBTIFDW
	// writable based on user's permission
	char	pbtifdw='U';

	// PBTIFNL
	// nullable
	char	pbtifnl=(cont->getColumnIsNullable(req->cur,col))?'Y':'N';

	// PBTIFMN
	// nulls can be returned
	// (FIXME: we don't know this, as opposed to nullable)
	char	pbtifmn=(cont->getColumnIsNullable(req->cur,col))?'Y':'N';

	// PBTIFSR
	// permitted in where clause
	// (FIXME: there could be other types that aren't allowed)
	int16_t	type=cont->getColumnType(req->cur,col);
	char	pbtifsr=(cont->isBlobType(type))?'N':'Y';

	// PBTIFWR
	// writable (i.e. is not an expression)
	// (FIXME: we don't know this but can't return a U)
	char	pbtifwr='Y';

	// PBTIFDT
	// data type
	uint16_t	pbtifdt=getColumnType(col);

	// PBTIFUT
	// UDT type
	// 1 = structured
	// 2 = distinct
	// 3 = internal
	// 0 = ambiguous
	// (FIXME: we don't know this)
	uint16_t	pbtifut=0;

	// PBTIFTY
	// type name
	uint16_t	pbtiftylen=getColumnTypeNameLength(col);
	const char	*pbtifty=getColumnTypeName(col);

	// PBTIFMI
	// misc info... (FIXME: ???)
	uint16_t	pbtifmi=0;

	// PBTIFMDL
	// byte length
	uint64_t	pbtifmdl=cont->getColumnLength(req->cur,col);

	// PBTIFND
	// precision
	uint16_t	pbtifnd=cont->getColumnPrecision(req->cur,col);

	// PBTIFNID
	// interval digits (FIXME: we don't know this)
	uint16_t	pbtifnid=0;

	// PBTIFNFD
	// scale
	uint16_t	pbtifnfd=cont->getColumnScale(req->cur,col);

	// PBTIFCT
	// charset
	// 1 = Latin
	// 2 = Unicode
	// 3 = Jap Shift-JIS
	// 4 = Graphic
	// 5 = Kanji
	// 0 = not char data
	// (FIXME: we don't know this)
	byte_t	pbtifct=(cont->isBitType(type) ||
				cont->isBoolType(type) ||
				cont->isFloatType(type) ||
				cont->isNumberType(type) ||
				cont->isBlobType(type) ||
				cont->isUnsignedType(type) ||
				cont->isBinaryType(type) ||
				cont->isDateTimeType(type))?0:1;

	// PBTIFMNC
	// char length
	// (FIXME: we don't know this, as opposed to byte length)
	uint64_t	pbtifmnc=(cont->isBitType(type) ||
				cont->isBoolType(type) ||
				cont->isFloatType(type) ||
				cont->isNumberType(type) ||
				cont->isBlobType(type) ||
				cont->isUnsignedType(type) ||
				cont->isBinaryType(type) ||
				cont->isDateTimeType(type))?
				0:cont->getColumnLength(req->cur,col);

	// PBTIFCS
	// case sensitive (FIXME: we don't know this)
	char	pbtifcs='U';

	// PBTIFSN
	// signed
	char	pbtifsn=(cont->getColumnIsUnsigned(req->cur,col))?'N':'Y';

	// PBTIFK
	// uniquely describes the row
	// (FIXME: we don't know this, except for auto-increment columns)
	char	pbtifk=(cont->getColumnIsAutoIncrement(req->cur,col))?'Y':'U';

	// PBTIFU
	// unique
	char	pbtifu=(cont->getColumnIsUnique(req->cur,col))?'Y':'N';

	// PBTIFE
	// expression (FIXME: we don't know this)
	char	pbtife='U';

	// PBTIFSO
	// permitted in order-by (FIXME: we don't know this)
	char	pbtifso='U';

	// tweak various byte-lengths and precisions
	if (!charstring::compare(pbtifty,"DECIMAL") ||
			!charstring::compare(pbtifty,"NUMBER")) {
		if (pbtifnd<3) {
			pbtifmdl=1;
		} else if (pbtifnd<5) {
			pbtifmdl=2;
		} else if (pbtifnd<10) {
			pbtifmdl=4;
		} else {
			pbtifmdl=8;
		}
	} else if (!charstring::compare(pbtifty,"FLOAT")) {
		pbtifmdl=8;
	} else if (!charstring::compare(pbtifty,"DATE")) {
		pbtifmdl=4;
	} else if (!charstring::compare(pbtifty,"TIME")) {
		pbtifnd=15;
	} else if (!charstring::compare(pbtifty,"TIMESTAMP") ||
			!charstring::compare(pbtifty,"DATETIME")) {
		pbtifnd=26;
	}

	if (getDebug()) {
		stdoutput.printf("			"
				"col %d {\n",col);

		if (req->function!='E') {
			stdoutput.printf("				"
				"PBTIFDB - db name: %.s\n",
				pbtifdblen,pbtifdb);
			stdoutput.printf("				"
				"PBTIFTB - table name: %.*s\n",
				pbtiftblen,pbtiftb);
			stdoutput.printf("				"
				"PBTIFCN - column name: %.*s\n",
				pbtifcnlen,pbtifcn);
			stdoutput.printf("				"
				"PBTIFCP - position in table: %d\n",
				pbtifcp);
			stdoutput.printf("				"
				"PBTIFAN - alias: %.*s\n",
				pbtifanlen,pbtifan);
			stdoutput.printf("				"
				"PBTIFT - column title: %.*s\n",
				pbtiftlen,pbtift);
			stdoutput.printf("				"
				"PBTIFF - column format: %.*s\n",
				pbtifflen,pbtiff);
			stdoutput.printf("				"
				"PBTIFDV - default: %.*s\n",
				pbtifdvlen,pbtifdv);
			stdoutput.printf("				"
				"PBTIFIC - identity: %c\n",
				pbtific);
			stdoutput.printf("				"
				"PBTIFDW - writable (permission): %c\n",
				pbtifdw);
			stdoutput.printf("				"
				"PBTIFNL - nullable: %c\n",
				pbtifnl);
			stdoutput.printf("				"
				"PBTIFMN - nulls can be returned: %c\n",
				pbtifmn);
			stdoutput.printf("				"
				"PBTIFSR - "
				"permitted in where clause: %c\n",
				pbtifsr);
			stdoutput.printf("				"
				"PBTIFWR - writable: %c\n",
				pbtifwr);
		}

		stdoutput.printf("				"
				"PBTIFDT - data type: 0x%04x (%d)\n",
				pbtifdt,pbtifdt);

		if (req->function!='E') {
			stdoutput.printf("				"
				"PBTIFUT - UDT type: %d\n",
				pbtifut);
			stdoutput.printf("				"
				"PBTIFTY - type name: %.*s\n",
				pbtiftylen,pbtifty);
			stdoutput.printf("				"
				"PBTIFMI - misc info: %d\n",
				pbtifmi);
			stdoutput.printf("				"
				"PBTIFMDL - byte length: %lld\n",
				pbtifmdl);
		}

		stdoutput.printf("				"
				"PBTIFND - precision: %d\n",
				pbtifnd);
		stdoutput.printf("				"
				"PBTIFNID - interval digits: %d\n",
				pbtifnid);
		stdoutput.printf("				"
				"PBTIFNFD - scale: %d\n",
				pbtifnfd);

		if (req->function!='E') {
			stdoutput.printf("				"
				"PBTIFCT - charset: %d\n",
				pbtifct);
			stdoutput.printf("				"
				"PBTIFMNC - char length: %lld\n",
				pbtifmnc);
			stdoutput.printf("				"
				"PBTIFCS - case sensitive: %c\n",
				pbtifcs);
			stdoutput.printf("				"
				"PBTIFSN - signed: %c\n",
				pbtifsn);
			stdoutput.printf("				"
				"PBTIFK - uniquely describes the row: %c\n",
				pbtifk);
			stdoutput.printf("				"
				"PBTIFU - unique: %c\n",
				pbtifu);
			stdoutput.printf("				"
				"PBTIFE - expression: %c\n",
				pbtife);
			stdoutput.printf("				"
				"PBTIFSO - permitted in order-by: %c\n",
				pbtifso);
		}

		stdoutput.printf("			}\n");
	}
	debugExtEnd();

	if (req->function!='E') {
		write(&respdata,pbtifdblen);
		write(&respdata,pbtifdb,pbtifdblen);
		write(&respdata,pbtiftblen);
		write(&respdata,pbtiftb,pbtiftblen);
		write(&respdata,pbtifcnlen);
		write(&respdata,pbtifcn,pbtifcnlen);
		write(&respdata,pbtifcp);
		write(&respdata,pbtifanlen);
		write(&respdata,pbtifan,pbtifanlen);
		write(&respdata,pbtiftlen);
		write(&respdata,pbtift,pbtiftlen);
		write(&respdata,pbtifflen);
		write(&respdata,pbtiff,pbtifflen);
		write(&respdata,pbtifdvlen);
		write(&respdata,pbtifdv,pbtifdvlen);
		write(&respdata,pbtific);
		write(&respdata,pbtifdw);
		write(&respdata,pbtifnl);
		write(&respdata,pbtifmn);
		write(&respdata,pbtifsr);
		write(&respdata,pbtifwr);
	}
	write(&respdata,pbtifdt);
	if (req->function!='E') {
		write(&respdata,pbtifut);
		write(&respdata,pbtiftylen);
		write(&respdata,pbtifty,pbtiftylen);
		write(&respdata,pbtifmi);
	}
	write(&respdata,pbtifmdl);
	write(&respdata,pbtifnd);
	write(&respdata,pbtifnid);
	write(&respdata,pbtifnfd);
	if (req->function!='E') {
		write(&respdata,pbtifct);
		write(&respdata,pbtifmnc);
		write(&respdata,pbtifcs);
		write(&respdata,pbtifsn);
		write(&respdata,pbtifk);
		write(&respdata,pbtifu);
		write(&respdata,pbtife);
		write(&respdata,pbtifso);

		// ??? (not described in spec)
		byte_t	unknown[]={
			0x55,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		write(&respdata,unknown,sizeof(unknown));
	}

	// backpatch length
	size_t	endpos=respdata.getPosition();
	respdata.setPositionRelativeToBeginning(lengthpos);
	respdata.write(hostTo((uint16_t)(endpos-startpos)));
	respdata.setPositionRelativeToBeginning(endpos);
}

void sqlrprotocol_teradata::appendEndQueryExtension() {

	debugExtStart("end query");
	debugExtEnd();
	
	// PBTILOUT - end-info
	write(&respdata,(uint16_t)4);
	// PBTIID - query
	write(&respdata,(uint16_t)2);
	// PBTILEN
	write(&respdata,(uint16_t)0);
}

void sqlrprotocol_teradata::appendStatementInfoEndParcel() {
	appendParcelHeader(170,0);
	debugParcelStart("send","statementinformationend",170);
	debugParcelEnd();
}

void sqlrprotocol_teradata::appendRowParcels(bool *eors) {

	// get column count
	uint16_t	colcount=cont->colCount(req->cur);

	// reset activity count (FIXME: see note below)
	req->activitycount=0;

	// fetch rows (unless we need to resend this row)
	bool	error;
	while (req->resendrow || cont->fetchRow(req->cur,&error)) {

		// reset resend-row flag
		req->resendrow=false;

		// We need to make sure we don't try to send too much data at
		// once.  So, keep track of where we are in the send-data
		// buffer.  If we write too much data to it, we'll truncate it
		// here and resend the row when we get a continue request.
		size_t	pos=respdata.getPosition();

		// append fields
		for (uint16_t i=0; i<colcount; i++) {

			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			if (!cont->getField(req->cur,i,
						&field,&fieldlength,
						&blob,&null)) {
				// FIXME: handle error
			}

			// bail if appending this field would be too much
			if (respdata.getSize()+fieldlength>maxmessagesize) {
				respdata.truncate(pos);
				req->resendrow=true;
				*eors=false;
				return;
			}

			appendField(i,field,fieldlength,null);
		}

		// FIXME: Somehow we have to know the total row count and send
		// it back in the 205 for the first group of rows.  Just sending
		// back the count for this group isn't right, and causes the
		// client to send a report a warning.
		req->activitycount++;

		// FIXME: kludgy
		cont->nextRow(req->cur);
	}

	if (error) {
		// FIXME: handle error
	}

	// if we got here then we fetched all rows,
	// and are at the end of the result set
	*eors=true;
}

void sqlrprotocol_teradata::backpatchActivityCount() {
	size_t	originalpos=respdata.getPosition();
	respdata.setPositionRelativeToBeginning(req->activitycountpos);
	if (req->activitycountsize==4) {
		respdata.write(hostTo((uint32_t)req->activitycount));
	} else {
		respdata.write(hostTo(req->activitycount));
	}
	respdata.setPositionRelativeToBeginning(originalpos);
}

void sqlrprotocol_teradata::appendTitleStartParcel() {
	debugParcelStart("send","title start",20);
	debugParcelEnd();
	appendParcelHeader(20,0);
}

void sqlrprotocol_teradata::appendTitleEndParcel() {
	debugParcelStart("send","title end",21);
	debugParcelEnd();
	appendParcelHeader(21,0);
}

void sqlrprotocol_teradata::appendFormatStartParcel() {
	debugParcelStart("send","format start",22);
	debugParcelEnd();
	appendParcelHeader(22,0);
}

void sqlrprotocol_teradata::appendFormatEndParcel() {
	debugParcelStart("send","format end",23);
	debugParcelEnd();
	appendParcelHeader(23,0);
}

void sqlrprotocol_teradata::appendSizeStartParcel() {
	debugParcelStart("send","size start",24);
	debugParcelEnd();
	appendParcelHeader(24,0);
}

void sqlrprotocol_teradata::appendSizeEndParcel() {
	debugParcelStart("send","size end",25);
	debugParcelEnd();
	appendParcelHeader(25,0);
}

void sqlrprotocol_teradata::appendSizeParcel(uint16_t size) {
	debugParcelStart("send","size",26);
	if (getDebug()) {
		stdoutput.printf("		size: %d\n",size);
	}
	debugParcelEnd();
	appendParcelHeader(26,2);
	write(&respdata,size);
}

void sqlrprotocol_teradata::appendRecStartParcel() {
	debugParcelStart("send","rec start",27);
	debugParcelEnd();
	appendParcelHeader(27,0);
}

void sqlrprotocol_teradata::appendRecEndParcel() {
	debugParcelStart("send","rec end",28);
	debugParcelEnd();
	appendParcelHeader(28,0);
}

void sqlrprotocol_teradata::appendField(uint16_t col, 
						const char *field,
						uint64_t fieldlength,
						bool null) {
	if (!col) {
		if (req->requestmode=='F') {
			appendRecStartParcel();
		} else {
			req->currentfield=0;
			req->nibuffer.clear();
			req->rowbuffer.clear();
		}
	}

	switch (req->requestmode) {
		case 'F':
			appendFieldParcel(field,fieldlength);
			break;
		case 'R':
			appendRecordModeField(col,field,fieldlength,false);
			break;
		case 'I':
		case 'M':
			appendIndicatorModeField(col,field,fieldlength,null);
			break;
	}

	if (col==cont->colCount(req->cur)-1) {
		if (req->requestmode=='F') {
			appendRecEndParcel();
		} else {
			appendRecordParcel();
		}
	}
}

void sqlrprotocol_teradata::appendRecordModeField(uint16_t col, 
							const char *field,
							uint64_t fieldlength,
							bool null) {

	// get column type
	const char	*type=getColumnTypeName(col);

	// FIXME: various calls below assume that field is null-terminated,
	// which may not be true for all backends

	if (!charstring::compare(type,"TINYINT")) {

		if (null) {
			write(&req->rowbuffer,(byte_t)0);
		} else {
			uint16_t	val=charstring::convertToInteger(field);
			write(&req->rowbuffer,(byte_t)val);
		}

	} else if (!charstring::compare(type,"SMALLINT")) {

		if (null) {
			write(&req->rowbuffer,(uint16_t)0);
		} else {
			uint16_t	val=charstring::convertToInteger(field);
			write(&req->rowbuffer,val);
		}

	} else if (!charstring::compare(type,"INTEGER")) {

		if (null) {
			write(&req->rowbuffer,(uint32_t)0);
		} else {
			uint32_t	val=charstring::convertToInteger(field);
			write(&req->rowbuffer,val);
		}

	} else if (!charstring::compare(type,"BIGINT")) {

		if (null) {
			write(&req->rowbuffer,(uint64_t)0);
		} else {
			uint64_t	val=charstring::convertToInteger(field);
			write(&req->rowbuffer,val);
		}

	} else if (!charstring::compare(type,"CHAR") ||
			!charstring::compare(type,"BYTE")) {

		if (null) {
			uint32_t	len=cont->getColumnLength(req->cur,col);
			for (uint32_t i=0; i<len; i++) {
				write(&req->rowbuffer,' ');
			}
		} else {
			write(&req->rowbuffer,field,fieldlength);
		}

	} else if (!charstring::compare(type,"VARCHAR") ||
			!charstring::compare(type,"VARBYTE")) {

		if (null) {
			write(&req->rowbuffer,(uint16_t)0);
		} else {
			write(&req->rowbuffer,(uint16_t)fieldlength);
			write(&req->rowbuffer,field,fieldlength);
		}

	} else if (!charstring::compare(type,"DECIMAL") ||
			!charstring::compare(type,"NUMBER")) {

		if (null) {
			uint16_t	prec=cont->getColumnPrecision(
								req->cur,col);
			if (prec<3) {
				write(&req->rowbuffer,(byte_t)0);
			} else if (prec<5) {
				write(&req->rowbuffer,(uint16_t)0);
			} else if (prec<10) {
				write(&req->rowbuffer,(uint32_t)0);
			} else {
				write(&req->rowbuffer,(uint64_t)0);
			}
		} else {
			char	*temp=charstring::duplicate(field,fieldlength);
			charstring::bothTrim(temp);
			charstring::strip(temp,'.');
			int64_t		val=charstring::convertToInteger(temp);
			delete[] temp;
			uint16_t	prec=cont->getColumnPrecision(
								req->cur,col);
			if (prec<3) {
				write(&req->rowbuffer,(byte_t)val);
			} else if (prec<5) {
				write(&req->rowbuffer,(uint16_t)val);
			} else if (prec<10) {
				write(&req->rowbuffer,(uint32_t)val);
			} else {
				write(&req->rowbuffer,(uint64_t)val);
			}
		}

	} else if (!charstring::compare(type,"FLOAT")) {

		if (null) {
			write(&req->rowbuffer,(uint64_t)0);
		} else {
			double		val=charstring::convertToFloat(field);
			uint64_t	bytes;
			bytestring::copy(&bytes,&val,sizeof(bytes));
			write(&req->rowbuffer,bytes);
		}

	} else if (!charstring::compare(type,"DATE")) {

		if (null) {
			write(&req->rowbuffer,(uint32_t)0);
		} else {
			int16_t	year=0;
			int16_t	month=0;
			int16_t	day=0;
			int16_t	hour=0;
			int16_t	minute=0;
			int16_t	second=0;
			int32_t	fraction=0;
			bool	isnegative=false;
			if (datetime::parse(field,
						false,
						false,
						"-",
						&year,
						&month,
						&day,
						&hour,
						&minute,
						&second,
						&fraction,
						&isnegative)) {
				// 32-bit signed two’s complement integer, most
				// significant byte first (4 bytes); DATE is
				// calculated as follows:
				// (year-1900)*10000 + month*100 + day 
				uint32_t	date=
					(year-1900)*10000+month*100+day;
				write(&req->rowbuffer,date);
			} else {
				write(&req->rowbuffer,(uint32_t)0);
			}
		}

	} else if (!charstring::compare(type,"TIME")) {

		uint16_t	scale=cont->getColumnScale(req->cur,col);
		if (null) {
			write(&req->rowbuffer,"         ");
			if (scale) {
				req->rowbuffer.printf("%*s",scale,"");
			}
		} else {
			// apparently returned as text in this format:
			// hh:mm:ss.ffffff
			int16_t	year=0;
			int16_t	month=0;
			int16_t	day=0;
			int16_t	hour=0;
			int16_t	minute=0;
			int16_t	second=0;
			int32_t	fraction=0;
			bool	isnegative=false;
			if (datetime::parse(field,
						false,
						false,
						"-",
						&year,
						&month,
						&day,
						&hour,
						&minute,
						&second,
						&fraction,
						&isnegative)) {
				req->rowbuffer.printf(
						"%02d:%02d:%02d",
						hour,minute,second);
				if (scale) {
					req->rowbuffer.printf(".%0*d",
								scale,fraction);
				}
			} else {
				write(&req->rowbuffer,"         ");
				if (scale) {
					req->rowbuffer.printf("%*s",scale,"");
				}
			}
		}

	} else if (!charstring::compare(type,"TIMESTAMP") ||
			!charstring::compare(type,"DATETIME")) {

		uint16_t	scale=cont->getColumnScale(req->cur,col);
		if (null) {
			write(&req->rowbuffer,"                   ");
			if (scale) {
				req->rowbuffer.printf("%*s",scale,"");
			}
		} else {
			// apparently returned as text in this format:
			// yyyy-mm-dd hh:mm:ss.ffffff
			int16_t	year=0;
			int16_t	month=0;
			int16_t	day=0;
			int16_t	hour=0;
			int16_t	minute=0;
			int16_t	second=0;
			int32_t	fraction=0;
			bool	isnegative=false;
			if (datetime::parse(field,
						false,
						false,
						"-",
						&year,
						&month,
						&day,
						&hour,
						&minute,
						&second,
						&fraction,
						&isnegative)) {
				if (year<0) {
					year=0;
				}
				if (month<0) {
					month=0;
				}
				if (day<0) {
					day=0;
				}
				if (hour<0) {
					hour=0;
				}
				if (minute<0) {
					minute=0;
				}
				if (second<0) {
					second=0;
				}
				if (fraction<0) {
					fraction=0;
				}
				req->rowbuffer.printf(
					"%04d-%02d-%02d %02d:%02d:%02d",
					year,month,day,
					hour,minute,second);
				if (scale) {
					req->rowbuffer.printf(".%0*d",
								scale,fraction);
				}
			} else {
				write(&req->rowbuffer,"                   ");
				if (scale) {
					req->rowbuffer.printf("%*s",scale,"");
				}
			}
		}
	}

	req->currentfield++;
}

void sqlrprotocol_teradata::appendIndicatorModeField(uint16_t col, 
							const char *field,
							uint64_t fieldlength,
							bool null) {

	// append to nibuffer
	byte_t	ni=(req->currentfield%8)?req->nibuffer[req->currentfield/8]:0;

	// make sure to cast the right side of the << to a byte
	// or some compilers (native CC on Unixware 7.0.1) will fail with:
	// internal compiler error: ... Runs out of registers
	ni|=((byte_t)null)<<((byte_t)(7-(req->currentfield%8)));

	req->nibuffer[req->currentfield/8]=ni;

	// append to rowbuffer (also increments currentfield)
	appendRecordModeField(col,field,fieldlength,null);
}

void sqlrprotocol_teradata::appendRecordParcel() {

	// FIXME: There's something that's supposed to be different about
	// multipart records...  Like, if the row is bigger than some
	// particular size, then the data needs to be split across multiple
	// records, or something.

	// send a (multipart)record parcel...

	uint16_t	parcelflavor=(req->requestmode=='M')?144:10;
	const char	*parcelname=(req->requestmode=='M')?
					"multipartrecord":"record";

	debugParcelStart("send",parcelname,parcelflavor);
	if (getDebug()) {
		if (req->requestmode=='I' || req->requestmode=='M') {
			stdoutput.printf("		null indicator:\n");
			for (uint16_t i=0; i<req->nibuffer.getCount(); i++) {
				stdoutput.printf("			");
				stdoutput.printBits(req->nibuffer[i]);
				stdoutput.printf(" (%02x)\n",req->nibuffer[i]);
			}
			stdoutput.write("\n");
		}
		stdoutput.printf("		row buffer:\n");
		debugHexDump(req->rowbuffer.getBuffer(),
				req->rowbuffer.getSize(),1);
	}
	debugParcelEnd();

	appendParcelHeader(parcelflavor,req->nibuffer.getCount()+
						req->rowbuffer.getSize());

	// null indicator
	if (req->requestmode=='I' || req->requestmode=='M') {
		for (uint16_t i=0; i<req->nibuffer.getCount(); i++) {
			write(&respdata,req->nibuffer[i]);
		}
	}

	// data
	write(&respdata,req->rowbuffer.getBuffer(),req->rowbuffer.getSize());

	// send an endmultipartrecord parcel...
	if (req->requestmode=='M') {
		debugParcelStart("send","endmultipartrecord",145);
		debugParcelEnd();
		appendParcelHeader(145,0);
	}
}

void sqlrprotocol_teradata::appendFailureParcel(const char *errorstring,
						uint16_t errorlength) {

	debugParcelStart("send","failure",9);
	if (getDebug()) {
		stdoutput.printf("		error: %.*s\n",
						errorlength,errorstring);
	}
	debugParcelEnd();

	// failure parcel
	appendParcelHeader(9,2+2+2+2+errorlength+2+3);

	// statement number (FIXME: see note in appendEndStatementParcel)
	write(&respdata,(uint16_t)1);

	// info ???
	write(&respdata,(uint16_t)0);

	// code (FIXME: ???)
	byte_t code[]={
		0xdf, 0x0e
	};
	write(&respdata,code,sizeof(code));

	// length
	write(&respdata,errorlength);

	// msg
	write(&respdata,errorstring);

	// ins count ???
	write(&respdata,(uint16_t)1);

	// ins traid ???
	byte_t instriad[]={
		0x00, 0x08, 0x0a
	};
	write(&respdata,instriad,sizeof(instriad));
}

void sqlrprotocol_teradata::appendErrorParcel(const char *errorstring) {

	uint16_t	errorlength=charstring::getLength(errorstring);

	debugParcelStart("send","error",49);
	if (getDebug()) {
		stdoutput.printf("		error: %.*s\n",
						errorlength,errorstring);
	}
	debugParcelEnd();

	// error parcel...
	appendParcelHeader(49,6+errorlength+2);

	// statement number (FIXME: see note in appendEndStatementParcel)
	write(&respdata,(uint16_t)1);

	// info ???
	write(&respdata,(uint16_t)0);

	// code (FIXME: ???)
	byte_t code[]={
		0xa3, 0x0e
	};
	write(&respdata,code,sizeof(code));

	// length
	write(&respdata,errorlength);

	// msg
	write(&respdata,errorstring);

	// ??? (not described in spec)
	byte_t	unknown[]={
		0x00, 0x00
	};
	write(&respdata,unknown,sizeof(unknown));
}

void sqlrprotocol_teradata::appendEndStatementParcel() {

	// FIXME: in a multi-statement request, statements are numbered 1-n,
	// we currently only support 1 statement per request so we're always
	// sending 1, but we ought to support multiple statements
	// someday...
	appendEndStatementParcel(1);
}

void sqlrprotocol_teradata::appendEndStatementParcel(uint16_t statementnumber) {

	debugParcelStart("send","end statement",11);
	if (getDebug()) {
		stdoutput.printf("		statement number: %d\n",
							statementnumber);
	}
	debugParcelEnd();

	appendParcelHeader(11,2);

	write(&respdata,statementnumber);
}

void sqlrprotocol_teradata::appendEndRequestParcel() {
	debugParcelStart("send","end request",12);
	debugParcelEnd();
	appendParcelHeader(12,0);
}

void sqlrprotocol_teradata::appendCursorErrorParcel() {
	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(req->cur,&errorstring,
					&errorlength,
					&errnum,
					&liveconnection);
	respdata.clear();
	appendFailureParcel(errorstring,errorlength);
}

void sqlrprotocol_teradata::appendConnectionErrorParcel() {
	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(&errorstring,
				&errorlength,
				&errnum,
				&liveconnection);
	respdata.clear();
	appendFailureParcel(errorstring,errorlength);
}

void sqlrprotocol_teradata::unexpectedParcel(uint16_t parcelflavor) {
	if (getDebug()) {
		stdoutput.printf("	recv unexpected parcel: %d\n",
							parcelflavor);
	}
}

uint16_t sqlrprotocol_teradata::getActivity() {

	// skip whitespace and comments
	req->query=cont->skipWhitespaceAndComments(req->query);

	// FIXME: this is slow, use a dictionary (or something)...
	if (!charstring::compareIgnoringCase(req->query,"sel ",4) ||
		!charstring::compareIgnoringCase(req->query,"select ",7)) {
		return SQL_SELECT;
	} else if (!charstring::compareIgnoringCase(req->query,
						"create table ",13)) {
		return SQL_CREATE_TABLE;
	} else if (!charstring::compareIgnoringCase(req->query,
						"drop table ",11)) {
		return SQL_DROP_TABLE;
	} else if (!charstring::compareIgnoringCase(req->query,
						"insert ",7)) {
		return SQL_INSERT;
	} else if (!charstring::compareIgnoringCase(req->query,
						"update ",7)) {
		return SQL_UPDATE;
	} else if (!charstring::compareIgnoringCase(req->query,
						"delete ",7)) {
		return SQL_DELETE;
	} else if (!charstring::compareIgnoringCase(req->query,
						"database ",9)) {
		return SQL_DATABASE;
	} else if (!charstring::compareIgnoringCase(req->query,"bt",2) ||
			!charstring::compareIgnoringCase(req->query,
						"begin transaction",17)) {
		return SQL_BEGIN_TRANSACTION;
	} else if (!charstring::compareIgnoringCase(req->query,"et",2) ||
			!charstring::compareIgnoringCase(req->query,
						"end transaction",15)) {
		return SQL_END_TRANSACTION;
	} else if (!charstring::compareIgnoringCase(req->query,
						"commit",6)) {
		return ANSI_SQL_COMMIT_WORK;
	} else if (!charstring::compareIgnoringCase(req->query,
						"rollback",8)) {
		return SQL_ROLLBACK;
	} else if (!charstring::compareIgnoringCase(req->query,
						"help ",5)) {
		return SQL_HELP;
	} else if (!charstring::compareIgnoringCase(req->query,
						"begin loading ",14)) {
		return BEGIN_LOADING;
	} else if (!charstring::compareIgnoringCase(req->query,
						"checkpoint loading ",19)) {
		return CHECK_POINT_LOAD;
	} else if (!charstring::compareIgnoringCase(req->query,
						"end loading",11)) {
		return END_LOADING;
	} else if (!charstring::compareIgnoringCase(req->query,
						"set query_band ",15)) {
		return SET_QUERY_BAND_STMT;
	} else if (!charstring::compareIgnoringCase(req->query,
						"check workload for ",19)) {
		return CHECK_WORKLOAD_FOR;
	} else if (!charstring::compareIgnoringCase(req->query,
						"check workload end",18)) {
		return CHECK_WORKLOAD_END;
	}
	// FIXME: support more activities
	return 0;
}

bool sqlrprotocol_teradata::activityReturnsResults() {
	return (req->activity==SQL_SELECT ||
		req->activity==SQL_HELP ||
		req->activity==CHECK_WORKLOAD_END ||
		req->fudgeselect);
}

const char *sqlrprotocol_teradata::getColumnTypeName(uint16_t col) {

	const char	*type=cont->getColumnTypeName(req->cur,col);

	// Some backends (ODBC) return DATETIME or TIMESTAMP for DATE,
	// TIME, and TIMESTAMP types.  They are distinguishable by
	// column length though.
	if (!charstring::compare(type,"TIMESTAMP") ||
			!charstring::compare(type,"DATETIME")) {
		uint32_t	len=cont->getColumnLength(req->cur,col);
		if (len==10) {
			type="DATE";
		} else if (len==15) {
			type="TIME";
		}
	}
	return type;
}

uint16_t sqlrprotocol_teradata::getColumnTypeNameLength(uint16_t col) {

	uint16_t	typelen=cont->getColumnTypeNameLength(req->cur,col);

	// Some backends (ODBC) return DATETIME or TIMESTAMP for DATE,
	// TIME, and TIMESTAMP types.  They are distinguishable by
	// column length though.
	const char	*type=cont->getColumnTypeName(req->cur,col);
	if (!charstring::compare(type,"TIMESTAMP") ||
			!charstring::compare(type,"DATETIME")) {
		uint32_t	len=cont->getColumnLength(req->cur,col);
		if (len==10 || len==15) {
			typelen=4;
		}
	}
	return typelen;
}

uint16_t sqlrprotocol_teradata::getColumnType(uint16_t col) {

	const char	*type=getColumnTypeName(col);
	uint16_t	retval=0;

	// FIXME: this is slow, use a dictionary (or something)...
	if (!charstring::compare(type,"BLOB")) {
		retval=400;
	}
	// FIXME: ???
	/*if (!charstring::compare(type,"BLOB AS DEFERRED")) {
		retval=404;
	}
	if (!charstring::compare(type,"BLOB AS LOCATOR")) {
		retval=408;
	}
	if (!charstring::compare(type,"BLOB AS DEFERRED BY NAME")) {
		retval=412;
	}*/
	if (!charstring::compare(type,"CLOB")) {
		retval=416;
	}
	// FIXME: ???
	/*if (!charstring::compare(type,"CLOB AS DEFERRED")) {
		retval=420;
	}
	if (!charstring::compare(type,"CLOB AS LOCATOR")) {
		retval=424;
	}*/
	if (!charstring::compare(type,"UDT")) {
		retval=432;
	}
	// FIXME: ???
	/*if (!charstring::compare(type,"Distinct UDT")) {
		retval=436;
	}
	if (!charstring::compare(type,"Structure UDT")) {
		retval=440;
	}*/
	if (!charstring::compare(type,"VARCHAR")) {
		retval=448;
	}
	if (!charstring::compare(type,"CHAR")) {
		retval=452;
	}
	if (!charstring::compare(type,"LONGVARCHAR")) {
		retval=456;
	}
	if (!charstring::compare(type,"VARGRAPHIC")) {
		retval=464;
	}
	if (!charstring::compare(type,"GRAPHIC")) {
		retval=468;
	}
	if (!charstring::compare(type,"LONGVARGRAPHIC")) {
		retval=472;
	}
	if (!charstring::compare(type,"FLOAT")) {
		retval=480;
	}
	if (!charstring::compare(type,"DECIMAL")) {
		retval=484;
	}
	if (!charstring::compare(type,"INTEGER")) {
		retval=496;
	}
	if (!charstring::compare(type,"SMALLINT")) {
		retval=500;
	}
	if (!charstring::compare(type,"ARRAY")) {
		// one-dimension
		retval=504;
	}
	// FIXME: ???
	/*if (!charstring::compare(type,"ARRAY - multiple dimensions")) {
		// multi-dimension
		retval=508;
	}*/
	if (!charstring::compare(type,"BIGINT")) {
		retval=600;
	}
	if (!charstring::compare(type,"NUMBER")) {
		retval=604;
	}
	if (!charstring::compare(type,"VARBYTE")) {
		retval=688;
	}
	if (!charstring::compare(type,"BYTE")) {
		retval=692;
	}
	if (!charstring::compare(type,"LONGVARBYTE")) {
		retval=696;
	}
	if (!charstring::compare(type,"DATE")) {
		// ANSI format (YYYY-MM-DD string)
		//retval=748;
		// Teradata format (4-byte number)
		retval=752;
	}
	if (!charstring::compare(type,"BYTEINT") ||
		!charstring::compare(type,"TINYINT")) {
		retval=756;
	}
	if (!charstring::compare(type,"TIME")) {
		retval=760;
	}
	if (!charstring::compare(type,"TIMESTAMP") ||
		!charstring::compare(type,"DATETIME")) {
		retval=764;
	}
	// FIXME: ???
	/*if (!charstring::compare(type,"TIME WITH TIME ZONE")) {
		retval=768;
	}
	if (!charstring::compare(type,"TIMESTAMP WITH TIME ZONE")) {
		retval=772;
	}
	if (!charstring::compare(type,"INTERVAL YEAR")) {
		retval=776;
	}
	if (!charstring::compare(type,"INTERVAL YEAR TO MONTH")) {
		retval=780;
	}
	if (!charstring::compare(type,"INTERVAL MONTH")) {
		retval=784;
	}
	if (!charstring::compare(type,"INTERVAL DAY")) {
		retval=788;
	}
	if (!charstring::compare(type,"INTERVAL DAY TO HOUR")) {
		retval=792;
	}
	if (!charstring::compare(type,"INTERVAL DAY TO MINUTE")) {
		retval=796;
	}
	if (!charstring::compare(type,"INTERVAL DAY TO SECOND")) {
		retval=800;
	}
	if (!charstring::compare(type,"INTERVAL HOUR")) {
		retval=804;
	}
	if (!charstring::compare(type,"INTERVAL HOUR TO MINUTE")) {
		retval=808;
	}
	if (!charstring::compare(type,"INTERVAL HOUR TO SECOND")) {
		retval=812;
	}
	if (!charstring::compare(type,"INTERVAL MINUTE")) {
		retval=816;
	}
	if (!charstring::compare(type,"INTERVAL MINUTE TO SECOND")) {
		retval=820;
	}
	if (!charstring::compare(type,"INTERVAL SECOND")) {
		retval=824;
	}
	if (!charstring::compare(type,"PERIOD (DATE)")) {
		retval=832;
	}
	if (!charstring::compare(type,"PERIOD (TIME)")) {
		retval=836;
	}
	if (!charstring::compare(type,"PERIOD (TIME WITH TIME ZONE)")) {
		retval=840;
	}
	if (!charstring::compare(type,"PERIOD (TIMESTAMP)")) {
		retval=844;
	}
	if (!charstring::compare(type,"PERIOD (TIMESTAMP WITH TIME ZONE)")) {
		retval=848;
	}*/
	if (!charstring::compare(type,"XML")) {
		retval=852;
	}
	// FIXME: ???
	/*if (!charstring::compare(type,"XML Text Deferred")) {
		retval=856;
	}
	if (!charstring::compare(type,"XML Text Locator")) {
		retval=860;
	}*/
	// FIXME: SQL Relay supports more types than this, and we need to map
	// all of them to Teradata types...

	// add 1 of the column is nullable
	if (retval && cont->getColumnIsNullable(req->cur,col)) {
		retval++;
	}
	return retval;
}

void sqlrprotocol_teradata::debugParcelStart(const char *direction,
						const char *flavorname,
						uint16_t parcelflavor,
						uint32_t parceldatalength) {
	if (getDebug()) {
		stdoutput.printf("	%s %s parcel - %d (%d) {\n",
			direction,flavorname,parcelflavor,parceldatalength);
	}
}

void sqlrprotocol_teradata::debugParcelEnd(const byte_t *parceldata,
						uint32_t parceldatalength) {
	if (getDebug()) {
		debugHexDump(parceldata,parceldatalength);
		stdoutput.write("	}\n");
	}
}

void sqlrprotocol_teradata::debugParcelStart(const char *direction,
						const char *flavorname,
						uint16_t parcelflavor) {
	if (getDebug()) {
		stdoutput.printf("	%s %s parcel - %d {\n",
				direction,flavorname,parcelflavor);
	}
}

void sqlrprotocol_teradata::debugParcelEnd() {
	if (getDebug()) {
		stdoutput.write("	}\n");
	}
}

void sqlrprotocol_teradata::debugExtStart(const char *extname) {
	if (getDebug()) {
		stdoutput.printf("		%s ext {\n",extname);
	}
}

void sqlrprotocol_teradata::debugExtEnd() {
	if (getDebug()) {
		stdoutput.write("		}\n");
	}
}

#ifdef DECRYPT
bool sqlrprotocol_teradata::decrypt(const byte_t *encdata,
						uint64_t encdatasize,
						bytebuffer *decdata) {

	// FIXME: push down to rudiments
	if (getDebug()) {
		stdoutput.printf("decrypting...\n");
	}

	// validate the encdata
	if (encdatasize<16) {
		if (getDebug()) {
			stdoutput.printf("encdata too small\n");
		}
		return false;
	}

	// initialize cipher context
	EVP_CIPHER_CTX	*ctx=EVP_CIPHER_CTX_new();
	if (!ctx) {
		if (getDebug()) {
			stdoutput.printf("init cipher context failed\n");
			ERR_print_errors_fp(stdout);
		}
		return false;
	}

	// chose a cipher (libcrypto enables PKCS5 padding by default)
	const EVP_CIPHER	*cipher=NULL;
	switch (negotiatedqop) {
		case QOP_AES_K128_GCM_PKCS5Padding_SHA2:
			cipher=EVP_aes_128_gcm();
			break;
		case QOP_AES_K128_CBC_PKCS5Padding_SHA1:
			cipher=EVP_aes_128_cbc();
			break;
		case QOP_AES_K192_GCM_PKCS5Padding_SHA2:
			cipher=EVP_aes_192_gcm();
			break;
		case QOP_AES_K192_CBC_PKCS5Padding_SHA1:
			cipher=EVP_aes_192_cbc();
			break;
		case QOP_AES_K256_GCM_PKCS5Padding_SHA2:
			cipher=EVP_aes_256_gcm();
			break;
		case QOP_AES_K256_CBC_PKCS5Padding_SHA1:
			cipher=EVP_aes_256_cbc();
			break;
	}

	// get the key
	// * not sure how to generate it
	// * presumably generated from the shared secret
	// * needs to be 16/24/32 bytes for AES128/192/256
	//  * try...
	//   * left x bits of sha256 hash
	//   * pbkdf2
	const byte_t	*key=sha2sharedsecret;
	uint32_t	keylength=sizeof(sha2sharedsecret);
	
	// get the initialization vector
	//
	// we need between 12 and 16 bytes for this, depending on the mode
	//
	// In this particular packet, request auth, request no, gateway byte,
	// and host charset are very different than in other packets (and the
	// request no, gateway byte, and host charset are nonsensical).
	// Together, these add up to 14 bytes, so it seems likely that they
	// combine (somehow) to form the iv.  We still need 2 more bytes though.
	// Maybe the non-zero bytes of the session number?
	//
	// Even if these are correct, who knows what order they go in...
// FIXME: hey... apparently it's semi-conventional to generate a random IV and
// send it to the other side as the first 16 bytes of the data
	byte_t		iv[16];
	byte_t		*ptr=iv;
	uint16_t	temp16=hostToBE((uint16_t)sessionno);
	bytestring::copy(ptr,&temp16,sizeof(temp16));
	ptr+=sizeof(temp16);
	bytestring::copy(ptr,requestauth,sizeof(requestauth));
	ptr+=sizeof(requestauth);
	uint32_t	temp32=hostToBE(requestno);
	bytestring::copy(ptr,&temp32,sizeof(temp32));
	ptr+=sizeof(temp32);
	bytestring::copy(ptr,&gtwbyte,sizeof(gtwbyte));
	ptr+=sizeof(gtwbyte);
	bytestring::copy(ptr,&hostcharset,sizeof(hostcharset));
	ptr+=sizeof(hostcharset);
	// FIXME: these probably shouldn't be 0
	/**ptr=0;
	ptr++;
	*ptr=0;*/

	// FIXME: SHA1/256 is somehow involved here, maybe as an hmac?
	
	// initialize the decryption process
	if (!EVP_DecryptInit_ex(ctx,cipher,NULL,key,iv)) {
		if (getDebug()) {
			stdoutput.printf("decrypt-init failed\n");
			ERR_print_errors_fp(stdout);
		}
		return false;
	}

stdoutput.printf("iv:\n");
debugHexDump(iv,sizeof(iv));
stdoutput.printf("block size: %d\n",EVP_CIPHER_CTX_block_size(ctx));
stdoutput.printf("cipher key length: %d\n",EVP_CIPHER_CTX_key_length(ctx));
stdoutput.printf("provided key length: %d\n",keylength);
stdoutput.printf("cipher iv length: %d\n",EVP_CIPHER_CTX_iv_length(ctx));
stdoutput.printf("enc data size: %d\n",encdatasize);

	// begin decrypting
	// (This assumes that the decrypted data will fit in "out", which is
	// probably true in this appliation, but not in general.
	// It's not immediately clear how to detect if the decrypted data won't
	// fit in "out", or what to do in that case.
	// I imagine maybe EVP_DecryptUpdate will fail, and maybe outsize will
	// contain the necessary size, but that's just a guess.)
	byte_t		out[1024];
	int		outsize=0;
	int		totaloutsize=0;
	bool		success=true;
	if (!EVP_DecryptUpdate(ctx,out,&outsize,encdata,encdatasize)) {
		if (getDebug()) {
			stdoutput.printf("decrypt-update failed\n");
			ERR_print_errors_fp(stdout);
		}
		success=false;
	}
	if (success) {
		totaloutsize+=outsize;
		if (getDebug()) {
			stdoutput.printf("decrypted %d bytes\n",outsize);
debugHexDump(out,outsize);
		}

		// finish decrypting
		if (!EVP_DecryptFinal_ex(ctx,out+outsize,&outsize)) {
			if (getDebug()) {
				stdoutput.printf("decrypt-final failed\n");
				ERR_print_errors_fp(stdout);
			}
			success=false;
		}
		totaloutsize+=outsize;
	}

	// copy out
	if (success) {
		decdata->append(out,totaloutsize);

		if (getDebug()) {
			stdoutput.printf("decryption succeeded\n");
		}
	}

	// clean up
	EVP_CIPHER_CTX_free(ctx);

	return success;
}

bool sqlrprotocol_teradata::encrypt(const byte_t *decdata,
						uint64_t decdatasize,
						bytebuffer *encdata) {
	// FIXME: push down to rudiments
	return true;
}

bool sqlrprotocol_teradata::generateServerPublicKey() {

	// FIXME: push down to rudiments

	// clear the server public key buffer
	bytestring::zero(serverpubkey,sizeof(serverpubkey));

	// reset the dh
	DH_free(dh);
	dh=DH_new();
#if OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)
	DH_set0_pqg(dh,BN_bin2bn(dhp,sizeof(dhp),NULL),
			NULL,
			BN_bin2bn(dhg,sizeof(dhg),NULL));
#else
	BN_free(dh->p);
	dh->p=BN_bin2bn(dhp,sizeof(dhp),NULL);
	BN_free(dh->g);
	dh->g=BN_bin2bn(dhg,sizeof(dhg),NULL);
#endif
	int	codes=0;
	if (!DH_check(dh,&codes)) {
		if (getDebug()) {
			stdoutput.printf("DH parameter check failed\n");
			ERR_print_errors_fp(stdout);
		}
		return false;
	}
	if (codes) {
		if (getDebug()) {
			stdoutput.printf("invalid DH parameters\n");
			ERR_print_errors_fp(stdout);
		}
		return false;
	}

	// generate new public/private keys
	if (DH_generate_key(dh)) {

		// get the public key
		const BIGNUM	*pubkey=NULL;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)
		DH_get0_key(dh,&pubkey,NULL);
#else
		pubkey=dh->pub_key;
#endif

		// verify that the key isn't too big for the buffer
		if ((uint64_t)BN_num_bytes(pubkey)<=
				(uint64_t)sizeof(serverpubkey)) {

			// copy out the key
			BN_bn2bin(pubkey,serverpubkey);

		} else {
			if (getDebug()) {
				stdoutput.printf("public key too large\n");
			}
			return false;
		}
	} else {
		if (getDebug()) {
			stdoutput.printf("generate key failed\n");
		}
		return false;
	}
	return true;
}

bool sqlrprotocol_teradata::generateSharedSecret() {

	// FIXME: push down to rudiments

	// convert the client public key to a BIGNUM
	BIGNUM	*cpkbn=BN_bin2bn(clientpubkey,sizeof(clientpubkey),NULL);

	// reallocate the shared secret buffers
	delete[] sharedsecret;
	sharedsecret=new byte_t[DH_size(dh)];
	bytestring::zero(sha2sharedsecret,sizeof(sha2sharedsecret));

	// compute the shared secret
	// (NOTE: sharedsecretsize might be less
	// than the size allocated for the buffer)
	int	result=DH_compute_key(sharedsecret,cpkbn,dh);

	// clean up
	BN_free(cpkbn);

	// handle success/failure
	if (result==-1) {
		delete[] sharedsecret;
		sharedsecret=NULL;
		sharedsecretsize=0;

		if (getDebug()) {
			stdoutput.printf("generate shared secret failed\n");
		}
		return false;
	} else {
		sharedsecretsize=result;
	}

	// get sha2 hash of the sharedsecret
	sha256		s256;
	if (!s256.append(sharedsecret,sharedsecretsize)) {
		if (getDebug()) {
			stdoutput.printf("sha2-append failed\n");
		}
		return false;
	}
	const byte_t	*hash=s256.getHash();
	if (!hash) {
		if (getDebug()) {
			stdoutput.printf("sha2-getHash failed\n");
		}
		return false;
	}
	bytestring::copy(sha2sharedsecret,hash,32);
	return true;
}
#endif

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_teradata(
						sqlrservercontroller *cont,
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_teradata(cont,ps,parameters);
	}
}
