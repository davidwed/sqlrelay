// Copyright (c) 1999-2019  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
#include <rudiments/bytebuffer.h>
#include <rudiments/process.h>
#include <rudiments/randomnumber.h>
#include <rudiments/file.h>
#include <rudiments/error.h>

#include <datatypes.h>

// operation codes
#define op_connect		1
#define op_attach		19
#define op_detach		21
#define op_create		20
#define op_drop_database	81
#define op_info_database	40
#define op_disconnect		6
#define op_transaction		29
#define op_commit		30
#define op_rollback		31
#define op_commit_retaining	50
#define op_prepare		32
#define op_prepare2		51
#define op_transaction_info	42
#define op_allocate_statement	62
#define op_free_statement	67
#define op_prepare_statement	68
#define op_execute		63	// for DDL/DML
#define op_execute2		76	// for Stored procedures
#define op_fetch		65
#define op_set_cursor		69
#define op_info_sql		70
#define op_create_blob		34
#define op_create_blob2		57
#define op_open_blob		35
#define op_open_blob2		56
#define op_get_segment		36
#define op_batch_segment	44
#define op_seek_blob		61
#define op_cancel_blob		38
#define op_close_blob		39
#define op_get_slice		58
#define op_put_slice		59
#define op_cancel		91
#define op_batch_create		99
#define op_batch_msg		100
#define op_batch_exec		101
#define op_batch_rls		102
#define op_batch_cancel		109
#define op_batch_sync		110
#define op_batch_set_bpb	106
#define op_batch_regblob	104
#define op_batch_blob_stream	105
#define op_service_attach	82
#define op_service_detach	83
#define op_service_start	85
#define op_service_info		84
#define op_connect_request	53
#define op_que_events		48
#define op_cancel_events	49

// response codes
#define op_accept		3
#define op_response		9
#define op_sql_response		78
#define op_fetch_response	66
#define op_slice		60



// arch codes
#define arch_generic		1
#define arch_sun		3
#define arch_sun4		8
#define arch_sunx86		9
#define arch_hpux		10
#define arch_rt			14
#define arch_intel_32		29
#define arch_linux		36
#define arch_freebsd		37
#define arch_netbsd		38
#define arch_darwin_ppc		39
#define arch_winnt_64		40
#define arch_darwin_x64		41
#define arch_darwin_ppc64	42
#define arch_arm		43

// protocol versions
#define PROTOCOL_VERSION2	2
#define PROTOCOL_VERSION3	3
// 4 supports server management functions
#define PROTOCOL_VERSION4	4
// 5 supports d_float data type
#define PROTOCOL_VERSION5	5
// 6 supports cancel remote events, blob seek, and unknown message type
#define PROTOCOL_VERSION6	6
// 7 supports dsql
#define PROTOCOL_VERSION7	7
// 8 supports includes collapsing first receive into a send, drop db,
// DSQL execute 2, DSQL execute immediate 2, DSQL insert, services, and
// transact request
#define PROTOCOL_VERSION8	8
// 9 includes support for SPX32
#define PROTOCOL_VERSION9	9
// 10 supports warnings, removes requirement for encoding/decoding status codes
#define PROTOCOL_VERSION10	10
// 11 supports user-auth-related operations
#define PROTOCOL_VERSION11	(0xffff8000|11)
// 12 supports asynchronous calls
#define PROTOCOL_VERSION12	(0xffff8000|12)
// 13 supports auth plugins
#define PROTOCOL_VERSION13	(0xffff8000|13)
// 14 bug-fix
#define PROTOCOL_VERSION14	(0xffff8000|14)
// 15 supports crypt key callback during connect
#define PROTOCOL_VERSION15	(0xffff8000|15)
// 16 supports statement timeouts
#define PROTOCOL_VERSION16	(0xffff8000|16)
// 17 supports op_batch_sync, op_batch_info
#define PROTOCOL_VERSION17	(0xffff8000|17)

// ptype codes
#define ptype_rpc		2
#define ptype_batch_send	3
#define ptype_out_of_band	4
#define ptype_lazy_send		5

// database parameters
#define isc_dpb_version1		1
#define isc_dpb_version2		2

#define isc_dpb_cdd_pathname		1
#define isc_dpb_allocation		2
#define isc_dpb_journal			3
#define isc_dpb_page_size		4
#define isc_dpb_num_buffers		5
#define isc_dpb_buffer_length		6
#define isc_dpb_debug			7
#define isc_dpb_garbage_collect		8
#define isc_dpb_verify			9
#define isc_dpb_sweep			10
#define isc_dpb_enable_journal		11
#define isc_dpb_disable_journal		12
#define isc_dpb_dbkey_scope		13
#define isc_dpb_number_of_users		14
#define isc_dpb_trace			15
#define isc_dpb_no_garbage_collect	16
#define isc_dpb_damaged			17
#define isc_dpb_license			18
#define isc_dpb_sys_user_name		19
#define isc_dpb_encrypt_key		20
#define isc_dpb_activate_shadow		21
#define isc_dpb_sweep_interval		22
#define isc_dpb_delete_shadow		23
#define isc_dpb_force_write		24
#define isc_dpb_begin_log		25
#define isc_dpb_quit_log		26
#define isc_dpb_no_reserve		27
#define isc_dpb_user_name		28
#define isc_dpb_password		29
#define isc_dpb_password_enc		30
#define isc_dpb_sys_user_name_enc	31
#define isc_dpb_interp			32
#define isc_dpb_online_dump		33
#define isc_dpb_old_file_size		34
#define isc_dpb_old_num_files		35
#define isc_dpb_old_file		36
#define isc_dpb_old_start_page		37
#define isc_dpb_old_start_seqno		38
#define isc_dpb_old_start_file		39
#define isc_dpb_drop_walfile		40
#define isc_dpb_old_dump_id		41
#define isc_dpb_wal_backup_dir		42
#define isc_dpb_wal_chkptlen		43
#define isc_dpb_wal_numbufs		44
#define isc_dpb_wal_bufsize		45
#define isc_dpb_wal_grp_cmt_wait	46
#define isc_dpb_lc_messages		47
#define isc_dpb_lc_ctype		48
#define isc_dpb_cache_manager		49
#define isc_dpb_shutdown		50
#define isc_dpb_online			51
#define isc_dpb_shutdown_delay		52
#define isc_dpb_reserved		53
#define isc_dpb_overwrite		54
#define isc_dpb_sec_attach		55
#define isc_dpb_disable_wal		56
#define isc_dpb_connect_timeout		57
#define isc_dpb_dummy_packet_interval	58
#define isc_dpb_gbak_attach		59
#define isc_dpb_sql_role_name		60
#define isc_dpb_set_page_buffers	61
#define isc_dpb_working_directory	62
#define isc_dpb_sql_dialect		63
#define isc_dpb_set_db_readonly		64
#define isc_dpb_set_db_sql_dialect	65
#define isc_dpb_gfix_attach		66
#define isc_dpb_gstat_attach		67
#define isc_dpb_set_db_charset		68
#define isc_dpb_gsec_attach		69
#define isc_dpb_address_path		70
#define isc_dpb_process_id		71
#define isc_dpb_no_db_triggers		72
#define isc_dpb_trusted_auth		73
#define isc_dpb_process_name		74
#define isc_dpb_trusted_role		75
#define isc_dpb_org_filename		76
#define isc_dpb_utf8_filename		77
#define isc_dpb_ext_call_depth		78
#define isc_dpb_auth_block		79
#define isc_dpb_client_version		80
#define isc_dpb_remote_protocol		81
#define isc_dpb_host_name		82
#define isc_dpb_os_user			83
#define isc_dpb_specific_auth_data	84
#define isc_dpb_auth_plugin_list	85
#define isc_dpb_auth_plugin_name	86
#define isc_dpb_config			87
#define isc_dpb_nolinger		88
#define isc_dpb_reset_icu		89
#define isc_dpb_map_attach		90
#define isc_dpb_session_time_zone	91
#define isc_dpb_set_db_replica		92
#define isc_dpb_set_bind		93
#define isc_dpb_decfloat_round		94
#define isc_dpb_decfloat_traps		95
#define isc_dpb_clear_map		96

// free statement flags
#define DSQL_close	1
#define DSQL_drop	2

