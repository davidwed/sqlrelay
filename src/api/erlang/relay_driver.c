#include <ei.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlrelay/sqlrclientwrapper.h>

#define BUF_SIZE 2000 
#define COL_NAME_SIZE 512 	// max length of column names
#define FILE_NAME_SIZE 512 	// max length of file name
#define TRUE 0
#define FALSE 1
#define DEBUG 0		// set to 1 for debugging, otherwise set to 0

// define error codes
#define ERR_BINARY_TERM 	-1	
#define ERR_PROTOCOL 		-2	
#define ERR_NUMBER_OF_ARGS 	-3
#define ERR_DECODING_ARGS	-4
#define ERR_ENCODING_ARGS	-5
#define ERR_PREPARING_RESULTS	-6
#define ERR_PROCESSING_QUERY	-7
#define ERR_ROW_OUT_OF_RANGE	-8 
#define ERR_COL_OUT_OF_RANGE	-9 

// type definitions
typedef char byte;
typedef unsigned long ulong;

// global variables
sqlrcon con;
sqlrcur cur;

// define functions
int read_cmd(byte *buf, int *size);
int write_cmd(ei_x_buff* x);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);
int rowCount();
int colCount();

#define ENCODE_VOID 	if (ei_x_encode_atom(&result, "ok") || ei_x_encode_string(&result, "void")) { return ERR_ENCODING_ARGS; }


/*-----------------------------------------------------------------
 * Utility functions
 *----------------------------------------------------------------*/

