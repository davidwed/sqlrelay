// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle7connection.h>

#include <config.h>
#include <datatypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

oracle7connection::oracle7connection() {
	oraclehomeenv=NULL;
	oraclesidenv=NULL;
	twotaskenv=NULL;
}

int	oracle7connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	oracle7connection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	sid=connectStringValue("oracle_sid");
	home=connectStringValue("oracle_home");
	char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom && !strcasecmp(autocom,"yes")));
}

int	oracle7connection::logIn() {

	// handle ORACLE_HOME
	if (home) {
		oraclehomeenv=new char[strlen(home)+13];
		sprintf(oraclehomeenv,"ORACLE_HOME=%s",home);
		if (!setEnv("ORACLE_HOME",home,oraclehomeenv)) {
			fprintf(stderr,"Failed to set ORACLE_HOME environment variable.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			return 0;
		}
	} else {
		if (!getenv("ORACLE_HOME")) {
			fprintf(stderr,"No ORACLE_HOME environment variable set or specified in connect string.\n");
			return 0;
		}
	}

	// handle ORACLE_SID
	if (sid) {
		oraclesidenv=new char[strlen(sid)+12];
		sprintf(oraclesidenv,"ORACLE_SID=%s",sid);
		if (!setEnv("ORACLE_SID",sid,oraclesidenv)) {
			fprintf(stderr,"Failed to set ORACLE_SID environment variable.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			delete[] oraclesidenv;
			oraclesidenv=NULL;
			return 0;
		}
	} else {
		if (!getenv("ORACLE_SID")) {
			fprintf(stderr,"No ORACLE_SID environment variable set or specified in connect string.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			return 0;
		}
	}

	// handle TWO_TASK
	if (sid) {
		twotaskenv=new char[strlen(sid)+10];
		sprintf(twotaskenv,"TWO_TASK=%s",sid);
		if (!setEnv("TWO_TASK",sid,twotaskenv)) {
			fprintf(stderr,"Failed to set TWO_TASK environment variable.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			delete[] oraclesidenv;
			oraclesidenv=NULL;
			delete[] twotaskenv;
			twotaskenv=NULL;
			return 0;
		}
	} else {
		if (!getenv("TWO_TASK")) {
			fprintf(stderr,"No TWO_TASK environment variable set or specified in connect string.\n");
			delete[] oraclehomeenv;
			oraclehomeenv=NULL;
			delete[] oraclesidenv;
			oraclesidenv=NULL;
			return 0;
		}
	}

	// log in
	if (olog(&lda,(ub1 *)hda,
			(text *)getUser(),-1,(text *)getPassword(),-1,
			(text *)0,-1,
			(ub4)OCI_LM_DEF)) {

		// display the error message
		text	message[512];
		sword	n=oerhms(&lda,lda.rc,message,(sword)sizeof(message));
		message[n]=NULL;
		fprintf(stderr,"%s\n",message);
		delete[] oraclehomeenv;
		oraclehomeenv=NULL;
		delete[] oraclesidenv;
		oraclesidenv=NULL;
		delete[] twotaskenv;
		twotaskenv=NULL;
		return 0;
	}
	return 1;
}

sqlrcursor	*oracle7connection::initCursor() {
	return (sqlrcursor *)new oracle7cursor((sqlrconnection *)this);
}

void	oracle7connection::deleteCursor(sqlrcursor *curs) {
	delete (oracle7cursor *)curs;
}

void	oracle7connection::logOut() {
	ologof(&lda);
	delete[] oraclehomeenv;
	oraclehomeenv=NULL;
	delete[] oraclesidenv;
	oraclesidenv=NULL;
	delete[] twotaskenv;
	twotaskenv=NULL;
}

unsigned short	oracle7connection::autoCommitOn() {
	return (unsigned short)!ocon(&lda);
}

unsigned short	oracle7connection::autoCommitOff() {
	return (unsigned short)!ocof(&lda);
}

int	oracle7connection::commit() {
	return !ocom(&lda);
}

int	oracle7connection::rollback() {
	return !orol(&lda);
}

int	oracle7connection::ping() {
	int	retval=0;
	oracle7cursor	*cur=new oracle7cursor(this);
	if (cur->openCursor(-1) && 
		cur->prepareQuery("select 1 from dual",18) && 
		cur->executeQuery("select 1 from dual",18,1)) {
		cur->cleanUpData();
		cur->closeCursor();
		retval=1;
	}
	delete cur;
	return retval;
}

char	*oracle7connection::identify() {
	return "oracle7";
}

oracle7cursor::oracle7cursor(sqlrconnection *conn) : sqlrcursor(conn) {
	errormessage=NULL;
	oracle7conn=(oracle7connection *)conn;
}

oracle7cursor::~oracle7cursor() {
	delete errormessage;
}

int	oracle7cursor::openCursor(int id) {
	return !oopen(&cda,&oracle7conn->lda,(text *)0,-1,-1,(text *)0,-1);
}

int	oracle7cursor::closeCursor() {
	return !oclose(&cda);
}

int	oracle7cursor::prepareQuery(const char *query, long length) {

	// parse the query
	if (oparse(&cda,(text *)query,(sb4)length,
		(sword)PARSE_DEFER,(ub4)PARSE_V7_LNG)) {
		return 0;
	}
	return 1;
}

int	oracle7cursor::inputBindString(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned short valuesize,
						short *isnull) {

	// bind the value to the variable
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
			(ub1 *)value,(sword)valuesize+1,NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return 0;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)valuesize+1,NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return 0;
		}
	}
	return 1;
}

int	oracle7cursor::inputBindLong(const char *variable, 
						unsigned short variablesize,
						unsigned long *value) {
	
	// bind the value to the variable
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
			(ub1 *)value,(sword)sizeof(long),LONG_BIND_TYPE,
			-1,(sb2 *)0,(text *)0,-1,-1)) {
			return 0;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)sizeof(long),LONG_BIND_TYPE,
			-1,(sb2 *)0,(text *)0,-1,-1)) {
			return 0;
		}
	}
	return 1;
}