// information items
#define isc_info_sql_select			4
#define isc_info_sql_bind			5
#define isc_info_sql_num_variables		6
#define isc_info_sql_describe_vars		7
#define isc_info_sql_describe_end		8
#define isc_info_sql_sqlda_seq			9
#define isc_info_sql_message_seq		10
#define isc_info_sql_type			11
#define isc_info_sql_sub_type			12
#define isc_info_sql_scale			13
#define isc_info_sql_length			14
#define isc_info_sql_null_ind			15
#define isc_info_sql_field			16
#define isc_info_sql_relation			17
#define isc_info_sql_owner			18
#define isc_info_sql_alias			19
#define isc_info_sql_sqlda_start		20
#define isc_info_sql_stmt_type			21
#define isc_info_sql_get_plan			22
#define isc_info_sql_records			23
#define isc_info_sql_batch_fetch		24
#define isc_info_sql_relation_alias		25
#define isc_info_sql_explain_plan		26
#define isc_info_sql_stmt_flags			27
#define isc_info_sql_stmt_timeout_user		28
#define isc_info_sql_stmt_timeout_run		29
#define isc_info_sql_stmt_blob_align		30
#define isc_info_sql_exec_path_blr_bytes	31
#define isc_info_sql_exec_path_blr_text		32

// status vector items
#define isc_arg_end		0
#define isc_arg_gds		1
#define isc_arg_string		2
#define isc_arg_cstring		3
#define isc_arg_number		4
#define isc_arg_interpreted	5
#define isc_arg_vms		6
#define isc_arg_unix		7
#define isc_arg_domain		8
#define isc_arg_dos		9
#define isc_arg_mpexl		10
#define isc_arg_mpexl_ipc	11
#define isc_arg_mach		15
#define isc_arg_netware		16
#define isc_arg_win32		17
#define isc_arg_warning		18
#define isc_arg_sql_state	19

// connection type
#define P_REQ_async	1

class SQLRSERVER_DLLSPEC sqlrprotocol_firebird : public sqlrprotocol {
	public:
			sqlrprotocol_firebird(sqlrservercontroller *cont,
							sqlrprotocols *ps,
							domnode *parameters);
		virtual	~sqlrprotocol_firebird();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

	private:
		void	init();
		void	free();

		bool	initialHandshake();
		bool	connect();
		bool	connectResponse();
		bool	attach();
		bool	attachResponse();

		bool	genericResponse(const char *title,
						uint32_t objecthandle,
						uint32_t objectid,
						uint32_t bufferlen,
						byte_t *buffer,
						uint64_t *statusvector,
						uint8_t statusvectorlen);
	
		bool	authenticate();

		bool	getOpCode();
		bool	detach();
		bool	create();
		bool	dropDatabase();
		bool	infoDatabase();
		bool	disconnect();
		bool	transaction();
		bool	commit();
		bool	rollback();
		bool	commitRetaining();
		bool	prepare();
		bool	prepare2();
		bool	transactionInfo();
		bool	allocateStatement();
		bool	freeStatement();
		bool	prepareStatement();
		bool	execute();
		bool	execute2();
		bool	fetch();
		bool	setCursor();
		bool	infoSql();
		bool	createBlob();
		bool	createBlob2();
		bool	openBlob();
		bool	openBlob2();
		bool	getSegment();
		bool	batchSegment();
		bool	seekBlob();
		bool	cancelBlob();
		bool	closeBlob();
		bool	getSlice();
		bool	putSlice();
		bool	cancel();
		bool	batchCreate();
		bool	batchMsg();
		bool	batchExec();
		bool	batchRls();
		bool	batchCancel();
		bool	batchSync();
		bool	batchSetBpb();
		bool	batchRegBlob();
		bool	batchBlobStream();
		bool	serviceAttach();
		bool	serviceDetach();
		bool	serviceStart();
		bool	serviceInfo();
		bool	connectRequest();
		bool	queEvents();
		bool	cancelEvents();
		bool	sendNotImplementedError();

		void	keepReading(int32_t sec, int32_t usec);

		void	readString(const byte_t *in,
					const char *name,
					char **buf,
					const byte_t **out);

		bool	readPadding(uint32_t bytesreadin,
					uint32_t *bytesreadout);

		void	debugSystemError();
		void	debugOpCode(uint32_t opcode);
		void	debugArchType(uint32_t archtype);
		void	debugProtocolVersion(uint32_t protoversion);
		void	debugProtocolType(const char *title,
						uint32_t protocoltype);
		void	debugDpbVersion(uint32_t dpbversion);
		void	debugDpbParam(uint32_t dpbparam);
		void	debugStatusVector(uint64_t *statusvector,
						uint8_t statusvectorlen);

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		filedescriptor	*clientsock;

		uint32_t	opcode;

		char	*db;
		char	*username;
		char	*password;
		char	*wd;
};


sqlrprotocol_firebird::sqlrprotocol_firebird(sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {

	clientsock=NULL;

	debugStart("parameters");
	debugEnd();

	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();

	init();
}

sqlrprotocol_firebird::~sqlrprotocol_firebird() {
	free();
}

void sqlrprotocol_firebird::init() {
	db=NULL;
	username=NULL;
	password=NULL;
	wd=NULL;
}

void sqlrprotocol_firebird::free() {
	delete[] db;
	delete[] username;
	delete[] password;
	delete[] wd;
}

clientsessionexitstatus_t sqlrprotocol_firebird::clientSession(
						filedescriptor *cs) {

	clientsock=cs;

	// Set up the socket...
	clientsock->translateByteOrder();
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);

	// Reinit session-local data...
	free();
	init();

	// state/status variables...
	bool				endsession=true;
	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;

	// perform the initial handshake...
	if (initialHandshake()) {

		// loop, getting and executing requests
		bool	loop=true;
		do {

			// get the request...
			if (!getOpCode()) {
				status=
				CLIENTSESSIONEXITSTATUS_CLOSED_CONNECTION;
				break;
			}

			// execute the request
			switch (opcode) {
				case op_detach:
					loop=detach();
					break;
				case op_create:
					loop=create();
					break;
				case op_drop_database:
					loop=dropDatabase();
					break;
				case op_info_database:
					loop=infoDatabase();
					break;
				case op_disconnect:
					loop=disconnect();
					break;
				case op_transaction:
					loop=transaction();
					break;
				case op_commit:
					loop=commit();
					break;
				case op_rollback:
					loop=rollback();
					break;
				case op_commit_retaining:
					loop=commitRetaining();
					break;
				case op_prepare:
					loop=prepare();
					break;
				case op_prepare2:
					loop=prepare2();
					break;
				case op_transaction_info:
					loop=transactionInfo();
					break;
				case op_allocate_statement:
					loop=allocateStatement();
					break;
				case op_free_statement:
					loop=freeStatement();
					break;
				case op_prepare_statement:
					loop=prepareStatement();
					break;
				case op_execute:
					loop=execute();
					break;
				case op_execute2:
					loop=execute2();
					break;
				case op_fetch:
					loop=fetch();
					break;
				case op_set_cursor:
					loop=setCursor();
					break;
				case op_info_sql:
					loop=infoSql();
					break;
				case op_create_blob:
					loop=createBlob();
					break;
				case op_create_blob2:
					loop=createBlob2();
					break;
				case op_open_blob:
					loop=openBlob();
					break;
				case op_open_blob2:
					loop=openBlob2();
					break;
				case op_get_segment:
					loop=getSegment();
					break;
				case op_batch_segment:
					loop=batchSegment();
					break;
				case op_seek_blob:
					loop=seekBlob();
					break;
				case op_cancel_blob:
					loop=cancelBlob();
					break;
				case op_close_blob:
					loop=closeBlob();
					break;
				case op_get_slice:
					loop=getSlice();
					break;
				case op_put_slice:
					loop=putSlice();
					break;
				case op_cancel:
					loop=cancel();
					break;
				case op_batch_create:
					loop=batchCreate();
					break;
				case op_batch_msg:
					loop=batchMsg();
					break;
				case op_batch_exec:
					loop=batchExec();
					break;
				case op_batch_rls:
					loop=batchRls();
					break;
				case op_batch_cancel:
					loop=batchCancel();
					break;
				case op_batch_sync:
					loop=batchSync();
					break;
				case op_batch_set_bpb:
					loop=batchSetBpb();
					break;
				case op_batch_regblob:
					loop=batchRegBlob();
					break;
				case op_batch_blob_stream:
					loop=batchBlobStream();
					break;
				case op_service_attach:
					loop=serviceAttach();
					break;
				case op_service_detach:
					loop=serviceDetach();
					break;
				case op_service_start:
					loop=serviceStart();
					break;
				case op_service_info:
					loop=serviceInfo();
					break;
				case op_connect_request:
					loop=connectRequest();
					break;
				case op_que_events:
					loop=queEvents();
					break;
				case op_cancel_events:
					loop=cancelEvents();
					break;
				default:
					loop=sendNotImplementedError();
					break;
			}

		} while (loop);
	}

	// close the client connection
	cont->closeClientConnection(0);

	// end the session if necessary
	if (endsession) {
		cont->endSession();
	}

	// return the status
	return status;
}