// check that the given row is within the limits of the cursor
int checkRowLimitsOK(int row) {
	if (row < 0 || row >= rowCount(cur)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

// check that the given column is within the limits of the cursor
int checkColLimitsOK(int col) {
	if (col < 0 || col >= colCount(cur)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void signalError(ei_x_buff *result, long err) {
	ei_x_encode_atom(result, "error");
	ei_x_encode_long(result, err); 
}

/*-----------------------------------------------------------------
 * API functions
 *----------------------------------------------------------------*/

long alloc(char *server, ulong port, char *socket, char *user, char *password, ulong retrytime, ulong tries) {
	if (DEBUG) {
		fprintf(stderr, "Processing alloc with arguments: %s, %ld, %s, %s, %s, %ld, %ld\n\r", server, port, socket, user, password, retrytime, tries);
	}

	con = sqlrcon_alloc(server, port, socket, user, password, retrytime, tries);

	return 0;
}



int sendQuery(char *sql) {
	if (DEBUG) {
		fprintf(stderr, "Processing sendQuery %s\n\r", sql);
	}

	cur = sqlrcur_alloc(con);
	return(sqlrcur_sendQuery(cur, sql)); 	
}

int sendQueryWithLength(char *sql, uint32_t length) {
	cur = sqlrcur_alloc(con);
	return(sqlrcur_sendQueryWithLength(cur, sql, length)); 	
}

int sendFileQuery(char *path, char *filename) {
	cur = sqlrcur_alloc(con);
	return(sqlrcur_sendFileQuery(cur, path, filename)); 	
}

int prepareQuery(char *sql) {
	if (DEBUG) {
		fprintf(stderr, "Processing prepareQuery %s\n\r", sql);
	}

	cur = sqlrcur_alloc(con);
	sqlrcur_prepareQuery(cur, sql); 	
	return 0;
}

int prepareQueryWithLength(char *sql, uint32_t length) {
	cur = sqlrcur_alloc(con);
	sqlrcur_prepareQueryWithLength(cur, sql, length); 	
	return 0;
}

int prepareFileQuery(char *path, char *filename) {
	cur = sqlrcur_alloc(con);
	sqlrcur_prepareFileQuery(cur, path, filename); 	
	return 0;
}



int colCount() {
	return (sqlrcur_colCount(cur)); 	
}

int rowCount() {
	return (sqlrcur_rowCount(cur)); 	
}





/*-----------------------------------------------------------------
 * MAIN
 *----------------------------------------------------------------*/
int main() {
  	byte 	  *buf;
  	int       size = BUF_SIZE;
  	char      command[MAXATOMLEN];
  	int       index, version, arity;
	int 	  err;
  	ei_x_buff result;

  	if ((buf = (byte *) malloc(size)) == NULL) 
    		return -1;
   
	// main loop 
  	while (read_cmd(buf, &size) > 0) {
    		// Reset the index, so that ei functions can decode terms from the  beginning of the buffer 
    		index = 0;

    		// Ensure that we are receiving the binary term by reading and stripping the version byte 
    		if (ei_decode_version(buf, &index, &version)) {
			return ERR_BINARY_TERM;
		}
    
    		// Get the number of arguments
    		if (ei_decode_tuple_header(buf, &index, &arity)) {
			return ERR_PROTOCOL;
		} else {
			--arity;	// remove the command name from the argument count
		}

		// Get the command  
    		if (ei_decode_atom(buf, &index, command)) {
			return ERR_PROTOCOL;
		}
 
    		// Prepare the output buffer that will hold {ok, Result} or {error, Reason} 
    		if (ei_x_new_with_version(&result) || ei_x_encode_tuple_header(&result, 2)) {
			return ERR_PREPARING_RESULTS;
		}


		// 
		// process command
		//	
		if (DEBUG) {
			fprintf(stderr, "Received command %s, number of arguments is %d\n\r", command, arity);
   		}
		
		if (strcmp("alloc", command) == TRUE) {
			char server[30]; 
                	unsigned long port; 
                	char socket[30];
                	char user[30]; 
                	char password[30]; 
                	unsigned long retrytime; 
                	unsigned long tries; 			
			long c;

			// check number of arguments
		    	if (arity != 7) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &server[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_ulong(buf, &index, &port)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &socket[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &user[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &password[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_ulong(buf, &index, &retrytime)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_ulong(buf, &index, &tries)) { 
				return ERR_DECODING_ARGS;
			}
       
			// call function 
			c = alloc(server, port, socket, user, password, retrytime, tries);
     
			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, c)) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("ping", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_ping(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("connectionFree", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;
			
			// call function and encode result 
			sqlrcon_free(con); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("cursorFree", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;
			
			// call function and encode result 
			sqlrcur_free(cur); 	
			ENCODE_VOID;   
		}


		if (strcmp("setConnectTimeout", command) == TRUE) {
                	long timeoutsec;
                	long timeoutusec;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			if (ei_decode_long(buf, &index, &timeoutsec)) { 
				return ERR_DECODING_ARGS;
			}

			if (ei_decode_long(buf, &index, &timeoutusec)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcon_setConnectTimeout(con,timeoutsec,timeoutusec);
			ENCODE_VOID;   
		}


		if (strcmp("setAuthenticationTimeout", command) == TRUE) {
                	long timeoutsec;
                	long timeoutusec;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			if (ei_decode_long(buf, &index, &timeoutsec)) { 
				return ERR_DECODING_ARGS;
			}

			if (ei_decode_long(buf, &index, &timeoutusec)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcon_setAuthenticationTimeout(con,timeoutsec,timeoutusec);
			ENCODE_VOID;   
		}


		if (strcmp("setResponseTimeout", command) == TRUE) {
                	long timeoutsec;
                	long timeoutusec;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			if (ei_decode_long(buf, &index, &timeoutsec)) { 
				return ERR_DECODING_ARGS;
			}

			if (ei_decode_long(buf, &index, &timeoutusec)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcon_setResponseTimeout(con,timeoutsec,timeoutusec);
			ENCODE_VOID;   
		}

		if (strcmp("setBindVariableDelimiters", command) == TRUE) {
			char delimiters[128];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &delimiters[0])) { 
				return ERR_DECODING_ARGS;
			}

			sqlrcon_setBindVariableDelimiters(con, delimiters);
			ENCODE_VOID;
		}

		if (strcmp("getBindVariableDelimiterQuestionMarkSupported", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getBindVariableDelimiterQuestionMarkSupported(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getBindVariableDelimiterColonSupported", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getBindVariableDelimiterColonSupported(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getBindVariableDelimiterAtSignSupported", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getBindVariableDelimiterAtSignSupported(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getBindVariableDelimiterDollarSignSupported", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getBindVariableDelimiterDollarSignSupported(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("enableKerberos", command) == TRUE) {
                	char service[128];
			char mech[128];
			char flags[512];

			// check number of arguments
		    	if (arity != 3) return ERR_NUMBER_OF_ARGS;

			// get arguments
			if (ei_decode_string(buf, &index, &service[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &mech[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &flags[0])) { 
				return ERR_DECODING_ARGS;
			}

			sqlrcon_enableKerberos(con,service,mech,flags);
			ENCODE_VOID;   
		}

		if (strcmp("enableTls", command) == TRUE) {
                	char version[128];
                	char cert[1024];
			char password[128];
			char ciphers[1024];
			char validate[16];
			char ca[1024];
			long depth;

			// check number of arguments
		    	if (arity != 3) return ERR_NUMBER_OF_ARGS;

			// get arguments
			if (ei_decode_string(buf, &index, &version[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &cert[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &password[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &ciphers[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &validate[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &ca[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &depth)) { 
				return ERR_DECODING_ARGS;
			}

			sqlrcon_enableTls(con,version,cert,password,ciphers,validate,ca,depth);
			ENCODE_VOID;   
		}

		if (strcmp("disableEncryption", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcon_disableEncryption(con);
			ENCODE_VOID;   
		}

		if (strcmp("endSession", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcon_endSession(con); 	
			ENCODE_VOID;   
		}

		if (strcmp("suspendSession", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcon_suspendSession(con); 	
			ENCODE_VOID;   
		}

		if (strcmp("getConnectionPort", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getConnectionPort(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("getConnectionSocket", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_getConnectionSocket(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("resumeSession", command) == TRUE) {
                	unsigned long port; 
                	char socket[30];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get arguments
			if (ei_decode_ulong(buf, &index, &port)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &socket[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result,  sqlrcon_resumeSession(con, port, socket))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("identify", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_identify(con) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("dbVersion", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_dbVersion(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("dbHostName", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_dbHostName(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("dbIpAddress", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_dbIpAddress(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("bindFormat", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_bindFormat(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("selectDatabase", command) == TRUE) {
			char database[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &database[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcon_selectDatabase(con, database); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("getCurrentDatabase", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_getCurrentDatabase(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("getLastInsertId", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getLastInsertId(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("autoCommitOn", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_autoCommitOn(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("autoCommitOff", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_autoCommitOff(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("beginTransaction", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_begin(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("commit", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_commit(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("rollback", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_rollback(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("connectionErrorMessage", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_errorMessage(con) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("connectionErrorNumber", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_errorNumber(con) )) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("debugOn", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcon_debugOn(con); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("debugOff", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcon_debugOff(con); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("getDebug", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcon_getDebug(con))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("setDebugFile", command) == TRUE) {
			char debugfile[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &debugfile[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcon_setDebugFile(con, debugfile); 	
			ENCODE_VOID;   
		}

		if (strcmp("setClientInfo", command) == TRUE) {
			char clientinfo[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &clientinfo[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcon_setClientInfo(con, clientinfo); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("getClientInfo", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcon_getClientInfo(con))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("setResultSetBufferSize", command) == TRUE) {
                	unsigned long rows; 			

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &rows)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_setResultSetBufferSize(cur, rows); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("getResultSetBufferSize", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getResultSetBufferSize(cur))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("dontGetColumnInfo", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_dontGetColumnInfo(cur); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("getColumnInfo", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_getColumnInfo(cur); 	
			ENCODE_VOID;   
		}

		if (strcmp("mixedCaseColumnNames", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_mixedCaseColumnNames(cur); 	
			ENCODE_VOID;   
		}

		if (strcmp("upperCaseColumnNames", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_upperCaseColumnNames(cur); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("lowerCaseColumnNames", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_lowerCaseColumnNames(cur); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("cacheToFile", command) == TRUE) {
			char filename[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &filename[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_cacheToFile(cur, filename); 	
			ENCODE_VOID;   
		}
		
		if (strcmp("setCacheTtl", command) == TRUE) {
                	unsigned long ttl; 			

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &ttl)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_setCacheTtl(cur, ttl); 	
			ENCODE_VOID;   
		}

		if (strcmp("getCacheFileName", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcur_getCacheFileName(cur))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getDatabaseList", command) == TRUE) {
			char wild[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &wild[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_getDatabaseList(cur, wild); 	
			ENCODE_VOID;   
		}

		if (strcmp("getTableList", command) == TRUE) {
			char wild[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &wild[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_getTableList(cur, wild); 	
			ENCODE_VOID;   
		}

		if (strcmp("getColumnList", command) == TRUE) {
			char table[2000];
			char wild[2000];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &table[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &wild[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_getColumnList(cur, table, wild); 	
			ENCODE_VOID;   
		}

		if (strcmp("cacheOff", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_cacheOff(cur); 	
			ENCODE_VOID;   
		}


		if (strcmp("sendQuery", command) == TRUE) {
			char sql[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &sql[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sendQuery(sql))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("sendQueryWithLength", command) == TRUE) {
			char sql[2000];
			long length;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &sql[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &length)) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sendQueryWithLength(sql, length))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("sendFileQuery", command) == TRUE) {
			char path[2000];
			char filename[2000];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &path[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &filename[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sendFileQuery(path, filename))) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("prepareQuery", command) == TRUE) {
			char sql[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &sql[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, prepareQuery(sql))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("prepareQueryWithLength", command) == TRUE) {
			char sql[2000];
			long length;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &sql[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &length)) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, prepareQueryWithLength(sql, length))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("prepareFileQuery", command) == TRUE) {
			char path[2000];
			char filename[2000];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &path[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &filename[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, prepareFileQuery(path, filename) )) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("subString", command) == TRUE) {
			char variable[2000];
			char value[2000];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &value[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_subString(cur, variable, value); 	
			ENCODE_VOID;
		}


		if (strcmp("subLong", command) == TRUE) {
			char variable[2000];
			long value;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &value)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_subLong(cur, variable, value); 	
			ENCODE_VOID;
		}

		if (strcmp("subDouble", command) == TRUE) {
			char variable[2000];
			double value;
			long precision;
			long scale;

			// check number of arguments
		    	if (arity != 4) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_double(buf, &index, &value)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &precision)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &scale)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_subDouble(cur, variable, value, precision, scale); 	
			ENCODE_VOID;
		}

		if (strcmp("clearBinds", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_clearBinds(cur); 	
			ENCODE_VOID;
		}

		if (strcmp("countBindVariables", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_countBindVariables(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("inputBindString", command) == TRUE) {
			char variable[2000];
			char value[2000];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &value[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_inputBindString(cur, variable, value); 	
			ENCODE_VOID;
		}

		if (strcmp("inputBindStringWithLength", command) == TRUE) {
			char variable[2000];
			char value[2000];
			long length;

			// check number of arguments
		    	if (arity != 3) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &value[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &length)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_inputBindStringWithLength(cur, variable, value, length); 	
			ENCODE_VOID;
		}


		if (strcmp("inputBindLong", command) == TRUE) {
			char variable[2000];
			long value;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &value)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_inputBindLong(cur, variable, value); 	
			ENCODE_VOID;
		}

		if (strcmp("inputBindDouble", command) == TRUE) {
			char variable[2000];
			double value;
			long precision;
			long scale;

			// check number of arguments
		    	if (arity != 4) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_double(buf, &index, &value)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &precision)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &scale)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_inputBindDouble(cur, variable, value, precision, scale); 	
			ENCODE_VOID;
		}

		if (strcmp("inputBindBlob", command) == TRUE) {
			char variable[2000];
			char value[2000];
			long size;

			// check number of arguments
		    	if (arity != 3) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &value[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &size)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_inputBindBlob(cur, variable, value, size); 	
			ENCODE_VOID;
		}

		if (strcmp("inputBindClob", command) == TRUE) {
			char variable[2000];
			char value[2000];
			long size;

			// check number of arguments
		    	if (arity != 3) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &value[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &size)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_inputBindClob(cur, variable, value, size); 	
			ENCODE_VOID;
		}

		if (strcmp("defineOutputBindString", command) == TRUE) {
			char variable[2000];
			long length;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &length)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_defineOutputBindString(cur, variable, length); 	
			ENCODE_VOID;
		}

		if (strcmp("defineOutputBindInteger", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_defineOutputBindInteger(cur, variable); 	
			ENCODE_VOID;
		}

		if (strcmp("defineOutputBindDouble", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_defineOutputBindDouble(cur, variable); 	
			ENCODE_VOID;
		}

		if (strcmp("defineOutputBindBlob", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_defineOutputBindBlob(cur, variable); 	
			ENCODE_VOID;
		}

		if (strcmp("defineOutputBindClob", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_defineOutputBindClob(cur, variable); 	
			ENCODE_VOID;
		}
		
		if (strcmp("defineOutputBindCursor", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			sqlrcur_defineOutputBindCursor(cur, variable); 	
			ENCODE_VOID;
		}

		if (strcmp("validateBinds", command) == TRUE) {

			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_validateBinds(cur); 	
			ENCODE_VOID;
		}

		if (strcmp("validBind", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_validBind(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("executeQuery", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_executeQuery(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("fetchFromBindCursor", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_fetchFromBindCursor(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getOutputBindString", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcur_getOutputBindString(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getOutputBindBlob", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcur_getOutputBindBlob(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getOutputBindClob", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcur_getOutputBindClob(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getOutputBindInteger", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getOutputBindInteger(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getOutputBindDouble", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_double(&result, sqlrcur_getOutputBindDouble(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getOutputBindLength", command) == TRUE) {
			char variable[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &variable[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getOutputBindLength(cur, variable) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("openCachedResultSet", command) == TRUE) {
			char filename[2000];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &filename[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_openCachedResultSet(cur, filename) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("colCount", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, colCount() )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("rowCount", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, rowCount() )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("totalRows", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_totalRows(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("affectedRows", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_affectedRows(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("firstRowIndex", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_firstRowIndex(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("endOfResultSet", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_endOfResultSet(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("errorMessage", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcur_errorMessage(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("errorNumber", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_errorNumber(cur) )) {
				return ERR_ENCODING_ARGS;
			}
		}
		
		if (strcmp("getNullsAsEmptyStrings", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_getNullsAsEmptyStrings(cur); 	
			ENCODE_VOID;
		}

		if (strcmp("getNullsAsNulls", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_getNullsAsNulls(cur); 	
			ENCODE_VOID;
		}

		if (strcmp("getFieldByIndex", command) == TRUE) {
			long row;
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row and col values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else {
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_string(&result, sqlrcur_getFieldByIndex(cur, row, col))) {
						return ERR_ENCODING_ARGS;
				}	
			}
		}

		if (strcmp("getFieldByName", command) == TRUE) {
			long row;
			char col[COL_NAME_SIZE];
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_string(&result, sqlrcur_getFieldByName(cur, row, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getFieldAsIntegerByIndex", command) == TRUE) {
			long row;
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row and col values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getFieldAsIntegerByIndex(cur, row, col) )) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getFieldAsIntegerByName", command) == TRUE) {
			long row;
			char col[COL_NAME_SIZE];
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getFieldAsIntegerByName(cur, row, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getFieldAsDoubleByIndex", command) == TRUE) {
			long row;
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row and col values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_double(&result, sqlrcur_getFieldAsDoubleByIndex(cur, row, col) )) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getFieldAsDoubleByName", command) == TRUE) {
			long row;
			char col[COL_NAME_SIZE];
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_double(&result, sqlrcur_getFieldAsDoubleByName(cur, row, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}


		if (strcmp("getFieldLengthByIndex", command) == TRUE) {
			long row;
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row and col values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getFieldLengthByIndex(cur, row, col) )) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getFieldLengthByName", command) == TRUE) {
			long row;
			char col[COL_NAME_SIZE];
			err = 0;

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getFieldLengthByName(cur, row, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getRow", command) == TRUE) {
			long row;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_string(&result, (char *) sqlrcur_getRow(cur, row))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getRowLengths", command) == TRUE) {
			long row;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &row)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of row values
			if (checkRowLimitsOK(row) == FALSE) {
				err = ERR_ROW_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, (long) sqlrcur_getRowLengths(cur, row))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnNames", command) == TRUE) {
			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, (char *) sqlrcur_getColumnNames(cur))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnName", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_string(&result, sqlrcur_getColumnName(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

/***/
		if (strcmp("getColumnTypeByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_string(&result, sqlrcur_getColumnTypeByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnTypeByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_string(&result, sqlrcur_getColumnTypeByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnLengthByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnLengthByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnLengthByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnLengthByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("getColumnPrecisionByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnPrecisionByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnPrecisionByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnPrecisionByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnScaleByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnScaleByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnScaleByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnScaleByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("getColumnIsNullableByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsNullableByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsNullableByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsNullableByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnIsPrimaryKeyByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsPrimaryKeyByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsPrimaryKeyByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsPrimaryKeyByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnIsUniqueByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsUniqueByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsUniqueByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsUniqueByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnIsPartOfKeyByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsPartOfKeyByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsPartOfKeyByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsPartOfKeyByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("getColumnIsUnsignedByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsUnsignedByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsUnsignedByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsUnsignedByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("getColumnIsZeroFilledByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsZeroFilledByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsZeroFilledByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsZeroFilledByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}



		if (strcmp("getColumnIsBinaryByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsBinaryByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsBinaryByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsBinaryByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("getColumnIsAutoIncrementByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getColumnIsAutoIncrementByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getColumnIsAutoIncrementByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getColumnIsAutoIncrementByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}


		if (strcmp("getLongestByIndex", command) == TRUE) {
			long col;
			err = 0;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &col)) { 
				return ERR_DECODING_ARGS;
			}

			// check sanity of column value
			if (checkColLimitsOK(col) == FALSE) {
				err = ERR_COL_OUT_OF_RANGE; 
			}
	
			// encode result 
			if (err) {
				signalError(&result, err); 
			} else { 
				if (ei_x_encode_atom(&result, "ok") || 
					ei_x_encode_long(&result, sqlrcur_getLongestByIndex(cur, col))) {
					return ERR_ENCODING_ARGS;
				}
			}
		}

		if (strcmp("getLongestByName", command) == TRUE) {
			char col[COL_NAME_SIZE];

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_string(buf, &index, &col[0])) { 
				return ERR_DECODING_ARGS;
			}

			// encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getLongestByName(cur, col))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("suspendResultSet", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_suspendResultSet(cur); 	
			ENCODE_VOID;   
		}

		if (strcmp("getResultSetId", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_getResultSetId(cur))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("resumeResultSet", command) == TRUE) {
			long id;

			// check number of arguments
		    	if (arity != 1) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &id)) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_resumeResultSet(cur, id))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("resumeCachedResultSet", command) == TRUE) {
			long id;
			char filename[FILE_NAME_SIZE];

			// check number of arguments
		    	if (arity != 2) return ERR_NUMBER_OF_ARGS;

			// get input parameters
			if (ei_decode_long(buf, &index, &id)) { 
				return ERR_DECODING_ARGS;
			}
			if (ei_decode_string(buf, &index, &filename[0])) { 
				return ERR_DECODING_ARGS;
			}

			// call function and encode result 
			if (ei_x_encode_atom(&result, "ok") || 
				ei_x_encode_long(&result, sqlrcur_resumeCachedResultSet(cur, id, filename))) {
				return ERR_ENCODING_ARGS;
			}
		}

		if (strcmp("closeResultSet", command) == TRUE) {
			// check number of arguments
		    	if (arity != 0) return ERR_NUMBER_OF_ARGS;

			// call function and encode result 
			sqlrcur_closeResultSet(cur); 	
			ENCODE_VOID;   
		}



		// write the result buffer back to the 
		// calling Erlang program    	
		write_cmd(&result);

		// free memory structure
    		ei_x_free(&result);
  	} // end of while statement

  	return 0;
}