int	oracle7cursor::inputBindDouble(const char *variable, 
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale) {
	
	// bind the value to the variable
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
			(ub1 *)value,(sword)sizeof(double),DOUBLE_BIND_TYPE,
			(sword)scale,(sb2 *)0,(text *)0,-1,-1)) {
			return 0;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)sizeof(double),DOUBLE_BIND_TYPE,
			(sword)scale,(sb2 *)0,(text *)0,-1,-1)) {
			return 0;
		}
	}
	return 1;
}

int	oracle7cursor::outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {

	// bind the value to the variable
	if (isNumber(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return 0;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
			(ub1 *)value,(sword)valuesize,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return 0;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)valuesize,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return 0;
		}
	}
	return 1;
}

int	oracle7cursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// initialize the column count
	ncols=0;

	// if the query is a select, describe/define it
	if (cda.ft==SELECT_QUERY) {

		// run through the columns...
		while (1) {

			// init the column name size
			desc[ncols].buflen=MAX_ITEM_BUFFER_SIZE;

			// describe the column
			if (!odescr(&cda,ncols+1,
				&desc[ncols].dbsize, 
				&desc[ncols].dbtype,
				&desc[ncols].buf[0], 
				&desc[ncols].buflen,
				&desc[ncols].dsize, 
				&desc[ncols].precision,
				&desc[ncols].scale, 
				&desc[ncols].nullok)) {

				// define the column
				if (odefin(&cda,ncols+1,
						*def_buf[ncols],
						MAX_ITEM_BUFFER_SIZE,
						NULL_TERMINATED_STRING,
						-1,
						def_indp[ncols],
						(text *)0,
						-1,
						-1,
						def_col_retlen[ncols],
						def_col_retcode[ncols])) {
					return 0;
				}

			} else {

				// if we're at the end of the column list...
				if (cda.rc==NOT_IN_LIST) {
					break;
				} else {
					return 0;
				}
			}
	
			ncols++;
		}
	}

	// initialize row counters
	row=0;
	maxrow=0;
	totalrows=0;

	// execute the query
	if (oexec(&cda)) {
		return 0;
	}

	return 1;
}

int	oracle7cursor::queryIsNotSelect() {

	if (cda.ft==SELECT_QUERY) {
		return 0;
	}
	return 1;
}