bool sqlrprotocol_firebird::initialHandshake() {
	return connect() && connectResponse() && attach() && attachResponse();
}

bool sqlrprotocol_firebird::connect() {

	// request packet data structure:
	//
	// data {
	// 	int32_t		op_connect
	// 	int32_t		op_attach
	// 	int32_t		connect version
	// 	int32_t		arch type
	// 	int32_t		db length
	// 	char[]		db path or alias
	// 	int32_t		count of protocol versions understood
	// 	int32_t		user id length
	// 	byte_t[]	user id
	//
	// 	// protocols...
	// 	int32_t		protocol version
	// 	int32_t		arch type
	// 	int32_t		minimum type
	// 	int32_t		maximum type
	// 	int32_t		preference weight
	// 	...
	// }

	debugStart("connect");

	uint32_t	bytesread=0;

	// get op_connect
	uint32_t	opcode=0;
	if (clientsock->read(&opcode)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read connect op code failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugOpCode(opcode);
	}
	if (opcode!=op_connect) {
		if (getDebug()) {
			stdoutput.printf("	invalid connect op code - "
							"got %d, expected %d\n",
							opcode,op_connect);
			debugEnd();
		}
		return false;
	}
	bytesread+=sizeof(uint32_t);

	// get op_attach
	if (clientsock->read(&opcode)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read attach op code failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugOpCode(opcode);
	}
	if (opcode!=op_attach) {
		if (getDebug()) {
			stdoutput.printf("	invalid attach op code - "
							"got %d, expected %d\n",
							opcode,op_attach);
			debugEnd();
		}
		return false;
	}
	bytesread+=sizeof(uint32_t);

	// get connect version
	uint32_t	connectversion=0;
	if (clientsock->read(&connectversion)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read connect version failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	connect version: %d\n",connectversion);
	}
	bytesread+=sizeof(uint32_t);

	// get arch type
	uint32_t	archtype=0;
	if (clientsock->read(&archtype)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read arch type failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugArchType(archtype);
	}
	bytesread+=sizeof(uint32_t);

	// get db
	uint32_t	dblen=0;
	if (clientsock->read(&dblen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read db length failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db len: %d\n",dblen);
	}
	bytesread+=sizeof(uint32_t);
	db=new char[dblen+1];
	if (clientsock->read(db,dblen)!=dblen) {
		if (getDebug()) {
			stdoutput.write("	read db path failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	db[dblen]='\0';
	if (getDebug()) {
		stdoutput.printf("	db: %s\n",db);
	}
	bytesread+=dblen;

	// padding
	if (!readPadding(bytesread,&bytesread)) {
		return false;
	}

	// get protocol count
	uint32_t	protocount=0;
	if (clientsock->read(&protocount)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read proto count failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	protocol count: %d\n",protocount);
	}
	bytesread+=sizeof(uint32_t);

	// get user identification
	uint32_t	useridlen=0;
	if (clientsock->read(&useridlen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read user id length failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	user id len: %d\n",useridlen);
	}
	bytesread+=sizeof(uint32_t);
	byte_t	*userid=new byte_t[useridlen+1];
	if (clientsock->read(userid,useridlen)!=useridlen) {
		if (getDebug()) {
			stdoutput.write("	read user id failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	userid[useridlen]='\0';
	if (getDebug()) {
		stdoutput.printf("	user id:\n");
		stdoutput.printHex(userid,useridlen);
	}
	bytesread+=useridlen;

	// padding
	if (!readPadding(bytesread,&bytesread)) {
		return false;
	}

	// get protocols...
	for (uint32_t i=0; i<protocount; i++) {

		// FIXME: I seem to get nonsensical values for these...

		stdoutput.printf("	protocol %d...\n",i);

		// get protocol version
		uint32_t	protoversion=0;
		if (clientsock->read(&protoversion)!=sizeof(uint32_t)) {
			if (getDebug()) {
				stdoutput.write("	read proto "
							"version failed\n");
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		if (getDebug()) {
			debugProtocolVersion(protoversion);
		}
		bytesread+=sizeof(uint32_t);

		// get arch type
		uint32_t	archtype=0;
		if (clientsock->read(&archtype)!=sizeof(uint32_t)) {
			if (getDebug()) {
				stdoutput.write("	read arch type "
								"failed\n");
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		if (getDebug()) {
			debugArchType(archtype);
		}
		bytesread+=sizeof(uint32_t);

		// get minimum type
		uint32_t	mintype=0;
		if (clientsock->read(&mintype)!=sizeof(uint32_t)) {
			if (getDebug()) {
				stdoutput.write("	read min type "
								"failed\n");
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		if (getDebug()) {
			debugProtocolType("min type",mintype);
		}
		bytesread+=sizeof(uint32_t);

		// get maximum type
		uint32_t	maxtype=0;
		if (clientsock->read(&maxtype)!=sizeof(uint32_t)) {
			if (getDebug()) {
				stdoutput.write("	read max type "
								"failed\n");
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		if (getDebug()) {
			debugProtocolType("max type",maxtype);
		}
		bytesread+=sizeof(uint32_t);

		// get preference weight
		uint32_t	prefwt=0;
		if (clientsock->read(&prefwt)!=sizeof(uint32_t)) {
			if (getDebug()) {
				stdoutput.write("	read preference weight"
								"failed\n");
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		if (getDebug()) {
			stdoutput.printf("	preference "
						"weight: %d\n",prefwt);
		}
		bytesread+=sizeof(uint32_t);
	}

	debugEnd();

	// FIXME: decide whether to accept to connection or not...

	// FIXME: decide which protocol to use...

	return true;
}

bool sqlrprotocol_firebird::connectResponse() {

	// response packet data structure:
	//
	// data {
	// 	int32_t		op_accept
	// 	int32_t		arch type
	// 	int32_t		minimum type
	// }

	debugStart("connect response");

	uint32_t	opcode=op_accept;
	if (clientsock->write(opcode)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write op code failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugOpCode(opcode);
	}

	// FIXME: determine this somehow
	uint32_t	protoversion=PROTOCOL_VERSION10;
	if (clientsock->write(protoversion)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write protocol "
						"version failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	protocol version: %d\n",
							protoversion);
	}

	// FIXME: determine this somehow
	uint32_t	archtype=arch_generic;
	if (clientsock->write(archtype)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write arch type failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugArchType(archtype);
	}

	// FIXME: determine this somehow
	uint32_t	mintype=ptype_rpc;
	if (clientsock->write(mintype)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write min type failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugProtocolType("min type",mintype);
	}

	debugEnd();

	clientsock->flushWriteBuffer(-1,-1);

	return true;
}

bool sqlrprotocol_firebird::attach() {

	// request packet data structure:
	//
	// data {
	// 	int32_t		op_attach
	// 	int32_t		db object id
	// 	int32_t		db length
	// 	char[]		db path or alias
	// 	int32_t		db parameter buffer length
	// 	byte_t[]	db parameter buffer
	// 	...
	// }

	debugStart("attach");

	uint32_t	bytesread=0;

	// get op_attach
	uint32_t	opcode=0;
	if (clientsock->read(&opcode)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read attach op code failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugOpCode(opcode);
	}
	if (opcode!=op_attach) {
		if (getDebug()) {
			stdoutput.printf("	invalid attach op code - "
							"got %d, expected %d\n",
							opcode,op_attach);
			debugEnd();
		}
		return false;
	}
	bytesread+=sizeof(uint32_t);

	// get db object id
	uint32_t	dbobjectid=0;
	if (clientsock->read(&dbobjectid)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read db object id failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db object id: %d\n",dbobjectid);
	}
	bytesread+=sizeof(uint32_t);

	// get db (FIXME: override db from connect?)
	uint32_t	dblen=0;
	if (clientsock->read(&dblen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read db length failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db len: %d\n",dblen);
	}
	bytesread+=sizeof(uint32_t);
	delete[] db;
	db=new char[dblen+1];
	if (clientsock->read(db,dblen)!=dblen) {
		if (getDebug()) {
			stdoutput.write("	read db failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	db[dblen]='\0';
	if (getDebug()) {
		stdoutput.printf("	db: %s\n",db);
	}
	bytesread+=dblen;

	// padding
	if (!readPadding(bytesread,&bytesread)) {
		return false;
	}

	// get db parameters buffer
	uint32_t	dpblen=0;
	if (clientsock->read(&dpblen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read db param buffer "
							"length failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db param buffer len: %d\n",dpblen);
	}
	bytesread+=sizeof(uint32_t);
	byte_t	*dpb=new byte_t[dpblen+1];
	if (clientsock->read(dpb,dpblen)!=dpblen) {
		if (getDebug()) {
			stdoutput.write("	read db param buffer failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db param buffer:\n");
		stdoutput.printHex(dpb,dpblen);
	}
	bytesread+=dpblen;

	// padding
	if (!readPadding(bytesread,&bytesread)) {
		return false;
	}

	// process db parameters buffer...
	const byte_t	*dpbptr=dpb;
	const byte_t	*dpbendptr=dpb+dpblen;

	// get the dpb version
	// FIXME: do something with this...
	byte_t		dpbversion;
	read(dpbptr,&dpbversion,&dpbptr);
	if (getDebug()) {
		debugDpbVersion(dpbversion);
	}

	// get each parameter...
	while (dpbptr!=dpbendptr) {
		
		// get the parameter
		byte_t	dpbparam;
		read(dpbptr,&dpbparam,&dpbptr);
		if (getDebug()) {
			debugDpbParam(dpbparam);
		}

		// process the parameter...
		switch (dpbparam) {
			case isc_dpb_cdd_pathname:
				// FIXME: do something...
				break;

			case isc_dpb_allocation:
				// FIXME: do something...
				break;

			case isc_dpb_journal:
				// FIXME: do something...
				break;

			case isc_dpb_page_size:
				// FIXME: do something...
				break;

			case isc_dpb_num_buffers:
				// FIXME: do something...
				break;

			case isc_dpb_buffer_length:
				// FIXME: do something...
				break;

			case isc_dpb_debug:
				// FIXME: do something...
				break;

			case isc_dpb_garbage_collect:
				// FIXME: do something...
				break;

			case isc_dpb_verify:
				// FIXME: do something...
				break;

			case isc_dpb_sweep:
				// FIXME: do something...
				break;

			case isc_dpb_enable_journal:
				// FIXME: do something...
				break;

			case isc_dpb_disable_journal:
				// FIXME: do something...
				break;

			case isc_dpb_dbkey_scope:
				// FIXME: do something...
				break;

			case isc_dpb_number_of_users:
				// FIXME: do something...
				break;

			case isc_dpb_trace:
				// FIXME: do something...
				break;

			case isc_dpb_no_garbage_collect:
				// FIXME: do something...
				break;

			case isc_dpb_damaged:
				// FIXME: do something...
				break;

			case isc_dpb_license:
				// FIXME: do something...
				break;

			case isc_dpb_sys_user_name:
				// FIXME: do something...
				break;

			case isc_dpb_encrypt_key:
				// FIXME: do something...
				break;

			case isc_dpb_activate_shadow:
				// FIXME: do something...
				break;

			case isc_dpb_sweep_interval:
				// FIXME: do something...
				break;

			case isc_dpb_delete_shadow:
				// FIXME: do something...
				break;

			case isc_dpb_force_write:
				// FIXME: do something...
				break;

			case isc_dpb_begin_log:
				// FIXME: do something...
				break;

			case isc_dpb_quit_log:
				// FIXME: do something...
				break;

			case isc_dpb_no_reserve:
				// FIXME: do something...
				break;

			case isc_dpb_user_name:
				readString(dpbptr,"user name",
						&username,&dpbptr);
				break;

			case isc_dpb_password:
				// FIXME: do something...
				break;

			case isc_dpb_password_enc:
				readString(dpbptr,"password",
						&password,&dpbptr);
				break;


			case isc_dpb_sys_user_name_enc:
				// FIXME: do something...
				break;

			case isc_dpb_interp:
				// FIXME: do something...
				break;

			case isc_dpb_online_dump:
				// FIXME: do something...
				break;

			case isc_dpb_old_file_size:
				// FIXME: do something...
				break;

			case isc_dpb_old_num_files:
				// FIXME: do something...
				break;

			case isc_dpb_old_file:
				// FIXME: do something...
				break;

			case isc_dpb_old_start_page:
				// FIXME: do something...
				break;

			case isc_dpb_old_start_seqno:
				// FIXME: do something...
				break;

			case isc_dpb_old_start_file:
				// FIXME: do something...
				break;

			case isc_dpb_drop_walfile:
				// FIXME: do something...
				break;

			case isc_dpb_old_dump_id:
				// FIXME: do something...
				break;

			case isc_dpb_wal_backup_dir:
				// FIXME: do something...
				break;

			case isc_dpb_wal_chkptlen:
				// FIXME: do something...
				break;

			case isc_dpb_wal_numbufs:
				// FIXME: do something...
				break;

			case isc_dpb_wal_bufsize:
				// FIXME: do something...
				break;

			case isc_dpb_wal_grp_cmt_wait:
				// FIXME: do something...
				break;

			case isc_dpb_lc_messages:
				// FIXME: do something...
				break;

			case isc_dpb_lc_ctype:
				// FIXME: do something...
				break;

			case isc_dpb_cache_manager:
				// FIXME: do something...
				break;

			case isc_dpb_shutdown:
				// FIXME: do something...
				break;

			case isc_dpb_online:
				// FIXME: do something...
				break;

			case isc_dpb_shutdown_delay:
				// FIXME: do something...
				break;

			case isc_dpb_reserved:
				// FIXME: do something...
				break;

			case isc_dpb_overwrite:
				// FIXME: do something...
				break;

			case isc_dpb_sec_attach:
				// FIXME: do something...
				break;

			case isc_dpb_disable_wal:
				// FIXME: do something...
				break;

			case isc_dpb_connect_timeout:
				// FIXME: do something...
				break;

			case isc_dpb_dummy_packet_interval:
				char	*val;
				readString(dpbptr,"val",&val,&dpbptr);
				delete[] val;
				break;

			case isc_dpb_gbak_attach:
				// FIXME: do something...
				break;

			case isc_dpb_sql_role_name:
				// FIXME: do something...
				break;

			case isc_dpb_set_page_buffers:
				// FIXME: do something...
				break;

			case isc_dpb_working_directory:
				readString(dpbptr,"working directory",
								&wd,&dpbptr);
				break;

			case isc_dpb_sql_dialect:
				// FIXME: do something...
				break;

			case isc_dpb_set_db_readonly:
				// FIXME: do something...
				break;

			case isc_dpb_set_db_sql_dialect:
				// FIXME: do something...
				break;

			case isc_dpb_gfix_attach:
				// FIXME: do something...
				break;

			case isc_dpb_gstat_attach:
				// FIXME: do something...
				break;

			case isc_dpb_set_db_charset:
				// FIXME: do something...
				break;

			case isc_dpb_gsec_attach:
				// FIXME: do something...
				break;

			case isc_dpb_address_path:
				// FIXME: do something...
				break;

			case isc_dpb_process_id:
				// FIXME: do something...
				break;

			case isc_dpb_no_db_triggers:
				// FIXME: do something...
				break;

			case isc_dpb_trusted_auth:
				// FIXME: do something...
				break;

			case isc_dpb_process_name:
				// FIXME: do something...
				break;

			case isc_dpb_trusted_role:
				// FIXME: do something...
				break;

			case isc_dpb_org_filename:
				// FIXME: do something...
				break;

			case isc_dpb_utf8_filename:
				// FIXME: do something...
				break;

			case isc_dpb_ext_call_depth:
				// FIXME: do something...
				break;

			case isc_dpb_auth_block:
				// FIXME: do something...
				break;

			case isc_dpb_client_version:
				// FIXME: do something...
				break;

			case isc_dpb_remote_protocol:
				// FIXME: do something...
				break;

			case isc_dpb_host_name:
				// FIXME: do something...
				break;

			case isc_dpb_os_user:
				// FIXME: do something...
				break;

			case isc_dpb_specific_auth_data:
				// FIXME: do something...
				break;

			case isc_dpb_auth_plugin_list:
				// FIXME: do something...
				break;

			case isc_dpb_auth_plugin_name:
				// FIXME: do something...
				break;

			case isc_dpb_config:
				// FIXME: do something...
				break;

			case isc_dpb_nolinger:
				// FIXME: do something...
				break;

			case isc_dpb_reset_icu:
				// FIXME: do something...
				break;

			case isc_dpb_map_attach:
				// FIXME: do something...
				break;

			case isc_dpb_session_time_zone:
				// FIXME: do something...
				break;

			case isc_dpb_set_db_replica:
				// FIXME: do something...
				break;

			case isc_dpb_set_bind:
				// FIXME: do something...
				break;

			case isc_dpb_decfloat_round:
				// FIXME: do something...
				break;

			case isc_dpb_decfloat_traps:
				// FIXME: do something...
				break;

			case isc_dpb_clear_map:
				// FIXME: do something...
				break;

			default:
				// FIXME: do something...
				break;
		}
	}

	debugEnd();

	return true;
}

bool sqlrprotocol_firebird::attachResponse() {

	// FIXME: object handle should be the database handle ???
	// FIXME: no idea what the database handle is
	uint32_t	objecthandle=0;

	// FIXME: no idea what the object id is
	uint32_t	objectid=0;

	// status vector...
	uint64_t	statusvector[20];
	bytestring::zero(statusvector,sizeof(statusvector));
	// interbase error...
	statusvector[0]=isc_arg_gds;
	// no error...
	statusvector[1]=0;
	uint8_t		statusvectorlen=2;

	return genericResponse("attach response",
				objecthandle,objectid,
				0,NULL,
				statusvector,statusvectorlen);
}

bool sqlrprotocol_firebird::genericResponse(const char *title,
						uint32_t objecthandle,
						uint32_t objectid,
						uint32_t bufferlen,
						byte_t *buffer,
						uint64_t *statusvector,
						uint8_t statusvectorlen) {

	// response packet data structure:
	//
	// data {
	// 	int32_t		op_response
	// 	int32_t		object handle
	// 	int32_t		object id
	// 	int32_t		buffer length
	// 	byte_t[]	buffer
	// 	byte_t[]	status vector
	// }

	debugStart("attach response");

	// write the opcode
	uint32_t	opcode=op_response;
	if (clientsock->write(opcode)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write op code failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugOpCode(opcode);
	}

	// write the object handle
	if (clientsock->write(objecthandle)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write object handle failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	object handle: %d\n",objecthandle);
	}

	if (clientsock->write(objectid)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write object id failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	object id: %d\n",objectid);
	}

	// write the buffer
	if (clientsock->write(bufferlen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write buffer length failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	buffer len: %d\n",bufferlen);
	}
	if (clientsock->write(buffer,bufferlen)!=bufferlen) {
		if (getDebug()) {
			stdoutput.write("	write buffer failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.write("	buffer:\n");
		stdoutput.printHex(buffer,bufferlen);
	}

	// write the status vector
	for (uint8_t i=0; i<statusvectorlen; i++) {
		if (clientsock->write(statusvector[i])!=sizeof(uint64_t)) {
			if (getDebug()) {
				stdoutput.printf("	write status "
						"vector [%d] failed\n",i);
				debugSystemError();
				debugEnd();
			}
			return false;
		}
	}
	if (getDebug()) {
		debugStatusVector(statusvector,statusvectorlen);
	}

	debugEnd();

	clientsock->flushWriteBuffer(-1,-1);

	return true;
}

bool sqlrprotocol_firebird::authenticate() {

#if 0
	// build auth credentials
	sqlrfirebirdcredentials	cred;
	cred.setUser(user);
	cred.setPassword(password);
	cred.setPasswordLength(charstring::length(password));
	cred.setMethod(authmethod);
	cred.setSalt(salt);

	// authenticate
	bool	retval=cont->auth(&cred);

	// debug
	if (getDebug()) {
		debugStart("authenticate");
		stdoutput.printf("	auth %s\n",(retvale?"success":"failed");
		debugEnd();
	}

	// error
	if (!retval) {
		stringbuffer	err;
		err.append("password authentication failed for user \"");
		err.append(user);
		err.append("\"");
		sendErrorResponse("FATAL","28P01",
					err.getString(),err.getStringLength());
		return false;
	}

	// success
	return sendAuthenticationOk();
#endif
	return false;
}

bool sqlrprotocol_firebird::getOpCode() {

	debugStart("get op code");
	if (clientsock->read(&opcode)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read op code failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		debugOpCode(opcode);
	}
	debugEnd();
	return true;
}

bool sqlrprotocol_firebird::detach() {
	return false;
}

bool sqlrprotocol_firebird::create() {
	return false;
}

bool sqlrprotocol_firebird::dropDatabase() {
	return false;
}

bool sqlrprotocol_firebird::infoDatabase() {
	return false;
}

bool sqlrprotocol_firebird::disconnect() {

	// no response packet

	debugStart("disconnect");
	debugEnd();

	// return false on purpose here
	return false;
}

bool sqlrprotocol_firebird::transaction() {
	return false;
}

bool sqlrprotocol_firebird::commit() {
	return false;
}

bool sqlrprotocol_firebird::rollback() {
	return false;
}

bool sqlrprotocol_firebird::commitRetaining() {
	return false;
}

bool sqlrprotocol_firebird::prepare() {
	return false;
}

bool sqlrprotocol_firebird::prepare2() {
	return false;
}

bool sqlrprotocol_firebird::transactionInfo() {
	return false;
}

bool sqlrprotocol_firebird::allocateStatement() {
	return false;
}

bool sqlrprotocol_firebird::freeStatement() {
	return false;
}

bool sqlrprotocol_firebird::prepareStatement() {
	return false;
}

bool sqlrprotocol_firebird::execute() {
	return false;
}

bool sqlrprotocol_firebird::execute2() {
	return false;
}

bool sqlrprotocol_firebird::fetch() {
	return false;
}

bool sqlrprotocol_firebird::setCursor() {
	return false;
}

bool sqlrprotocol_firebird::infoSql() {
	return false;
}

bool sqlrprotocol_firebird::createBlob() {
	return false;
}

bool sqlrprotocol_firebird::createBlob2() {
	return false;
}

bool sqlrprotocol_firebird::openBlob() {
	return false;
}

bool sqlrprotocol_firebird::openBlob2() {
	return false;
}

bool sqlrprotocol_firebird::getSegment() {
	return false;
}

bool sqlrprotocol_firebird::batchSegment() {
	return false;
}

bool sqlrprotocol_firebird::seekBlob() {
	return false;
}

bool sqlrprotocol_firebird::cancelBlob() {
	return false;
}

bool sqlrprotocol_firebird::closeBlob() {
	return false;
}

bool sqlrprotocol_firebird::getSlice() {
	return false;
}

bool sqlrprotocol_firebird::putSlice() {
	return false;
}

bool sqlrprotocol_firebird::cancel() {
	return false;
}

bool sqlrprotocol_firebird::batchCreate() {
	return false;
}

bool sqlrprotocol_firebird::batchMsg() {
	return false;
}

bool sqlrprotocol_firebird::batchExec() {
	return false;
}

bool sqlrprotocol_firebird::batchRls() {
	return false;
}

bool sqlrprotocol_firebird::batchCancel() {
	return false;
}

bool sqlrprotocol_firebird::batchSync() {
	return false;
}

bool sqlrprotocol_firebird::batchSetBpb() {
	return false;
}

bool sqlrprotocol_firebird::batchRegBlob() {
	return false;
}

bool sqlrprotocol_firebird::batchBlobStream() {
	return false;
}

bool sqlrprotocol_firebird::serviceAttach() {
	return false;
}

bool sqlrprotocol_firebird::serviceDetach() {
	return false;
}

bool sqlrprotocol_firebird::serviceStart() {
	return false;
}

bool sqlrprotocol_firebird::serviceInfo() {
	return false;
}

bool sqlrprotocol_firebird::connectRequest() {
	return false;
}

bool sqlrprotocol_firebird::queEvents() {
	return false;
}

bool sqlrprotocol_firebird::cancelEvents() {
	return false;
}

bool sqlrprotocol_firebird::sendNotImplementedError() {
	return false;
}

void sqlrprotocol_firebird::keepReading(int32_t sec, int32_t usec) {
	for (;;) {
		byte_t	buffer[1024];
		ssize_t	r=clientsock->read(&buffer,1024,sec,usec);
		if (getDebug()) {
			stdoutput.printf("read %d more bytes...\n",r);
		}
		if (r<1) {
			break;
		}
		debugHexDump(buffer,r);
	}
}

void sqlrprotocol_firebird::readString(const byte_t *in,
					const char *name,
					char **buf,
					const byte_t **out) {

	// get the length
	byte_t	len;
	read(in,&len,out);

	// get the value
	*buf=new char[len+1];
	read(*out,*buf,len,out);
	(*buf)[len]='\0';
	if (getDebug()) {
		stdoutput.printf("	%s: %s\n",name,*buf);
	}
}

bool sqlrprotocol_firebird::readPadding(uint32_t bytesreadin,
					uint32_t *bytesreadout) {

	// initialize return value
	*bytesreadout=bytesreadin;

	// how much padding do we need to read?
	// (pad to a 4-byte boundary)
	uint32_t	pad=(((bytesreadin/4)+1)*4)-bytesreadin;

	// bail if we don't need to read any padding
	if (!pad) {
		return true;
	}

	// read the padding
	byte_t		dummy;
	for (uint32_t i=0; i<pad; i++) {
		if (clientsock->read(&dummy)!=sizeof(byte_t)) {
			if (getDebug()) {
				stdoutput.write("	read padding failed\n");
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		(*bytesreadout)++;
	}
	if (getDebug()) {
		stdoutput.printf("	read %d bytes of padding\n",pad);
	}
	return true;
}

void sqlrprotocol_firebird::debugSystemError() {
	char	*err=error::getErrorString();
	stdoutput.printf("%s\n",err);
	delete[] err;
}

void sqlrprotocol_firebird::debugOpCode(uint32_t opcode) {
	const char	*opcodestr=NULL;
	switch (opcode) {
		case op_connect:
			opcodestr="op_connect";
			break;
		case op_attach:
			opcodestr="op_attach";
			break;
		case op_detach:
			opcodestr="op_detach";
			break;
		case op_create:
			opcodestr="op_create";
			break;
		case op_drop_database:
			opcodestr="op_drop_database";
			break;
		case op_info_database:
			opcodestr="op_info_database";
			break;
		case op_disconnect:
			opcodestr="op_disconnect";
			break;
		case op_transaction:
			opcodestr="op_transaction";
			break;
		case op_commit:
			opcodestr="op_commit";
			break;
		case op_rollback:
			opcodestr="op_rollback";
			break;
		case op_commit_retaining:
			opcodestr="op_commit_retaining";
			break;
		case op_prepare:
			opcodestr="op_prepare";
			break;
		case op_prepare2:
			opcodestr="op_prepare2";
			break;
		case op_transaction_info:
			opcodestr="op_transaction_info";
			break;
		case op_allocate_statement:
			opcodestr="op_allocate_statement";
			break;
		case op_free_statement:
			opcodestr="op_free_statement";
			break;
		case op_prepare_statement:
			opcodestr="op_prepare_statement";
			break;
		case op_execute:
			opcodestr="op_execute";
			break;
		case op_execute2:
			opcodestr="op_execute2";
			break;
		case op_fetch:
			opcodestr="op_fetch";
			break;
		case op_set_cursor:
			opcodestr="op_set_cursor";
			break;
		case op_info_sql:
			opcodestr="op_info_sql";
			break;
		case op_create_blob:
			opcodestr="op_create_blob";
			break;
		case op_create_blob2:
			opcodestr="op_create_blob2";
			break;
		case op_open_blob:
			opcodestr="op_open_blob";
			break;
		case op_open_blob2:
			opcodestr="op_open_blob2";
			break;
		case op_get_segment:
			opcodestr="op_get_segment";
			break;
		case op_batch_segment:
			opcodestr="op_batch_segment";
			break;
		case op_seek_blob:
			opcodestr="op_seek_blob";
			break;
		case op_cancel_blob:
			opcodestr="op_cancel_blob";
			break;
		case op_close_blob:
			opcodestr="op_close_blob";
			break;
		case op_get_slice:
			opcodestr="op_get_slice";
			break;
		case op_put_slice:
			opcodestr="op_put_slice";
			break;
		case op_cancel:
			opcodestr="op_cancel";
			break;
		case op_batch_create:
			opcodestr="op_batch_create";
			break;
		case op_batch_msg:
			opcodestr="op_batch_msg";
			break;
		case op_batch_exec:
			opcodestr="op_batch_exec";
			break;
		case op_batch_rls:
			opcodestr="op_batch_rls";
			break;
		case op_batch_cancel:
			opcodestr="op_batch_cancel";
			break;
		case op_batch_sync:
			opcodestr="op_batch_sync";
			break;
		case op_batch_set_bpb:
			opcodestr="op_batch_set_bpb";
			break;
		case op_batch_regblob:
			opcodestr="op_batch_regblob";
			break;
		case op_batch_blob_stream:
			opcodestr="op_batch_blob_stream";
			break;
		case op_service_attach:
			opcodestr="op_service_attach";
			break;
		case op_service_detach:
			opcodestr="op_service_detach";
			break;
		case op_service_start:
			opcodestr="op_service_start";
			break;
		case op_service_info:
			opcodestr="op_service_info";
			break;
		case op_connect_request:
			opcodestr="op_connect_request";
			break;
		case op_que_events:
			opcodestr="op_que_events";
			break;
		case op_cancel_events:
			opcodestr="op_cancel_events";
			break;
		case op_accept:
			opcodestr="op_accept";
			break;
		case op_response:
			opcodestr="op_response";
			break;
		case op_sql_response:
			opcodestr="op_sql_response";
			break;
		case op_fetch_response:
			opcodestr="op_fetch_response";
			break;
		case op_slice:
			opcodestr="op_slice";
			break;
		default:
			opcodestr="unknown";
			break;
	}
	stdoutput.printf("	op code: %d (%s)\n",opcode,opcodestr);
}

void sqlrprotocol_firebird::debugArchType(uint32_t archtype) {
	const char	*archtypestr=NULL;
	switch (archtype) {
		case arch_generic:
			archtypestr="arch_generic";
			break;
		case arch_sun:
			archtypestr="arch_sun";
			break;
		case arch_sun4:
			archtypestr="arch_sun4";
			break;
		case arch_sunx86:
			archtypestr="arch_sunx86";
			break;
		case arch_hpux:
			archtypestr="arch_hpux";
			break;
		case arch_rt:
			archtypestr="arch_rt";
			break;
		case arch_intel_32:
			archtypestr="arch_intel_32";
			break;
		case arch_linux:
			archtypestr="arch_linux";
			break;
		case arch_freebsd:
			archtypestr="arch_freebsd";
			break;
		case arch_netbsd:
			archtypestr="arch_netbsd";
			break;
		case arch_darwin_ppc:
			archtypestr="arch_darwin_ppc";
			break;
		case arch_winnt_64:
			archtypestr="arch_winnt_64";
			break;
		case arch_darwin_x64:
			archtypestr="arch_darwin_x64";
			break;
		case arch_darwin_ppc64:
			archtypestr="arch_darwin_ppc64";
			break;
		case arch_arm:
			archtypestr="arch_arm";
			break;
		default:
			archtypestr="unknown";
			break;
	}
	stdoutput.printf("	arch type: %d (%s)\n",archtype,archtypestr);
}

void sqlrprotocol_firebird::debugProtocolVersion(uint32_t protoversion) {
	const char	*protoversionstr=NULL;
	switch (protoversion) {
		case PROTOCOL_VERSION2:
			protoversionstr="PROTOCOL_VERSION2";
			break;
		case PROTOCOL_VERSION3:
			protoversionstr="PROTOCOL_VERSION3";
			break;
		case PROTOCOL_VERSION4:
			protoversionstr="PROTOCOL_VERSION4";
			break;
		case PROTOCOL_VERSION5:
			protoversionstr="PROTOCOL_VERSION5";
			break;
		case PROTOCOL_VERSION6:
			protoversionstr="PROTOCOL_VERSION6";
			break;
		case PROTOCOL_VERSION7:
			protoversionstr="PROTOCOL_VERSION7";
			break;
		case PROTOCOL_VERSION8:
			protoversionstr="PROTOCOL_VERSION8";
			break;
		case PROTOCOL_VERSION9:
			protoversionstr="PROTOCOL_VERSION9";
			break;
		case PROTOCOL_VERSION10:
			protoversionstr="PROTOCOL_VERSION10";
			break;
		case PROTOCOL_VERSION11:
			protoversionstr="PROTOCOL_VERSION11";
			break;
		case PROTOCOL_VERSION12:
			protoversionstr="PROTOCOL_VERSION12";
			break;
		case PROTOCOL_VERSION13:
			protoversionstr="PROTOCOL_VERSION13";
			break;
		case PROTOCOL_VERSION14:
			protoversionstr="PROTOCOL_VERSION14";
			break;
		case PROTOCOL_VERSION15:
			protoversionstr="PROTOCOL_VERSION15";
			break;
		case PROTOCOL_VERSION16:
			protoversionstr="PROTOCOL_VERSION16";
			break;
		case PROTOCOL_VERSION17:
			protoversionstr="PROTOCOL_VERSION17";
			break;
		default:
			protoversionstr="unknown";
			break;
	}
	stdoutput.printf("	protocol version: %d (0x%04x) (%s)\n",
				protoversion,protoversion,protoversionstr);
}

void sqlrprotocol_firebird::debugProtocolType(const char *title,
						uint32_t protocoltype) {
	const char	*protocoltypestr=NULL;
	switch (protocoltype) {
		case ptype_rpc:
			protocoltypestr="ptype_rpc";
			break;
		case ptype_batch_send:
			protocoltypestr="ptype_batch_send";
			break;
		case ptype_out_of_band:
			protocoltypestr="ptype_out_of_band";
			break;
		case ptype_lazy_send:
			protocoltypestr="ptype_lazy_send";
			break;
		default:
			protocoltypestr="unknown";
			break;
	}
	stdoutput.printf("	%s: %d (%s)\n",
				title,protocoltype,protocoltypestr);
}

void sqlrprotocol_firebird::debugDpbVersion(uint32_t dpbversion) {
	const char	*dpbversionstr=NULL;
	switch (dpbversion) {
		case isc_dpb_version1:
			dpbversionstr="isc_dpb_version1";
			break;
		case isc_dpb_version2:
			dpbversionstr="isc_dpb_version2";
			break;
		default:
			dpbversionstr="unknown";
			break;
	}
	stdoutput.printf("	dpb version: %d (%s)\n",
				dpbversion,dpbversionstr);
}

void sqlrprotocol_firebird::debugDpbParam(uint32_t dpbparam) {
	const char	*dpbparamstr=NULL;
	switch (dpbparam) {
		case isc_dpb_cdd_pathname:
			dpbparamstr="isc_dpb_cdd_pathname";
			break;
		case isc_dpb_allocation:
			dpbparamstr="isc_dpb_allocation";
			break;
		case isc_dpb_journal:
			dpbparamstr="isc_dpb_journal";
			break;
		case isc_dpb_page_size:
			dpbparamstr="isc_dpb_page_size";
			break;
		case isc_dpb_num_buffers:
			dpbparamstr="isc_dpb_num_buffers";
			break;
		case isc_dpb_buffer_length:
			dpbparamstr="isc_dpb_buffer_length";
			break;
		case isc_dpb_debug:
			dpbparamstr="isc_dpb_debug";
			break;
		case isc_dpb_garbage_collect:
			dpbparamstr="isc_dpb_garbage_collect";
			break;
		case isc_dpb_verify:
			dpbparamstr="isc_dpb_verify";
			break;
		case isc_dpb_sweep:
			dpbparamstr="isc_dpb_sweep";
			break;
		case isc_dpb_enable_journal:
			dpbparamstr="isc_dpb_enable_journal";
			break;
		case isc_dpb_disable_journal:
			dpbparamstr="isc_dpb_disable_journal";
			break;
		case isc_dpb_dbkey_scope:
			dpbparamstr="isc_dpb_dbkey_scope";
			break;
		case isc_dpb_number_of_users:
			dpbparamstr="isc_dpb_number_of_users";
			break;
		case isc_dpb_trace:
			dpbparamstr="isc_dpb_trace";
			break;
		case isc_dpb_no_garbage_collect:
			dpbparamstr="isc_dpb_no_garbage_collect";
			break;
		case isc_dpb_damaged:
			dpbparamstr="isc_dpb_damaged";
			break;
		case isc_dpb_license:
			dpbparamstr="isc_dpb_license";
			break;
		case isc_dpb_sys_user_name:
			dpbparamstr="isc_dpb_sys_user_name";
			break;
		case isc_dpb_encrypt_key:
			dpbparamstr="isc_dpb_encrypt_key";
			break;
		case isc_dpb_activate_shadow:
			dpbparamstr="isc_dpb_activate_shadow";
			break;
		case isc_dpb_sweep_interval:
			dpbparamstr="isc_dpb_sweep_interval";
			break;
		case isc_dpb_delete_shadow:
			dpbparamstr="isc_dpb_delete_shadow";
			break;
		case isc_dpb_force_write:
			dpbparamstr="isc_dpb_force_write";
			break;
		case isc_dpb_begin_log:
			dpbparamstr="isc_dpb_begin_log";
			break;
		case isc_dpb_quit_log:
			dpbparamstr="isc_dpb_quit_log";
			break;
		case isc_dpb_no_reserve:
			dpbparamstr="isc_dpb_no_reserve";
			break;
		case isc_dpb_user_name:
			dpbparamstr="isc_dpb_user_name";
			break;
		case isc_dpb_password:
			dpbparamstr="isc_dpb_password";
			break;
		case isc_dpb_password_enc:
			dpbparamstr="isc_dpb_password_enc";
			break;
		case isc_dpb_sys_user_name_enc:
			dpbparamstr="isc_dpb_sys_user_name_enc";
			break;
		case isc_dpb_interp:
			dpbparamstr="isc_dpb_interp";
			break;
		case isc_dpb_online_dump:
			dpbparamstr="isc_dpb_online_dump";
			break;
		case isc_dpb_old_file_size:
			dpbparamstr="isc_dpb_old_file_size";
			break;
		case isc_dpb_old_num_files:
			dpbparamstr="isc_dpb_old_num_files";
			break;
		case isc_dpb_old_file:
			dpbparamstr="isc_dpb_old_file";
			break;
		case isc_dpb_old_start_page:
			dpbparamstr="isc_dpb_old_start_page";
			break;
		case isc_dpb_old_start_seqno:
			dpbparamstr="isc_dpb_old_start_seqno";
			break;
		case isc_dpb_old_start_file:
			dpbparamstr="isc_dpb_old_start_file";
			break;
		case isc_dpb_drop_walfile:
			dpbparamstr="isc_dpb_drop_walfile";
			break;
		case isc_dpb_old_dump_id:
			dpbparamstr="isc_dpb_old_dump_id";
			break;
		case isc_dpb_wal_backup_dir:
			dpbparamstr="isc_dpb_wal_backup_dir";
			break;
		case isc_dpb_wal_chkptlen:
			dpbparamstr="isc_dpb_wal_chkptlen";
			break;
		case isc_dpb_wal_numbufs:
			dpbparamstr="isc_dpb_wal_numbufs";
			break;
		case isc_dpb_wal_bufsize:
			dpbparamstr="isc_dpb_wal_bufsize";
			break;
		case isc_dpb_wal_grp_cmt_wait:
			dpbparamstr="isc_dpb_wal_grp_cmt_wait";
			break;
		case isc_dpb_lc_messages:
			dpbparamstr="isc_dpb_lc_messages";
			break;
		case isc_dpb_lc_ctype:
			dpbparamstr="isc_dpb_lc_ctype";
			break;
		case isc_dpb_cache_manager:
			dpbparamstr="isc_dpb_cache_manager";
			break;
		case isc_dpb_shutdown:
			dpbparamstr="isc_dpb_shutdown";
			break;
		case isc_dpb_online:
			dpbparamstr="isc_dpb_online";
			break;
		case isc_dpb_shutdown_delay:
			dpbparamstr="isc_dpb_shutdown_delay";
			break;
		case isc_dpb_reserved:
			dpbparamstr="isc_dpb_reserved";
			break;
		case isc_dpb_overwrite:
			dpbparamstr="isc_dpb_overwrite";
			break;
		case isc_dpb_sec_attach:
			dpbparamstr="isc_dpb_sec_attach";
			break;
		case isc_dpb_disable_wal:
			dpbparamstr="isc_dpb_disable_wal";
			break;
		case isc_dpb_connect_timeout:
			dpbparamstr="isc_dpb_connect_timeout";
			break;
		case isc_dpb_dummy_packet_interval:
			dpbparamstr="isc_dpb_dummy_packet_interval";
			break;
		case isc_dpb_gbak_attach:
			dpbparamstr="isc_dpb_gbak_attach";
			break;
		case isc_dpb_sql_role_name:
			dpbparamstr="isc_dpb_sql_role_name";
			break;
		case isc_dpb_set_page_buffers:
			dpbparamstr="isc_dpb_set_page_buffers";
			break;
		case isc_dpb_working_directory:
			dpbparamstr="isc_dpb_working_directory";
			break;
		case isc_dpb_sql_dialect:
			dpbparamstr="isc_dpb_sql_dialect";
			break;
		case isc_dpb_set_db_readonly:
			dpbparamstr="isc_dpb_set_db_readonly";
			break;
		case isc_dpb_set_db_sql_dialect:
			dpbparamstr="isc_dpb_set_db_sql_dialect";
			break;
		case isc_dpb_gfix_attach:
			dpbparamstr="isc_dpb_gfix_attach";
			break;
		case isc_dpb_gstat_attach:
			dpbparamstr="isc_dpb_gstat_attach";
			break;
		case isc_dpb_set_db_charset:
			dpbparamstr="isc_dpb_set_db_charset";
			break;
		case isc_dpb_gsec_attach:
			dpbparamstr="isc_dpb_gsec_attach";
			break;
		case isc_dpb_address_path:
			dpbparamstr="isc_dpb_address_path";
			break;
		case isc_dpb_process_id:
			dpbparamstr="isc_dpb_process_id";
			break;
		case isc_dpb_no_db_triggers:
			dpbparamstr="isc_dpb_no_db_triggers";
			break;
		case isc_dpb_trusted_auth:
			dpbparamstr="isc_dpb_trusted_auth";
			break;
		case isc_dpb_process_name:
			dpbparamstr="isc_dpb_process_name";
			break;
		case isc_dpb_trusted_role:
			dpbparamstr="isc_dpb_trusted_role";
			break;
		case isc_dpb_org_filename:
			dpbparamstr="isc_dpb_org_filename";
			break;
		case isc_dpb_utf8_filename:
			dpbparamstr="isc_dpb_utf8_filename";
			break;
		case isc_dpb_ext_call_depth:
			dpbparamstr="isc_dpb_ext_call_depth";
			break;
		case isc_dpb_auth_block:
			dpbparamstr="isc_dpb_auth_block";
			break;
		case isc_dpb_client_version:
			dpbparamstr="isc_dpb_client_version";
			break;
		case isc_dpb_remote_protocol:
			dpbparamstr="isc_dpb_remote_protocol";
			break;
		case isc_dpb_host_name:
			dpbparamstr="isc_dpb_host_name";
			break;
		case isc_dpb_os_user:
			dpbparamstr="isc_dpb_os_user";
			break;
		case isc_dpb_specific_auth_data:
			dpbparamstr="isc_dpb_specific_auth_data";
			break;
		case isc_dpb_auth_plugin_list:
			dpbparamstr="isc_dpb_auth_plugin_list";
			break;
		case isc_dpb_auth_plugin_name:
			dpbparamstr="isc_dpb_auth_plugin_name";
			break;
		case isc_dpb_config:
			dpbparamstr="isc_dpb_config";
			break;
		case isc_dpb_nolinger:
			dpbparamstr="isc_dpb_nolinger";
			break;
		case isc_dpb_reset_icu:
			dpbparamstr="isc_dpb_reset_icu";
			break;
		case isc_dpb_map_attach:
			dpbparamstr="isc_dpb_map_attach";
			break;
		case isc_dpb_session_time_zone:
			dpbparamstr="isc_dpb_session_time_zone";
			break;
		case isc_dpb_set_db_replica:
			dpbparamstr="isc_dpb_set_db_replica";
			break;
		case isc_dpb_set_bind:
			dpbparamstr="isc_dpb_set_bind";
			break;
		case isc_dpb_decfloat_round:
			dpbparamstr="isc_dpb_decfloat_round";
			break;
		case isc_dpb_decfloat_traps:
			dpbparamstr="isc_dpb_decfloat_traps";
			break;
		case isc_dpb_clear_map:
			dpbparamstr="isc_dpb_clear_map";
			break;
		default:
			dpbparamstr="unknown";
			break;
	}
	stdoutput.printf("	dpb param: %d (0x%02x) (%s)\n",
				dpbparam,dpbparam,dpbparamstr);
}

void sqlrprotocol_firebird::debugStatusVector(uint64_t *statusvector,
						uint8_t statusvectorlen) {
	stdoutput.write("	status vector:\n");
	uint32_t	cluster=1;
	uint8_t		i=0;
	while (i<statusvectorlen) {
		stdoutput.printf("		cluster %d:\n",cluster);
		switch (statusvector[i]) {
			case isc_arg_end:
				stdoutput.write("			"
						"code: isc_arg_end\n");
				i++;
				break;
			case isc_arg_gds:
				stdoutput.write("			"
						"code: isc_arg_gds\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_string:
				stdoutput.write("			"
						"code: isc_arg_string\n");
				i++;
				stdoutput.printf("			"
						"address: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_cstring:
				stdoutput.write("			"
						"code: isc_arg_cstring\n");
				i++;
				stdoutput.printf("			"
						"length: %lld\n",
						statusvector[i]);
				i++;
				stdoutput.printf("			"
						"address: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_number:
				stdoutput.write("			"
						"code: isc_arg_number\n");
				i++;
				stdoutput.printf("			"
						"number: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_interpreted:
				stdoutput.write("			"
						"code: isc_arg_interpreted\n");
				i++;
				stdoutput.printf("			"
						"address: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_vms:
				stdoutput.write("			"
						"code: isc_arg_vms\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_unix:
				stdoutput.write("			"
						"code: isc_arg_unix\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_domain:
				stdoutput.write("			"
						"code: isc_arg_domain\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_dos:
				stdoutput.write("			"
						"code: isc_arg_dos\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_mpexl:
				stdoutput.write("			"
						"code: isc_arg_mpexl\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_mpexl_ipc:
				stdoutput.write("			"
						"code: isc_arg_mpexl_ipc\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_mach:
				stdoutput.write("			"
						"code: isc_arg_mach\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_netware:
				stdoutput.write("			"
						"code: isc_arg_netware\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_win32:
				stdoutput.write("			"
						"code: isc_arg_win32\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_warning:
				stdoutput.write("			"
						"code: isc_arg_warning\n");
				i++;
				stdoutput.printf("			"
						"warning: %lld\n",
						statusvector[i]);
				i++;
				break;
			case isc_arg_sql_state:
				stdoutput.write("			"
						"code: isc_arg_sql_state\n");
				i++;
				stdoutput.printf("			"
						"sql state: %lld\n",
						statusvector[i]);
				i++;
				break;
			default:
				stdoutput.write("			"
						"code: unknown\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",
						statusvector[i]);
				i++;
				break;
		}
		cluster++;
	}
	stdoutput.printf("\n");
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_firebird(
						sqlrservercontroller *cont,
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_firebird(cont,ps,parameters);
	}
}
