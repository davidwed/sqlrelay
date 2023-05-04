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

// common structural codes
#define isc_info_end			1
#define isc_info_truncated		2
#define isc_info_error			3
#define isc_info_data_not_ready		4
#define isc_info_length			126
#define isc_info_flag_end		127

// db information items
#define isc_info_db_id			4
#define isc_info_reads			5
#define isc_info_writes			6
#define isc_info_fetches		7
#define isc_info_marks			8
#define isc_info_implementation		11
#define isc_info_isc_version		12
#define isc_info_base_level		13
#define isc_info_page_size		14
#define isc_info_num_buffers		15
#define isc_info_limbo			16
#define isc_info_current_memory		17
#define isc_info_max_memory		18
#define isc_info_window_turns		19
#define isc_info_license		20
#define isc_info_allocation		21
#define isc_info_attachment_id		22
#define isc_info_read_seq_count		23
#define isc_info_read_idx_count		24
#define isc_info_insert_count		25
#define isc_info_update_count		26
#define isc_info_delete_count		27
#define isc_info_backout_count		28
#define isc_info_purge_count		29
#define isc_info_expunge_count		30
#define isc_info_sweep_interval		31
#define isc_info_ods_version		32
#define isc_info_ods_minor_version	33
#define isc_info_no_reserve		34
#define isc_info_logfile		35
#define isc_info_cur_logfile_name	36
#define isc_info_cur_log_part_offset	37
#define isc_info_num_wal_buffers	38
#define isc_info_wal_buffer_size	39
#define isc_info_wal_ckpt_length	40
#define isc_info_wal_cur_ckpt_interval	41
#define isc_info_wal_prv_ckpt_fname	42
#define isc_info_wal_prv_ckpt_poffset	43
#define isc_info_wal_recv_ckpt_fname	44
#define isc_info_wal_recv_ckpt_poffset	45
#define isc_info_wal_grpc_wait_usecs	47
#define isc_info_wal_num_io		48
#define isc_info_wal_avg_io_size	49
#define isc_info_wal_num_commits	50
#define isc_info_wal_avg_grpc_size	51
#define isc_info_forced_writes		52
#define isc_info_user_names		53
#define isc_info_page_errors		54
#define isc_info_record_errors		55
#define isc_info_bpage_errors		56
#define isc_info_dpage_errors		57
#define isc_info_ipage_errors		58
#define isc_info_ppage_errors		59
#define isc_info_tpage_errors		60
#define isc_info_set_page_buffers	61
#define isc_info_db_sql_dialect		62
#define isc_info_db_read_only		63
#define isc_info_db_size_in_pages	64
#define frb_info_att_charset		101
#define isc_info_db_class		102
#define isc_info_firebird_version	103
#define isc_info_oldest_transaction	104
#define isc_info_oldest_active		105
#define isc_info_oldest_snapshot	106
#define isc_info_next_transaction	107
#define isc_info_db_provider		108
#define isc_info_active_transactions	109
#define isc_info_active_tran_count	110
#define isc_info_creation_date		111
#define isc_info_db_file_size		112
#define fb_info_page_contents		113

// transaction parameters
#define isc_tpb_version1                  1
#define isc_tpb_version3                  3
#define isc_tpb_consistency               1
#define isc_tpb_concurrency               2
#define isc_tpb_shared                    3
#define isc_tpb_protected                 4
#define isc_tpb_exclusive                 5
#define isc_tpb_wait                      6
#define isc_tpb_nowait                    7
#define isc_tpb_read                      8
#define isc_tpb_write                     9
#define isc_tpb_lock_read                 10
#define isc_tpb_lock_write                11
#define isc_tpb_verb_time                 12
#define isc_tpb_commit_time               13
#define isc_tpb_ignore_limbo              14
#define isc_tpb_read_committed	          15
#define isc_tpb_autocommit                16
#define isc_tpb_rec_version               17
#define isc_tpb_no_rec_version            18
#define isc_tpb_restart_requests          19
#define isc_tpb_no_auto_undo              20
#define isc_tpb_lock_timeout              21

// free statement flags
#define DSQL_close	1
#define DSQL_drop	2