int	oracle7cursor::queryIsCommitOrRollback() {

	if (cda.ft==COMMIT_QUERY || cda.ft==ROLLBACK_QUERY) {
		return 1;
	}
	return 0;
}

char	*oracle7cursor::getErrorMessage(int *liveconnection) {

	// get the message from oracle
	text	message[512];
	sword	n=oerhms(&oracle7conn->lda,
				cda.rc,message,(sword)sizeof(message));

	// check for dead connection
	if (!strncmp((char *)message,"ORA-03114",9) || 
		!strncmp((char *)message,"ORA-03113",9)) {
		*liveconnection=0;
	} else {
		*liveconnection=1;
	}

	// only return an error message if the error wasn't a dead database
	delete errormessage;
	errormessage=new stringbuffer();
	if (*liveconnection) {
		for (int i=0; i<n; i++) {
			errormessage->append((char)message[i]);
		}
	}

	return errormessage->getString();
}

void	oracle7cursor::returnRowCounts() {

	// send row counts (actual row count unknown in oracle)
	oracle7conn->sendRowCounts((long)-1,(long)cda.rpc);
}

void	oracle7cursor::returnColumnCount() {
	oracle7conn->sendColumnCount(ncols);
}

void	oracle7cursor::returnColumnInfo() {

	// a useful variables
	int		type;

	// for each column...
	for (int i=0; i<ncols; i++) {

		// set column type
		if (desc[i].dbtype==VARCHAR2_TYPE) {
			type=VARCHAR2_DATATYPE;
		} else if (desc[i].dbtype==NUMBER_TYPE) {
			type=NUMBER_DATATYPE;
		} else if (desc[i].dbtype==LONG_TYPE) {
			type=LONG_DATATYPE;
		} else if (desc[i].dbtype==ROWID_TYPE) {
			type=ROWID_DATATYPE;
		} else if (desc[i].dbtype==DATE_TYPE) {
			type=DATE_DATATYPE;
		} else if (desc[i].dbtype==RAW_TYPE) {
			type=RAW_DATATYPE;
		} else if (desc[i].dbtype==LONG_RAW_TYPE) {
			type=LONG_RAW_DATATYPE;
		} else if (desc[i].dbtype==CHAR_TYPE) {
			type=CHAR_DATATYPE;
		} else if (desc[i].dbtype==MLSLABEL_TYPE) {
			type=MLSLABEL_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send the column definition
		conn->sendColumnDefinition((char *)desc[i].buf,
					(short)desc[i].buflen,type,
					(int)desc[i].dbsize);
	}
}

int	oracle7cursor::noRowsToReturn() {
	if (cda.ft!=SELECT_QUERY) {
		return 1;
	}
	return 0;
}

int	oracle7cursor::skipRow() {
	if (fetchRow()) {
		row++;
		return 1;
	}
	return 0;
}

int	oracle7cursor::fetchRow() {
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return 0;
	}
	if (row==0) {
		if (ofen(&cda,FETCH_AT_ONCE) && cda.rpc==totalrows) {
			return 0;
		}
		maxrow=cda.rpc-totalrows;
		totalrows=cda.rpc;
	}
	return 1;
}

void	oracle7cursor::returnRow() {

	for (sword col=0; col<ncols; col++) {

		// handle NULL's
		if (def_indp[col][row]==-1) {
			conn->sendNullField();
			continue;
		}

		// handle long datatypes
		if (desc[col].dbtype==LONG_TYPE ||
			desc[col].dbtype==LONG_RAW_TYPE) {

			ub4	retlen=MAX_ITEM_BUFFER_SIZE;
			sb4	offset=0;

			conn->startSendingLong();

			while (retlen==MAX_ITEM_BUFFER_SIZE) {

				oflng(&cda,col+1,
					def_buf[col][row],
					MAX_ITEM_BUFFER_SIZE,1,
					&retlen,offset);

				conn->sendLongSegment((char *)
							def_buf[col][row],
							(long)retlen);

				offset=offset+retlen;
			}

			conn->endSendingLong();

			continue;

		}

		// handle normal datatypes
		conn->sendField((char *)def_buf[col][row],
					(int)def_col_retlen[col][row]);
	}

	// increment the row counter
	row++;
}

void	oracle7cursor::cleanUpData() {
	ocan(&cda);
}
