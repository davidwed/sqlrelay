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
// 10 supports warnings, removes requirement for encoding/decoding status codes
#define PROTOCOL_VERSION10	10
// 11 supports user-auth-related operations
#define PROTOCOL_VERSION11	(0x8000|11)
// 12 supports asynchronous calls
#define PROTOCOL_VERSION12	(0x8000|12)
// 13 supports auth plugins
#define PROTOCOL_VERSION13	(0x8000|13)
// 14 bug-fix
#define PROTOCOL_VERSION14	(0x8000|14)
// 15 supports crypt key callback during connect
#define PROTOCOL_VERSION15	(0x8000|15)
// 16 supports statement timeouts
#define PROTOCOL_VERSION16	(0x8000|16)
// 17 supports op_batch_sync, op_batch_info
#define PROTOCOL_VERSION17	(0x8000|17)

// ptype codes
#define ptype_rpc		2
#define ptype_batch_send	3

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
		void	debugOpCode(uint32_t opcode);
		void	debugArchType(uint32_t archtype);
		void	debugProtocolVersion(uint32_t protoversion);
		void	debugProtocolType(const char *title,
						uint32_t protocoltype);
		bool	connectResponse();
		bool	attach();
		bool	attachResponse();
	
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

		void	debugSystemError();
		void	keepReading();

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		filedescriptor	*clientsock;

		uint32_t	opcode;
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
}

void sqlrprotocol_firebird::free() {
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

	if (connectversion==2) {
		// ...
	} else if (connectversion==3) {
		// ...
	} else {
		// FIXME: not supported
	}

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
	char	*db=new char[dblen+1];
	if (clientsock->read(db,dblen)!=dblen) {
		if (getDebug()) {
			stdoutput.write("	read db path failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db: %s\n",db);
	}

if (connectversion==3) {
	// FIXME: connect version 3 is somehow different here...
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
	byte_t	*userid=new byte_t[useridlen+1];
	if (clientsock->read(userid,useridlen)!=useridlen) {
		if (getDebug()) {
			stdoutput.write("	read user id failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	user id: %s\n",userid);
	}

	// get protocols...
	for (uint32_t i=0; i<protocount; i++) {

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

		// FIXME: decide on protocol...
	}
	debugEnd();

	return true;
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
	stdoutput.printf("	protocol version: %d (%s)\n",
						protoversion,protoversionstr);
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
		default:
			protocoltypestr="unknown";
			break;
	}
	stdoutput.printf("	%s: %d (%s)\n",
				title,protocoltype,protocoltypestr);
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

	// FIXME: we might not want to accept...
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

	// FIXME: this should depend on offered protocols...
	uint32_t	protoversionnumber=0;
	if (clientsock->write(protoversionnumber)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	write protocol "
						"version numberfailed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	protocol version number: %d\n",
							protoversionnumber);
	}

	// FIXME: set this to our arch?
	uint32_t	archtype=arch_linux;
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

	// FIXME: do we support batch send?
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
	// 	byte_t[]	db parameters
	// 	...
	// }

	debugStart("attach");

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
	char	*db=new char[dblen+1];
	if (clientsock->read(db,dblen)!=dblen) {
		if (getDebug()) {
			stdoutput.write("	read db path failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db: %s\n",db);
	}

	// get db parameters
	uint32_t	dbplen=0;
	if (clientsock->read(&dbplen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.write("	read db parameters "
							"length failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db params len: %d\n",dbplen);
	}
	char	*dbp=new char[dbplen+1];
	if (clientsock->read(dbp,dbplen)!=dbplen) {
		if (getDebug()) {
			stdoutput.write("	read db params failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	db params:\n%s\n",dbp);
	}

	debugEnd();

	return true;
}

bool sqlrprotocol_firebird::attachResponse() {
	return false;
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
		stdoutput.printf("	auth %s\n",(retval)?"success":"failed");
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

void sqlrprotocol_firebird::debugSystemError() {
	char	*err=error::getErrorString();
	stdoutput.printf("%s\n",err);
	delete[] err;
}

void sqlrprotocol_firebird::keepReading() {
	for (;;) {
		byte_t	buffer[1024];
		ssize_t	r=clientsock->read(&buffer,1024,1,0);
		stdoutput.printf("read %d more bytes...\n",r);
		if (r<1) {
			break;
		}
		debugHexDump(buffer,r);
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_firebird(
						sqlrservercontroller *cont,
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_firebird(cont,ps,parameters);
	}
}