// sql information items
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
		bool	attach();

		bool	genericResponse(const char *title,
						uint32_t objecthandle,
						uint32_t objectid,
						const byte_t *buffer,
						uint32_t bufferlen,
						uint64_t *sv,
						uint8_t svlen);
		bool	genericResponse(const char *title,
						uint32_t objecthandle,
						uint32_t objectid,
						bool padding,
						const byte_t *buffer,
						uint32_t bufferlen,
						uint64_t *sv,
						uint8_t svlen);
	
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

		void	readStringFromBuffer(const byte_t *in,
						const char *name,
						char **buf,
						const byte_t **out);

		bool	readInt(uint32_t *val,
					const char *name,
					uint32_t *bytesread);
		bool	readInt(uint32_t *val,
					const char *name,
					uint32_t expected,
					uint32_t *bytesread);
		bool	readString(char **val,
					const char *name,
					uint32_t *bytesread);
		bool	readString(char **val,
					uint32_t *len,
					const char *name,
					uint32_t *bytesread);
		bool	readBuffer(byte_t **val,
					const char *name,
					uint32_t *bytesread);
		bool	readBuffer(byte_t **val,
					uint32_t *len,
					const char *name,
					uint32_t *bytesread);
		bool	readPadding(uint32_t *bytesread);

		bool	writeInt(uint32_t val,
					const char *name,
					uint32_t *byteswritten);
		bool	writeBuffer(const byte_t *val,
					uint32_t len,
					const char *name,
					uint32_t *byteswritten);

		void	debugSystemError();
		void	debugOpCode(const char *name, uint32_t opcode);
		void	debugArchType(uint32_t archtype);
		void	debugProtocolVersion(uint32_t protoversion);
		void	debugProtocolType(const char *title,
						uint32_t protocoltype);
		void	debugDpbVersion(byte_t dpbversion);
		void	debugDpbParam(byte_t dpbparam);
		void	debugDbInfoItem(byte_t dbinfoitem);
		void	debugTpbVersion(byte_t tpbversion);
		void	debugTpbParam(byte_t tpbparam);
		void	debugStatusVector(uint64_t *sv, uint8_t svlen);

		uint32_t	maxquerysize;
		uint16_t	maxbindcount;

		filedescriptor	*clientsock;

		uint32_t	opcode;

		char		*db;
		char		*username;
		char		*password;
		char		*wd;
		uint32_t	dbhandle;

		uint64_t	statusvector[20];
		uint8_t		statusvectorlen;

		bytebuffer	respbuffer;
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
	dbhandle=0;
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
	clientsock->setTranslateByteOrder(true);
	clientsock->setNaglesAlgorithmEnabled(false);
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
	return connect() && attach();
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
	if (!readInt(&opcode,"connect op code",op_connect,&bytesread)) {
		return false;
	}
	debugOpCode("connect op code",opcode);

	// get op_attach
	if (!readInt(&opcode,"attach op code",op_attach,&bytesread)) {
		return false;
	}
	debugOpCode("attach op code",opcode);

	// get connect version
	uint32_t	connectversion=0;
	if (!readInt(&connectversion,"connect version",&bytesread)) {
		return false;
	}

	// get arch type
	uint32_t	archtype=0;
	if (!readInt(&archtype,"arch type",&bytesread)) {
		return false;
	}
	debugArchType(archtype);

	// get db
	if (!readString(&db,"db",&bytesread)) {
		return false;
	}

	// get protocol count
	uint32_t	protocount=0;
	if (!readInt(&protocount,"protocol count",&bytesread)) {
		return false;
	}

	// get user identification
	byte_t	*userid;
	if (!readBuffer(&userid,"user id",&bytesread)) {
		return false;
	}
	// FIXME: parse userid

	// get protocols...
	for (uint32_t i=0; i<protocount; i++) {

		stdoutput.printf("	protocol %d...\n",i);

		// get protocol version
		uint32_t	protoversion=0;
		if (!readInt(&protoversion,"protocol version",&bytesread)) {
			return false;
		}
		debugProtocolVersion(protoversion);

		// get arch type
		uint32_t	archtype=0;
		if (!readInt(&archtype,"arch type",&bytesread)) {
			return false;
		}
		debugArchType(archtype);

		// get minimum type
		uint32_t	mintype=0;
		if (!readInt(&mintype,"min type",&bytesread)) {
			return false;
		}
		debugProtocolType("min type",mintype);

		// get maximum type
		uint32_t	maxtype=0;
		if (!readInt(&maxtype,"max type",&bytesread)) {
			return false;
		}
		debugProtocolType("max type",maxtype);

		// get preference weight
		uint32_t	prefwt=0;
		if (!readInt(&prefwt,"preference weight",&bytesread)) {
			return false;
		}
	}

	debugEnd();

	// FIXME: decide whether to accept to connection or not...

	// FIXME: decide which protocol to use...

	// response packet data structure:
	//
	// data {
	// 	int32_t		op_accept
	// 	int32_t		arch type
	// 	int32_t		minimum type
	// }

	debugStart("connect response");

	uint32_t	byteswritten=0;

	opcode=op_accept;
	if (!writeInt(opcode,"accept op code",&byteswritten)) {
		return false;
	}
	debugOpCode("accept op code",opcode);

	// FIXME: determine this somehow
	uint32_t	protoversion=PROTOCOL_VERSION10;
	if (!writeInt(protoversion,"protocol version",&byteswritten)) {
		return false;
	}
	debugProtocolVersion(protoversion);

	// FIXME: determine this somehow
	archtype=arch_generic;
	if (!writeInt(archtype,"arch type",&byteswritten)) {
		return false;
	}
	debugArchType(archtype);

	// FIXME: determine this somehow
	uint32_t	mintype=ptype_rpc;
	if (!writeInt(mintype,"min type",&byteswritten)) {
		return false;
	}
	debugProtocolType("min type",mintype);

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
	if (!readInt(&opcode,"attach op code",op_attach,&bytesread)) {
		return false;
	}
	debugOpCode("attach op code",opcode);

	// get db object id
	uint32_t	dbobjectid=0;
	if (!readInt(&dbobjectid,"db object id",&bytesread)) {
		return false;
	}

	// get db (override db from connect)
	delete[] db;
	if (!readString(&db,"db",&bytesread)) {
		return false;
	}

	// get db parameters buffer
	uint32_t	dpblen;
	byte_t		*dpb;
	if (!readBuffer(&dpb,&dpblen,"db param buffer",&bytesread)) {
		return false;
	}

	// process db parameters buffer...
	const byte_t	*dpbptr=dpb;
	const byte_t	*dpbendptr=dpb+dpblen;

	// get the dpb version
	if (dpbptr) {
		byte_t		dpbversion;
		read(dpbptr,&dpbversion,&dpbptr);
		debugDpbVersion(dpbversion);
		// FIXME: do something with this...
	}

	// get each parameter...
	while (dpbptr!=dpbendptr) {
		
		// get the parameter
		byte_t	dpbparam;
		read(dpbptr,&dpbparam,&dpbptr);
		debugDpbParam(dpbparam);

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
				readStringFromBuffer(
					dpbptr,"user name",&username,&dpbptr);
				break;

			case isc_dpb_password:
				// FIXME: do something...
				break;

			case isc_dpb_password_enc:
				readStringFromBuffer(
					dpbptr,"password",&password,&dpbptr);
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
				readStringFromBuffer(
					dpbptr,"val",&val,&dpbptr);
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
				readStringFromBuffer(
					dpbptr,"working directory",&wd,&dpbptr);
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

	// clean up
	delete[] dpb;

	// FIXME: object handle should be the database handle ???
	// FIXME: no idea what the database handle is
	uint32_t	objecthandle=0;

	// FIXME: no idea what the object id is
	uint32_t	objectid=0;

	// status vector...
	bytestring::zero(statusvector,sizeof(statusvector));
	// interbase error...
	statusvector[0]=isc_arg_gds;
	// no error...
	statusvector[1]=0;
	statusvectorlen=2;

	return genericResponse("attach response",
				objecthandle,objectid,
				NULL,0,
				statusvector,statusvectorlen);
}

bool sqlrprotocol_firebird::genericResponse(const char *title,
						uint32_t objecthandle,
						uint32_t objectid,
						const byte_t *buffer,
						uint32_t bufferlen,
						uint64_t *sv,
						uint8_t svlen) {
	return genericResponse(title,objecthandle,objectid,false,
						buffer,bufferlen,sv,svlen);
}

bool sqlrprotocol_firebird::genericResponse(const char *title,
						uint32_t objecthandle,
						uint32_t objectid,
						bool padding,
						const byte_t *buffer,
						uint32_t bufferlen,
						uint64_t *sv,
						uint8_t svlen) {

	// response packet data structure:
	//
	// data {
	// 	int32_t		op_response
	// 	int32_t		object handle
	// 	int32_t		object id
	// 	int32_t		padding sometimes???
	// 	int32_t		buffer length
	// 	byte_t[]	buffer
	// 	byte_t[]	status vector
	// }

	debugStart(title);

	uint32_t	byteswritten=0;

	// write the opcode
	opcode=op_response;
	if (!writeInt(opcode,"response op code",&byteswritten)) {
		return false;
	}
	debugOpCode("response op code",opcode);

	// write the object handle
	if (!writeInt(objecthandle,"object handle",&byteswritten)) {
		return false;
	}

	// write the object id
	if (!writeInt(objectid,"object id",&byteswritten)) {
		return false;
	}

	if (padding) {
		if (!writeInt(0,"padding",&byteswritten)) {
			return false;
		}
	}

	// write the buffer
	if (!writeBuffer(buffer,bufferlen,"buffer",&byteswritten)) {
		return false;
	}

	// write the status vector
	for (uint8_t i=0; i<svlen; i++) {
		if (clientsock->write(sv[i])!=sizeof(uint64_t)) {
			if (getDebug()) {
				stdoutput.printf("	write status "
						"vector [%d] failed\n",i);
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		byteswritten+=sizeof(uint64_t);
	}
	if (getDebug()) {
		debugStatusVector(sv,svlen);
	}
	// FIXME: write padding?

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
	cred.setPasswordLength(charstring::getLength(password));
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

	uint32_t	bytesread=0;
	
	if (!readInt(&opcode,"op code",&bytesread)) {
		return false;
	}
	debugOpCode("op code",opcode);
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

	// request packet data structure:
	//
	// data {
	// 	int32_t		db handle
	// 	int32_t		object id
	// 	int32_t		requested db info items length
	// 	byte_t[]	requested db info items
	// 	int32_t		response buffer length
	// }

	debugStart("info database");

	uint32_t	bytesread=0;

	// get database handle
	uint32_t	clientdbhandle;
	if (!readInt(&clientdbhandle,"db handle",&bytesread)) {
		return false;
	}

	// get object id
	uint32_t	objectid;
	if (!readInt(&objectid,"object id",&bytesread)) {
		return false;
	}

	// get requested db info items
	uint32_t	dbinfolen;
	byte_t		*dbinfo;
	if (!readBuffer(&dbinfo,&dbinfolen,
			"requested db info items",&bytesread)) {
		return false;
	}

	// get response buffer length
	uint32_t	respbufferlen;
	if (!readInt(&respbufferlen,"response buffer length",&bytesread)) {
		return false;
	}

	// process requested db info items
	const byte_t	*dbinfoptr=dbinfo;
	const byte_t	*dbinfoendptr=dbinfo+dbinfolen;

	// build response buffer...
	respbuffer.clear();
	while (dbinfoptr!=dbinfoendptr) {
		
		// get the requetted db info item
		byte_t	dbinfoitem;
		read(dbinfoptr,&dbinfoitem,&dbinfoptr);
		debugDbInfoItem(dbinfoitem);

		// append the db info item
		respbuffer.append(dbinfoitem);

		switch (dbinfoitem) {
			case isc_info_end:
				// there might be multiple of these, but if we
				// hit one of them then just append one and bail
				dbinfoptr=dbinfoendptr;
				break;

			case isc_info_db_id:
				// FIXME: do something
				break;

			case isc_info_reads:
				// FIXME: do something
				break;

			case isc_info_writes:
				// FIXME: do something
				break;

			case isc_info_fetches:
				// FIXME: do something
				break;

			case isc_info_marks:
				// FIXME: do something
				break;

			case isc_info_implementation:
				// FIXME: do something
				break;

			case isc_info_isc_version:
				// FIXME: do something
				break;

			case isc_info_base_level:
				// FIXME: do something
				break;

			case isc_info_page_size:
				// FIXME: do something
				break;

			case isc_info_num_buffers:
				// FIXME: do something
				break;

			case isc_info_limbo:
				// FIXME: do something
				break;

			case isc_info_current_memory:
				// FIXME: do something
				break;

			case isc_info_max_memory:
				// FIXME: do something
				break;

			case isc_info_window_turns:
				// FIXME: do something
				break;

			case isc_info_license:
				// FIXME: do something
				break;

			case isc_info_allocation:
				// FIXME: do something
				break;

			case isc_info_attachment_id:
				// FIXME: do something
				break;

			case isc_info_read_seq_count:
				// FIXME: do something
				break;

			case isc_info_read_idx_count:
				// FIXME: do something
				break;

			case isc_info_insert_count:
				// FIXME: do something
				break;

			case isc_info_update_count:
				// FIXME: do something
				break;

			case isc_info_delete_count:
				// FIXME: do something
				break;

			case isc_info_backout_count:
				// FIXME: do something
				break;

			case isc_info_purge_count:
				// FIXME: do something
				break;

			case isc_info_expunge_count:
				// FIXME: do something
				break;

			case isc_info_sweep_interval:
				// FIXME: do something
				break;

			case isc_info_ods_version:
				respbuffer.append((byte_t)4);
				// FIXME: ???
				respbuffer.append((byte_t)0);
				// FIXME: ???
				respbuffer.append((byte_t)0x0c);
				respbuffer.append((byte_t)0);
				respbuffer.append((byte_t)0);
				respbuffer.append((byte_t)0);
				break;

			case isc_info_ods_minor_version:
				respbuffer.append((byte_t)4);
				// FIXME: ???
				respbuffer.append((byte_t)0);
				// FIXME: ???
				respbuffer.append((byte_t)0);
				respbuffer.append((byte_t)0);
				respbuffer.append((byte_t)0);
				respbuffer.append((byte_t)0);
				break;

			case isc_info_no_reserve:
				// FIXME: do something
				break;

			case isc_info_logfile:
				// FIXME: do something
				break;

			case isc_info_cur_logfile_name:
				// FIXME: do something
				break;

			case isc_info_cur_log_part_offset:
				// FIXME: do something
				break;

			case isc_info_num_wal_buffers:
				// FIXME: do something
				break;

			case isc_info_wal_buffer_size:
				// FIXME: do something
				break;

			case isc_info_wal_ckpt_length:
				// FIXME: do something
				break;

			case isc_info_wal_cur_ckpt_interval:
				// FIXME: do something
				break;

			case isc_info_wal_prv_ckpt_fname:
				// FIXME: do something
				break;

			case isc_info_wal_prv_ckpt_poffset:
				// FIXME: do something
				break;

			case isc_info_wal_recv_ckpt_fname:
				// FIXME: do something
				break;

			case isc_info_wal_recv_ckpt_poffset:
				// FIXME: do something
				break;

			case isc_info_wal_grpc_wait_usecs:
				// FIXME: do something
				break;

			case isc_info_wal_num_io:
				// FIXME: do something
				break;

			case isc_info_wal_avg_io_size:
				// FIXME: do something
				break;

			case isc_info_wal_num_commits:
				// FIXME: do something
				break;

			case isc_info_wal_avg_grpc_size:
				// FIXME: do something
				break;

			case isc_info_forced_writes:
				// FIXME: do something
				break;

			case isc_info_user_names:
				// FIXME: do something
				break;

			case isc_info_page_errors:
				// FIXME: do something
				break;

			case isc_info_record_errors:
				// FIXME: do something
				break;

			case isc_info_bpage_errors:
				// FIXME: do something
				break;

			case isc_info_dpage_errors:
				// FIXME: do something
				break;

			case isc_info_ipage_errors:
				// FIXME: do something
				break;

			case isc_info_ppage_errors:
				// FIXME: do something
				break;

			case isc_info_tpage_errors:
				// FIXME: do something
				break;

			case isc_info_set_page_buffers:
				// FIXME: do something
				break;

			case isc_info_db_sql_dialect:
				respbuffer.append((byte_t)1);
				// FIXME: ???
				respbuffer.append((byte_t)0);
				// FIXME: ???
				respbuffer.append((byte_t)3);
				break;

			case isc_info_db_read_only:
				// FIXME: do something
				break;

			case isc_info_db_size_in_pages:
				// FIXME: do something
				break;

			case frb_info_att_charset:
				// FIXME: do something
				break;

			case isc_info_db_class:
				// FIXME: do something
				break;

			case isc_info_firebird_version:
				// FIXME: do something
				break;

			case isc_info_oldest_transaction:
				// FIXME: do something
				break;

			case isc_info_oldest_active:
				// FIXME: do something
				break;

			case isc_info_oldest_snapshot:
				// FIXME: do something
				break;

			case isc_info_next_transaction:
				// FIXME: do something
				break;

			case isc_info_db_provider:
				// FIXME: do something
				break;

			case isc_info_active_transactions:
				// FIXME: do something
				break;

			case isc_info_active_tran_count:
				// FIXME: do something
				break;

			case isc_info_creation_date:
				// FIXME: do something
				break;

			case isc_info_db_file_size:
				// FIXME: do something
				break;

			case fb_info_page_contents:
				// FIXME: do something
				break;

			default:
				// FIXME: do something
				break;
		}
	}

	// FIXME: handle cases where
	// respbuffer.getSize() > response buffer length

	debugEnd();

	// clean up
	delete[] dbinfo;

	// response packet data structure:
	//
	// data {
	// 	int32_t		op_response
	// 	int32_t		object handle
	// 	int32_t		object id
	// 	int32_t		padding???
	// 	int32_t		response buffer length
	// 	byte_t[]	response buffer
	// 	byte_t		1 ???
	// 	int32_t		0 ???
	// 	byte_t		1 ???
	// 	int32_t		0 ???
	// }

	debugStart("info database response");

	uint32_t	byteswritten=0;

	// write the opcode
	opcode=op_response;
	if (!writeInt(opcode,"response op code",&byteswritten)) {
		return false;
	}
	debugOpCode("response op code",opcode);

	// write the db handle
	if (!writeInt(dbhandle,"db handle",&byteswritten)) {
		return false;
	}

	// write the object id
	if (!writeInt(objectid,"object id",&byteswritten)) {
		return false;
	}

	// write the padding, or whatever this is
	if (!writeInt(0,"padding",&byteswritten)) {
		return false;
	}

	// write the response buffer
	if (!writeBuffer(respbuffer.getBuffer(),
				respbuffer.getSize(),
				"response buffer",
				&byteswritten)) {
		return false;
	}

	// write whatever this is
	byte_t	trailer[]={0,0,0,0,1,0,0,0,0,0,0,0,0};
	if (!clientsock->write(trailer,sizeof(trailer))) {
		if (getDebug()) {
			stdoutput.write("	write trailer failed\n");
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.write("	trailer:\n");
		stdoutput.printHex(trailer,sizeof(trailer));
	}

	debugEnd();

	clientsock->flushWriteBuffer(-1,-1);

	return true;
}

bool sqlrprotocol_firebird::disconnect() {

	// no response packet

	debugStart("disconnect");
	debugEnd();

	// return false on purpose here
	return false;
}

bool sqlrprotocol_firebird::transaction() {

	// request packet data structure:
	//
	// data {
	// 	int32_t		db handle
	// 	int32_t		tx parameters buffer length
	// 	byte_t[]	tx parameters buffer
	// }

	debugStart("transaction");

	uint32_t	bytesread=0;

	// get database handle
	uint32_t	clientdbhandle;
	if (!readInt(&clientdbhandle,"db handle",&bytesread)) {
		return false;
	}

	// get tx parameters buffer
	uint32_t	tpblen;
	byte_t		*tpb;
	if (!readBuffer(&tpb,&tpblen,"tx param buffer",&bytesread)) {
		return false;
	}

	// process tx parameters buffer...
	const byte_t	*tpbptr=tpb;
	const byte_t	*tpbendptr=tpb+tpblen;

	// get the tpb version
	if (tpbptr) {
		byte_t		tpbversion;
		read(tpbptr,&tpbversion,&tpbptr);
		debugTpbVersion(tpbversion);
		// FIXME: do something with this...
	}

	// get each parameter...
	while (tpbptr!=tpbendptr) {
		
		// get the parameter
		byte_t	tpbparam;
		read(tpbptr,&tpbparam,&tpbptr);
		debugTpbParam(tpbparam);

		// process the parameter...
		switch (tpbparam) {
			case isc_tpb_consistency:
				// FIXME: do something
				break;

			case isc_tpb_concurrency:
				// FIXME: do something
				break;

			case isc_tpb_shared:
				// FIXME: do something
				break;

			case isc_tpb_protected:
				// FIXME: do something
				break;

			case isc_tpb_exclusive:
				// FIXME: do something
				break;

			case isc_tpb_wait:
				// FIXME: do something
				break;

			case isc_tpb_nowait:
				// FIXME: do something
				break;

			case isc_tpb_read:
				// FIXME: do something
				break;

			case isc_tpb_write:
				// FIXME: do something
				break;

			case isc_tpb_lock_read:
				// FIXME: do something
				break;

			case isc_tpb_lock_write:
				// FIXME: do something
				break;

			case isc_tpb_verb_time:
				// FIXME: do something
				break;

			case isc_tpb_commit_time:
				// FIXME: do something
				break;

			case isc_tpb_ignore_limbo:
				// FIXME: do something
				break;

			case isc_tpb_read_committed:
				// FIXME: do something
				break;

			case isc_tpb_autocommit:
				// FIXME: do something
				break;

			case isc_tpb_rec_version:
				// FIXME: do something
				break;

			case isc_tpb_no_rec_version:
				// FIXME: do something
				break;

			case isc_tpb_restart_requests:
				// FIXME: do something
				break;

			case isc_tpb_no_auto_undo:
				// FIXME: do something
				break;

			case isc_tpb_lock_timeout:
				// FIXME: do something
				break;

			default:
				// FIXME: do something
				break;
		}
	}

	debugEnd();

	// clean up
	delete[] tpb;

	// increment the dbhandle, apparently???
	dbhandle++;

	// FIXME: object id should be the transaction handle???
	uint32_t	objectid=0;

	// status vector...
	bytestring::zero(statusvector,sizeof(statusvector));
	// interbase error...
	statusvector[0]=isc_arg_gds;
	// no error...
	statusvector[1]=0;
	statusvectorlen=2;

	return genericResponse("transaction response",
				dbhandle,objectid,
				NULL,0,
				statusvector,statusvectorlen);
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

void sqlrprotocol_firebird::readStringFromBuffer(const byte_t *in,
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

bool sqlrprotocol_firebird::readInt(uint32_t *val,
					const char *name,
					uint32_t *bytesread) {
	
	if (clientsock->read(val)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.printf("	read %s failed\n",name);
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	(*bytesread)+=sizeof(uint32_t);
	if (getDebug()) {
		stdoutput.printf("	%s: %d\n",name,*val);
	}
	return true;
}

bool sqlrprotocol_firebird::readInt(uint32_t *val,
					const char *name,
					uint32_t expected,
					uint32_t *bytesread) {

	if (!readInt(val,name,bytesread)) {
		return false;
	}
	if (*val!=expected) {
		if (getDebug()) {
			stdoutput.printf("	invalid %s - "
						"got %d, expected %d\n",
						name,*val,expected);
			debugEnd();
		}
		return false;
	}
	return true;
}

bool sqlrprotocol_firebird::readString(char **val,
					const char *name,
					uint32_t *bytesread) {
	return readString(val,NULL,name,bytesread);
}

bool sqlrprotocol_firebird::readString(char **val,
					uint32_t *len,
					const char *name,
					uint32_t *bytesread) {

	// read length
	uint32_t	vallen=0;
	if (clientsock->read(&vallen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.printf("	read %s length failed\n",name);
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	%s length: %d\n",name,vallen);
	}
	(*bytesread)+=sizeof(uint32_t);

	// init buffer
	*val=NULL;

	if (vallen) {

		// allocate buffer
		*val=new char[vallen+1];

		// read buffer
		if (clientsock->read(*val,vallen)!=(ssize_t)vallen) {
			if (getDebug()) {
				stdoutput.printf("	read %s failed\n",name);
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		(*val)[vallen]='\0';
		if (getDebug()) {
			stdoutput.printf("	%s: %s\n",name,*val);
		}
		(*bytesread)+=vallen;
	}

	if (len) {
		*len=vallen;
	}

	// read padding
	return readPadding(bytesread);
}

bool sqlrprotocol_firebird::readBuffer(byte_t **val,
					const char *name,
					uint32_t *bytesread) {
	return readBuffer(val,NULL,name,bytesread);
}

bool sqlrprotocol_firebird::readBuffer(byte_t **val,
					uint32_t *len,
					const char *name,
					uint32_t *bytesread) {

	// read length
	uint32_t	vallen=0;
	if (clientsock->read(&vallen)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.printf("	read %s length failed\n",name);
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	%s length: %d\n",name,vallen);
	}
	(*bytesread)+=sizeof(uint32_t);

	// init buffer
	*val=NULL;

	if (vallen) {

		// allocate buffer
		*val=new byte_t[vallen];

		// read buffer
		if (clientsock->read(*val,vallen)!=(ssize_t)vallen) {
			if (getDebug()) {
				stdoutput.printf("	read %s failed\n",name);
				debugSystemError();
				debugEnd();
			}
			return false;
		}
		if (getDebug()) {
			stdoutput.printf("	%s:\n",name);
			stdoutput.printHex(*val,vallen);
		}
		(*bytesread)+=vallen;
	}

	if (len) {
		*len=vallen;
	}

	// read padding
	return readPadding(bytesread);
}

bool sqlrprotocol_firebird::readPadding(uint32_t *bytesread) {

	// handle degenerate case
	if (!(*bytesread%4)) {
		if (getDebug()) {
			stdoutput.write("	(0 bytes of padding)\n");
		}
		return true;
	}

	// how much padding do we need to read?
	// (pad to a 4-byte boundary)
	uint32_t	pad=((((*bytesread)/4)+1)*4)-(*bytesread);

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
		(*bytesread)++;
	}
	if (getDebug()) {
		stdoutput.printf("	(%d bytes of padding)\n",pad);
	}
	return true;
}

bool sqlrprotocol_firebird::writeInt(uint32_t val,
					const char *name,
					uint32_t *byteswritten) {
	
	if (clientsock->write(val)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.printf("	write %s failed\n",name);
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	(*byteswritten)+=sizeof(uint32_t);
	if (getDebug()) {
		stdoutput.printf("	%s: %d\n",name,val);
	}
	return true;
}

bool sqlrprotocol_firebird::writeBuffer(const byte_t *val,
					uint32_t len,
					const char *name,
					uint32_t *byteswritten) {

	// write length
	if (clientsock->write(len)!=sizeof(uint32_t)) {
		if (getDebug()) {
			stdoutput.printf("	write %s length failed\n",name);
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	%s len: %d\n",name,len);
	}

	// write buffer
	if (clientsock->write(val,len)!=(ssize_t)len) {
		if (getDebug()) {
			stdoutput.printf("	write %s failed\n",name);
			debugSystemError();
			debugEnd();
		}
		return false;
	}
	if (getDebug()) {
		stdoutput.printf("	%s:\n",name);
		stdoutput.printHex(val,len);
	}

	// FIXME: write padding?
	return true;
}

void sqlrprotocol_firebird::debugSystemError() {
	if (!getDebug()) {
		return;
	}
	char	*err=error::getErrorString();
	stdoutput.printf("%s\n",err);
	delete[] err;
}

void sqlrprotocol_firebird::debugOpCode(const char *name, uint32_t opcode) {
	if (!getDebug()) {
		return;
	}
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
	stdoutput.printf("	%s: 0x%02x %s\n",name,opcode,opcodestr);
}

void sqlrprotocol_firebird::debugArchType(uint32_t archtype) {
	if (!getDebug()) {
		return;
	}
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
	stdoutput.printf("	arch type: %s\n",archtypestr);
}

void sqlrprotocol_firebird::debugProtocolVersion(uint32_t protoversion) {
	if (!getDebug()) {
		return;
	}
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
	stdoutput.printf("	protocol version: 0x%04x %s\n",
				protoversion,protoversionstr);
}

void sqlrprotocol_firebird::debugProtocolType(const char *title,
						uint32_t protocoltype) {
	if (!getDebug()) {
		return;
	}
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
	stdoutput.printf("	%s: %s\n",
				title,protocoltypestr);
}

void sqlrprotocol_firebird::debugDpbVersion(byte_t dpbversion) {
	if (!getDebug()) {
		return;
	}
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

void sqlrprotocol_firebird::debugDpbParam(byte_t dpbparam) {
	if (!getDebug()) {
		return;
	}
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

void sqlrprotocol_firebird::debugDbInfoItem(byte_t dbinfoitem) {
	if (!getDebug()) {
		return;
	}
	const char	*dbinfoitemstr=NULL;
	switch (dbinfoitem) {
		case isc_info_end:
			dbinfoitemstr="isc_info_end";
			break;
		case isc_info_db_id:
			dbinfoitemstr="isc_info_db_id";
			break;
		case isc_info_reads:
			dbinfoitemstr="isc_info_reads";
			break;
		case isc_info_writes:
			dbinfoitemstr="isc_info_writes";
			break;
		case isc_info_fetches:
			dbinfoitemstr="isc_info_fetches";
			break;
		case isc_info_marks:
			dbinfoitemstr="isc_info_marks";
			break;
		case isc_info_implementation:
			dbinfoitemstr="isc_info_implementation";
			break;
		case isc_info_isc_version:
			dbinfoitemstr="isc_info_isc_version";
			break;
		case isc_info_base_level:
			dbinfoitemstr="isc_info_base_level";
			break;
		case isc_info_page_size:
			dbinfoitemstr="isc_info_page_size";
			break;
		case isc_info_num_buffers:
			dbinfoitemstr="isc_info_num_buffers";
			break;
		case isc_info_limbo:
			dbinfoitemstr="isc_info_limbo";
			break;
		case isc_info_current_memory:
			dbinfoitemstr="isc_info_current_memory";
			break;
		case isc_info_max_memory:
			dbinfoitemstr="isc_info_max_memory";
			break;
		case isc_info_window_turns:
			dbinfoitemstr="isc_info_window_turns";
			break;
		case isc_info_license:
			dbinfoitemstr="isc_info_license";
			break;
		case isc_info_allocation:
			dbinfoitemstr="isc_info_allocation";
			break;
		case isc_info_attachment_id:
			dbinfoitemstr="isc_info_attachment_id";
			break;
		case isc_info_read_seq_count:
			dbinfoitemstr="isc_info_read_seq_count";
			break;
		case isc_info_read_idx_count:
			dbinfoitemstr="isc_info_read_idx_count";
			break;
		case isc_info_insert_count:
			dbinfoitemstr="isc_info_insert_count";
			break;
		case isc_info_update_count:
			dbinfoitemstr="isc_info_update_count";
			break;
		case isc_info_delete_count:
			dbinfoitemstr="isc_info_delete_count";
			break;
		case isc_info_backout_count:
			dbinfoitemstr="isc_info_backout_count";
			break;
		case isc_info_purge_count:
			dbinfoitemstr="isc_info_purge_count";
			break;
		case isc_info_expunge_count:
			dbinfoitemstr="isc_info_expunge_count";
			break;
		case isc_info_sweep_interval:
			dbinfoitemstr="isc_info_sweep_interval";
			break;
		case isc_info_ods_version:
			dbinfoitemstr="isc_info_ods_version";
			break;
		case isc_info_ods_minor_version:
			dbinfoitemstr="isc_info_ods_minor_version";
			break;
		case isc_info_no_reserve:
			dbinfoitemstr="isc_info_no_reserve";
			break;
		case isc_info_logfile:
			dbinfoitemstr="isc_info_logfile";
			break;
		case isc_info_cur_logfile_name:
			dbinfoitemstr="isc_info_cur_logfile_name";
			break;
		case isc_info_cur_log_part_offset:
			dbinfoitemstr="isc_info_cur_log_part_offset";
			break;
		case isc_info_num_wal_buffers:
			dbinfoitemstr="isc_info_num_wal_buffers";
			break;
		case isc_info_wal_buffer_size:
			dbinfoitemstr="isc_info_wal_buffer_size";
			break;
		case isc_info_wal_ckpt_length:
			dbinfoitemstr="isc_info_wal_ckpt_length";
			break;
		case isc_info_wal_cur_ckpt_interval:
			dbinfoitemstr="isc_info_wal_cur_ckpt_interval";
			break;
		case isc_info_wal_prv_ckpt_fname:
			dbinfoitemstr="isc_info_wal_prv_ckpt_fname";
			break;
		case isc_info_wal_prv_ckpt_poffset:
			dbinfoitemstr="isc_info_wal_prv_ckpt_poffset";
			break;
		case isc_info_wal_recv_ckpt_fname:
			dbinfoitemstr="isc_info_wal_recv_ckpt_fname";
			break;
		case isc_info_wal_recv_ckpt_poffset:
			dbinfoitemstr="isc_info_wal_recv_ckpt_poffset";
			break;
		case isc_info_wal_grpc_wait_usecs:
			dbinfoitemstr="isc_info_wal_grpc_wait_usecs";
			break;
		case isc_info_wal_num_io:
			dbinfoitemstr="isc_info_wal_num_io";
			break;
		case isc_info_wal_avg_io_size:
			dbinfoitemstr="isc_info_wal_avg_io_size";
			break;
		case isc_info_wal_num_commits:
			dbinfoitemstr="isc_info_wal_num_commits";
			break;
		case isc_info_wal_avg_grpc_size:
			dbinfoitemstr="isc_info_wal_avg_grpc_size";
			break;
		case isc_info_forced_writes:
			dbinfoitemstr="isc_info_forced_writes";
			break;
		case isc_info_user_names:
			dbinfoitemstr="isc_info_user_names";
			break;
		case isc_info_page_errors:
			dbinfoitemstr="isc_info_page_errors";
			break;
		case isc_info_record_errors:
			dbinfoitemstr="isc_info_record_errors";
			break;
		case isc_info_bpage_errors:
			dbinfoitemstr="isc_info_bpage_errors";
			break;
		case isc_info_dpage_errors:
			dbinfoitemstr="isc_info_dpage_errors";
			break;
		case isc_info_ipage_errors:
			dbinfoitemstr="isc_info_ipage_errors";
			break;
		case isc_info_ppage_errors:
			dbinfoitemstr="isc_info_ppage_errors";
			break;
		case isc_info_tpage_errors:
			dbinfoitemstr="isc_info_tpage_errors";
			break;
		case isc_info_set_page_buffers:
			dbinfoitemstr="isc_info_set_page_buffers";
			break;
		case isc_info_db_sql_dialect:
			dbinfoitemstr="isc_info_db_sql_dialect";
			break;
		case isc_info_db_read_only:
			dbinfoitemstr="isc_info_db_read_only";
			break;
		case isc_info_db_size_in_pages:
			dbinfoitemstr="isc_info_db_size_in_pages";
			break;
		case frb_info_att_charset:
			dbinfoitemstr="frb_info_att_charset";
			break;
		case isc_info_db_class:
			dbinfoitemstr="isc_info_db_class";
			break;
		case isc_info_firebird_version:
			dbinfoitemstr="isc_info_firebird_version";
			break;
		case isc_info_oldest_transaction:
			dbinfoitemstr="isc_info_oldest_transaction";
			break;
		case isc_info_oldest_active:
			dbinfoitemstr="isc_info_oldest_active";
			break;
		case isc_info_oldest_snapshot:
			dbinfoitemstr="isc_info_oldest_snapshot";
			break;
		case isc_info_next_transaction:
			dbinfoitemstr="isc_info_next_transaction";
			break;
		case isc_info_db_provider:
			dbinfoitemstr="isc_info_db_provider";
			break;
		case isc_info_active_transactions:
			dbinfoitemstr="isc_info_active_transactions";
			break;
		case isc_info_active_tran_count:
			dbinfoitemstr="isc_info_active_tran_count";
			break;
		case isc_info_creation_date:
			dbinfoitemstr="isc_info_creation_date";
			break;
		case isc_info_db_file_size:
			dbinfoitemstr="isc_info_db_file_size";
			break;
		case fb_info_page_contents:
			dbinfoitemstr="fb_info_page_contents";
			break;
		default:
			dbinfoitemstr="unknown";
			break;
	}
	stdoutput.printf("	info item: %d (0x%02x) (%s)\n",
				dbinfoitem,dbinfoitem,dbinfoitemstr);
}

void sqlrprotocol_firebird::debugTpbVersion(byte_t tpbversion) {
	if (!getDebug()) {
		return;
	}
	const char	*tpbversionstr=NULL;
	switch (tpbversion) {
		case isc_tpb_version1:
			tpbversionstr="isc_tpb_version1";
			break;
		case isc_tpb_version3:
			tpbversionstr="isc_tpb_version3";
			break;
		default:
			tpbversionstr="unknown";
			break;
	}
	stdoutput.printf("	tpb version: %d (%s)\n",
				tpbversion,tpbversionstr);
}

void sqlrprotocol_firebird::debugTpbParam(byte_t tpbparam) {
	if (!getDebug()) {
		return;
	}
	const char	*tpbparamstr=NULL;
	switch (tpbparam) {
		case isc_tpb_consistency:
			tpbparamstr="isc_tpb_consistency";
			break;
		case isc_tpb_concurrency:
			tpbparamstr="isc_tpb_concurrency";
			break;
		case isc_tpb_shared:
			tpbparamstr="isc_tpb_shared";
			break;
		case isc_tpb_protected:
			tpbparamstr="isc_tpb_protected";
			break;
		case isc_tpb_exclusive:
			tpbparamstr="isc_tpb_exclusive";
			break;
		case isc_tpb_wait:
			tpbparamstr="isc_tpb_wait";
			break;
		case isc_tpb_nowait:
			tpbparamstr="isc_tpb_nowait";
			break;
		case isc_tpb_read:
			tpbparamstr="isc_tpb_read";
			break;
		case isc_tpb_write:
			tpbparamstr="isc_tpb_write";
			break;
		case isc_tpb_lock_read:
			tpbparamstr="isc_tpb_lock_read";
			break;
		case isc_tpb_lock_write:
			tpbparamstr="isc_tpb_lock_write";
			break;
		case isc_tpb_verb_time:
			tpbparamstr="isc_tpb_verb_time";
			break;
		case isc_tpb_commit_time:
			tpbparamstr="isc_tpb_commit_time";
			break;
		case isc_tpb_ignore_limbo:
			tpbparamstr="isc_tpb_ignore_limbo";
			break;
		case isc_tpb_read_committed:
			tpbparamstr="isc_tpb_read_committed";
			break;
		case isc_tpb_autocommit:
			tpbparamstr="isc_tpb_autocommit";
			break;
		case isc_tpb_rec_version:
			tpbparamstr="isc_tpb_rec_version";
			break;
		case isc_tpb_no_rec_version:
			tpbparamstr="isc_tpb_no_rec_version";
			break;
		case isc_tpb_restart_requests:
			tpbparamstr="isc_tpb_restart_requests";
			break;
		case isc_tpb_no_auto_undo:
			tpbparamstr="isc_tpb_no_auto_undo";
			break;
		case isc_tpb_lock_timeout:
			tpbparamstr="isc_tpb_lock_timeout";
			break;
		default:
			tpbparamstr="unknown";
			break;
	}
	stdoutput.printf("	tpb param: %d (0x%02x) (%s)\n",
				tpbparam,tpbparam,tpbparamstr);
}

void sqlrprotocol_firebird::debugStatusVector(uint64_t *sv, uint8_t svlen) {
	if (!getDebug()) {
		return;
	}
	stdoutput.write("	status vector:\n");
	uint32_t	cluster=1;
	uint8_t		i=0;
	while (i<svlen) {
		stdoutput.printf("		cluster %d:\n",cluster);
		switch (sv[i]) {
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
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_string:
				stdoutput.write("			"
						"code: isc_arg_string\n");
				i++;
				stdoutput.printf("			"
						"address: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_cstring:
				stdoutput.write("			"
						"code: isc_arg_cstring\n");
				i++;
				stdoutput.printf("			"
						"length: %lld\n",sv[i]);
				i++;
				stdoutput.printf("			"
						"address: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_number:
				stdoutput.write("			"
						"code: isc_arg_number\n");
				i++;
				stdoutput.printf("			"
						"number: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_interpreted:
				stdoutput.write("			"
						"code: isc_arg_interpreted\n");
				i++;
				stdoutput.printf("			"
						"address: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_vms:
				stdoutput.write("			"
						"code: isc_arg_vms\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_unix:
				stdoutput.write("			"
						"code: isc_arg_unix\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_domain:
				stdoutput.write("			"
						"code: isc_arg_domain\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_dos:
				stdoutput.write("			"
						"code: isc_arg_dos\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_mpexl:
				stdoutput.write("			"
						"code: isc_arg_mpexl\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_mpexl_ipc:
				stdoutput.write("			"
						"code: isc_arg_mpexl_ipc\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_mach:
				stdoutput.write("			"
						"code: isc_arg_mach\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_netware:
				stdoutput.write("			"
						"code: isc_arg_netware\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_win32:
				stdoutput.write("			"
						"code: isc_arg_win32\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_warning:
				stdoutput.write("			"
						"code: isc_arg_warning\n");
				i++;
				stdoutput.printf("			"
						"warning: %lld\n",sv[i]);
				i++;
				break;
			case isc_arg_sql_state:
				stdoutput.write("			"
						"code: isc_arg_sql_state\n");
				i++;
				stdoutput.printf("			"
						"sql state: %lld\n",sv[i]);
				i++;
				break;
			default:
				stdoutput.write("			"
						"code: unknown\n");
				i++;
				stdoutput.printf("			"
						"error: %lld\n",sv[i]);
				i++;
				break;
		}
		cluster++;
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
