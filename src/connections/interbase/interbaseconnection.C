// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sys/types.h>
#include <unistd.h>

#include <interbaseconnection.h>

#include <config.h>
#include <datatypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

interbaseconnection::interbaseconnection() {
	db=0L;
	tr=0L;
	env=new environment();
}

interbaseconnection::~interbaseconnection() {
	delete env;
}

int	interbaseconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void	interbaseconnection::handleConnectString() {

	// override legacy "database" parameter with modern "db" parameter
	database=connectStringValue("database");
	char *tmp=connectStringValue("db");
	if (tmp && tmp[0]) {
		database=tmp;
	}

	char	*dialectstr=connectStringValue("dialect");
	if (dialectstr) {
		dialect=atoi(dialectstr);
		if (dialect<1) {
			dialect=1;
		}
		if (dialect>3) {
			dialect=3;
		}
	} else {
		dialect=3;
	}
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom && !strcasecmp(autocom,"yes")));
}

int	interbaseconnection::logIn() {

	// initialize a dpb
	char	*dpbptr=dpb;
	*dpbptr++=isc_dpb_version1;
	*dpbptr++=isc_dpb_num_buffers;
	*dpbptr++=1;
	*dpbptr++=90;
	dpblength=dpbptr-dpb;

	// handle user/password parameters
	char	*user=getUser();
	if (user) {
		env->setValue("ISC_USER",user);
	}
	char	*password=getPassword();
	if (password) {
		env->setValue("ISC_PASSWORD",password);
	}

	// attach to the database
	if (isc_attach_database(error,strlen(database),database,&db,
							//dpblength,dpb)) {
							0,NULL)) {
		db=0L;
		return 0;
	}

	// start a transaction for non-ddl queries
	if (isc_start_transaction(error,&tr,1,&db,
				(unsigned short)sizeof(tpb),&tpb)) {

		// print the error message
		char	msg[512];
		long	*err=error;
		while (isc_interprete(msg,&err)) {
			fprintf(stderr,"%s\n",msg);
		}
		fprintf(stderr,"\n");
		return 0;
	}
	return 1;
}

sqlrcursor	*interbaseconnection::initCursor() {
	return (sqlrcursor *)new interbasecursor((sqlrconnection *)this);
}

void	interbaseconnection::deleteCursor(sqlrcursor *curs) {
	delete (interbasecursor *)curs;
}

void	interbaseconnection::logOut() {
	isc_detach_database(error,&db);
}

int	interbaseconnection::commit() {
	if (isc_commit_retaining(error,&tr)) {
		return 0;
	}
	return 1;
}

int	interbaseconnection::rollback() {
	if (isc_rollback_retaining(error,&tr)) {
		return 0;
	}
	return 1;
}

int	interbaseconnection::ping() {

	// call isc_database_info to get page_size and num_buffers,
	// this should always be available unless the db is down
	// if we get an error, then return 0, otherwise return 1
	ISC_STATUS	status[20];
	char		dbitems[]={isc_info_page_size,
					isc_info_num_buffers,
					isc_info_end};
	char		resbuffer[40];

	isc_database_info(status,&db,
				sizeof(dbitems),dbitems,
				sizeof(resbuffer),resbuffer);

	if (status[0] == 1 && status[1]) {
		return 0;
	}
	return 1;
}

char	*interbaseconnection::identify() {
	return "interbase";
}

interbasecursor::interbasecursor(sqlrconnection *conn) : sqlrcursor(conn) {
	interbaseconn=(interbaseconnection *)conn;
	errormsg=NULL;

	outsqlda=(XSQLDA ISC_FAR *)malloc(XSQLDA_LENGTH(MAX_SELECT_LIST_SIZE));
	outsqlda->version=SQLDA_VERSION1;
	outsqlda->sqln=MAX_SELECT_LIST_SIZE;

	insqlda=(XSQLDA ISC_FAR *)malloc(XSQLDA_LENGTH(MAX_BIND_VARS));
	insqlda->version=SQLDA_VERSION1;
	insqlda->sqln=MAX_BIND_VARS;

	querytype=0;
	stmt=NULL;

	queryIsExecSP=0;
}

interbasecursor::~interbasecursor() {
	if (errormsg) {
		delete errormsg;
	}
	free(outsqlda);
	free(insqlda);
}

int	interbasecursor::prepareQuery(const char *query, long length) {

	queryIsExecSP=0;

	// free the old statement if it exists
	if (stmt) {
		isc_dsql_free_statement(interbaseconn->error,&stmt,DSQL_drop);
	}

	// allocate a cursor handle
	stmt=NULL;
	if (isc_dsql_allocate_statement(interbaseconn->error,
					&interbaseconn->db,&stmt)) {
		return 0;
	}

	// skip whitespace
	char	*qptr=(char *)query;
	while (*qptr==' ' || *qptr=='\n' || *qptr=='	');

	// prepare the cursor using the appropriate transaction
	if (isc_dsql_prepare(interbaseconn->error,&interbaseconn->tr,
					&stmt,length,(char *)query,
					interbaseconn->dialect,outsqlda)) {
		return 0;
	}

	// get the cursor type
	char	typeitem[]={isc_info_sql_stmt_type};
	char	resbuffer[1024];
	if (isc_dsql_sql_info(interbaseconn->error,&stmt,
				sizeof(typeitem),typeitem,
				1024,resbuffer)) {
		return 0;
	}

	int	len=isc_vax_integer(resbuffer+1,2);
	querytype=isc_vax_integer(resbuffer+3,len);

	// find bind parameters, if any
	insqlda->sqld=0;
	if (isc_dsql_describe_bind(interbaseconn->error,&stmt,1,insqlda)) {
		return 0;
	}
	insqlda->sqln=insqlda->sqld;

	return 1;
}

int	interbasecursor::inputBindString(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned short valuesize,
					short *isnull) {

	// make bind vars 1 based like all other db's
	int	index=atoi(variable+1)-1;
	if (index<0) {
		return 0;
	}
	insqlda->sqlvar[index].sqltype=SQL_TEXT+1;
	insqlda->sqlvar[index].sqlscale=0;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=valuesize;
	insqlda->sqlvar[index].sqldata=(char *)value;
	insqlda->sqlvar[index].sqlind=isnull;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]=(char)NULL;
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]=(char)NULL;
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]=(char)NULL;
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]=(char)NULL;
	return 1;
}

int	interbasecursor::inputBindLong(const char *variable,
					unsigned short variablesize,
					unsigned long *value) {

	// make bind vars 1 based like all other db's
	int	index=atoi(variable+1)-1;
	if (index<0) {
		return 0;
	}
	insqlda->sqlvar[index].sqltype=SQL_LONG;
	insqlda->sqlvar[index].sqlscale=0;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=sizeof(long);
	insqlda->sqlvar[index].sqldata=(char *)value;
	insqlda->sqlvar[index].sqlind=(short *)NULL;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]=(char)NULL;
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]=(char)NULL;
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]=(char)NULL;
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]=(char)NULL;
	return 1;
}

int	interbasecursor::inputBindDouble(const char *variable,
					unsigned short variablesize,
					double *value,
					unsigned short precision,
					unsigned short scale) {

	// make bind vars 1 based like all other db's
	int	index=atoi(variable+1)-1;
	if (index<0) {
		return 0;
	}
	insqlda->sqlvar[index].sqltype=SQL_DOUBLE;
	insqlda->sqlvar[index].sqlscale=scale;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=sizeof(double);
	insqlda->sqlvar[index].sqldata=(char *)value;
	insqlda->sqlvar[index].sqlind=(short *)NULL;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]=(char)NULL;
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]=(char)NULL;
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]=(char)NULL;
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]=(char)NULL;
	return 1;
}

int	interbasecursor::outputBindString(const char *variable, 
				unsigned short variablesize,
				char *value, 
				unsigned short valuesize, 
				short *isnull) {

	// if we're doing output binds then the
	// query must be a stored procedure
	// FIXME: what if the procedure only has input vars?
	queryIsExecSP=1;

	// make bind vars 1 based like all other db's
	int	index=atoi(variable+1)-1;
	if (index<0) {
		return 0;
	}
	outsqlda->sqlvar[index].sqltype=SQL_TEXT+1;
	outsqlda->sqlvar[index].sqlscale=0;
	outsqlda->sqlvar[index].sqlsubtype=0;
	outsqlda->sqlvar[index].sqllen=valuesize;
	outsqlda->sqlvar[index].sqldata=value;
	outsqlda->sqlvar[index].sqlind=isnull;
	outsqlda->sqlvar[index].sqlname_length=0;
	outsqlda->sqlvar[index].sqlname[0]=(char)NULL;
	outsqlda->sqlvar[index].relname_length=0;
	outsqlda->sqlvar[index].relname[0]=(char)NULL;
	outsqlda->sqlvar[index].ownname_length=0;
	outsqlda->sqlvar[index].ownname[0]=(char)NULL;
	outsqlda->sqlvar[index].aliasname_length=0;
	outsqlda->sqlvar[index].aliasname[0]=(char)NULL;
	return 1;
}

int	interbasecursor::executeQuery(const char *query, long length,
						unsigned short execute) {

	// for commit or rollback, execute the API call and return
	if (querytype==isc_info_sql_stmt_commit) {
		return !isc_commit_retaining(interbaseconn->error,
							&interbaseconn->tr);
	} else if (querytype==isc_info_sql_stmt_rollback) {
		return !isc_rollback_retaining(interbaseconn->error,
							&interbaseconn->tr);
	}

	// if the query is a stored procedure then execute it as such
	if (queryIsExecSP) {
		if (isc_dsql_execute2(interbaseconn->error,
					&interbaseconn->tr,
					&stmt,1,insqlda,outsqlda)) {
			return 0;
		}

		for (int i=0; i<outsqlda->sqld; i++) {
			if ((outsqlda->sqlvar[i].sqltype==SQL_TEXT) ||
				(outsqlda->sqlvar[i].sqltype==SQL_TEXT+1) ) {
				outsqlda->sqlvar[i].
					sqldata[outsqlda->sqlvar[i].sqllen-1]=0;
			}
		}
		return 1;
	}

	// handle non-stored procedures...

	// describe the cursor
	outsqlda->sqld=0;
	if (isc_dsql_describe(interbaseconn->error,&stmt,1,outsqlda)) {
		return 0;
	}
	if (outsqlda->sqld>MAX_SELECT_LIST_SIZE) {
		outsqlda->sqld=MAX_SELECT_LIST_SIZE;
	}

	for (int i=0; i<outsqlda->sqld; i++) {

		// save the actual field type
		field[i].type=outsqlda->sqlvar[i].sqltype;

		// handle the null indicator
		outsqlda->sqlvar[i].sqlind=&field[i].nullindicator;

		// coerce the datatypes and point where the data should go
		if (outsqlda->sqlvar[i].sqltype==SQL_TEXT || 
				outsqlda->sqlvar[i].sqltype==SQL_TEXT+1) {
			outsqlda->sqlvar[i].sqldata=field[i].textbuffer;
			field[i].sqlrtype=CHAR_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_VARYING ||
				outsqlda->sqlvar[i].sqltype==SQL_VARYING+1) {
			outsqlda->sqlvar[i].sqldata=field[i].textbuffer;
			field[i].sqlrtype=VARCHAR_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_SHORT ||
				outsqlda->sqlvar[i].sqltype==SQL_SHORT+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].shortbuffer;
			field[i].sqlrtype=SMALLINT_DATATYPE;

		// Looks like sometimes interbase returns INT64's as
		// SQL_LONG type.  These can be identified because
		// the sqlscale gets set too.  Treat SQL_LONG's with
		// an sqlscale as INT64's.
		} else if ((outsqlda->sqlvar[i].sqltype==SQL_LONG ||
				outsqlda->sqlvar[i].sqltype==SQL_LONG+1) &&
				outsqlda->sqlvar[i].sqlscale==0) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].longbuffer;
			field[i].sqlrtype=INTEGER_DATATYPE;
		} else if (
		#ifdef SQL_INT64
				(outsqlda->sqlvar[i].sqltype==SQL_INT64 ||
				outsqlda->sqlvar[i].sqltype==SQL_INT64+1) ||
		#endif
				((outsqlda->sqlvar[i].sqltype==SQL_LONG ||
				outsqlda->sqlvar[i].sqltype==SQL_LONG+1) &&
				outsqlda->sqlvar[i].sqlscale)) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].int64buffer;
			if (outsqlda->sqlvar[i].sqlsubtype==1) {
				field[i].sqlrtype=NUMERIC_DATATYPE;
			} else {
				field[i].sqlrtype=DECIMAL_DATATYPE;
			}
		} else if (outsqlda->sqlvar[i].sqltype==SQL_FLOAT ||
			outsqlda->sqlvar[i].sqltype==SQL_FLOAT+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].floatbuffer;
			field[i].sqlrtype=FLOAT_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_DOUBLE ||
			outsqlda->sqlvar[i].sqltype==SQL_DOUBLE+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].doublebuffer;
			field[i].sqlrtype=DOUBLE_PRECISION_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_D_FLOAT ||
			outsqlda->sqlvar[i].sqltype==SQL_D_FLOAT+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].doublebuffer;
			field[i].sqlrtype=D_FLOAT_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_ARRAY || 
				outsqlda->sqlvar[i].sqltype==SQL_ARRAY+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].quadbuffer;
			field[i].sqlrtype=ARRAY_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_QUAD || 
				outsqlda->sqlvar[i].sqltype==SQL_QUAD+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].quadbuffer;
			field[i].sqlrtype=QUAD_DATATYPE;
		#ifdef SQL_TIMESTAMP
		} else if (outsqlda->sqlvar[i].sqltype==SQL_TIMESTAMP || 
				outsqlda->sqlvar[i].sqltype==SQL_TIMESTAMP+1) {
		#else
		} else if (outsqlda->sqlvar[i].sqltype==SQL_DATE || 
				outsqlda->sqlvar[i].sqltype==SQL_DATE+1) {
		#endif
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].timestampbuffer;
			field[i].sqlrtype=TIMESTAMP_DATATYPE;
		#ifdef SQL_TIMESTAMP
		} else if (outsqlda->sqlvar[i].sqltype==SQL_TYPE_TIME || 
				outsqlda->sqlvar[i].sqltype==SQL_TYPE_TIME+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].timebuffer;
			field[i].sqlrtype=TIME_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_TYPE_DATE || 
				outsqlda->sqlvar[i].sqltype==SQL_TYPE_DATE+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].datebuffer;
			field[i].sqlrtype=DATE_DATATYPE;
		#endif
		} else if (outsqlda->sqlvar[i].sqltype==SQL_BLOB || 
				outsqlda->sqlvar[i].sqltype==SQL_BLOB+1) {
			outsqlda->sqlvar[i].sqltype=SQL_BLOB;
			outsqlda->sqlvar[i].sqldata=(char *)NULL;
			field[i].sqlrtype=BLOB_DATATYPE;
		} else {
			outsqlda->sqlvar[i].sqltype=SQL_VARYING;
			outsqlda->sqlvar[i].sqldata=field[i].textbuffer;
			field[i].sqlrtype=UNKNOWN_DATATYPE;
		}
	}

	// Execute the query, using the appropriate transaction.  
	// Commit ddl queries immediately.
	return !isc_dsql_execute(interbaseconn->error,&interbaseconn->tr,
							&stmt,1,insqlda);
}

int	interbasecursor::queryIsNotSelect() {
	return !(querytype==isc_info_sql_stmt_select);
}

int	interbasecursor::queryIsCommitOrRollback() {
	return (querytype==isc_info_sql_stmt_commit ||
		querytype==isc_info_sql_stmt_rollback);
}

char	*interbasecursor::getErrorMessage(int *liveconnection) {

	char	msg[512];
	long	*pvector=interbaseconn->error;

	// declare a buffer for the error
	if (errormsg) {
		delete errormsg;
	}
	errormsg=new stringbuffer();

	// get the status message
	while (isc_interprete(msg,&pvector)) {
		errormsg->append(msg)->append(" \n");
	}

	// get the error message
	// FIXME: vladimir commented this out why?
	long	sqlcode=isc_sqlcode(interbaseconn->error);
	isc_sql_interprete(sqlcode, msg, 512);
	errormsg->append(msg);

	*liveconnection=1;

	return errormsg->getString();
}

void	interbasecursor::returnRowCounts() {
	conn->sendRowCounts((long)-1,(long)-1);
}

void	interbasecursor::returnColumnCount() {
	conn->sendColumnCount(outsqlda->sqld);
}

void	interbasecursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	long	precision;

	// for each column...
	for (int i=0; i<outsqlda->sqld; i++) {

		if (field[i].sqlrtype==CHAR_DATATYPE) {
			precision=outsqlda->sqlvar[i].sqllen;
		} else if (field[i].sqlrtype==VARCHAR_DATATYPE) {
			precision=outsqlda->sqlvar[i].sqllen;
		} else if (field[i].sqlrtype==SMALLINT_DATATYPE) {
			precision=5;
		} else if (field[i].sqlrtype==INTEGER_DATATYPE) {
			precision=11;
		} else if (field[i].sqlrtype==NUMERIC_DATATYPE) {
			// FIXME: can be from 1 to 18
			// (oddly, scale is given as a negative number)
			precision=18+outsqlda->sqlvar[i].sqlscale;
		} else if (field[i].sqlrtype==DECIMAL_DATATYPE) {
			// FIXME: can be from 1 to 18
			// (oddly, scale is given as a negative number)
			precision=18+outsqlda->sqlvar[i].sqlscale;
		} else if (field[i].sqlrtype==FLOAT_DATATYPE) {
			precision=0;
		} else if (field[i].sqlrtype==DOUBLE_PRECISION_DATATYPE) {
			precision=0;
		} else if (field[i].sqlrtype==D_FLOAT_DATATYPE) {
			precision=0;
		} else if (field[i].sqlrtype==ARRAY_DATATYPE) {
			// not sure
			precision=0;
		} else if (field[i].sqlrtype==QUAD_DATATYPE) {
			// not sure
			precision=0;
		} else if (field[i].sqlrtype==TIMESTAMP_DATATYPE) {
			// not sure
			precision=0;
		} else if (field[i].sqlrtype==TIME_DATATYPE) {
			precision=8;
		} else if (field[i].sqlrtype==DATE_DATATYPE) {
			precision=10;
		} else if (field[i].sqlrtype==BLOB_DATATYPE) {
			precision=outsqlda->sqlvar[i].sqllen;
		} else if (field[i].sqlrtype==UNKNOWN_DATATYPE) {
			precision=outsqlda->sqlvar[i].sqllen;
		}

		// send column definition
		// (oddly, scale is given as a negative number)
		conn->sendColumnDefinition(outsqlda->sqlvar[i].sqlname,
					strlen(outsqlda->sqlvar[i].sqlname),
					field[i].sqlrtype,
					outsqlda->sqlvar[i].sqllen,
					precision,
					-outsqlda->sqlvar[i].sqlscale,0,0,0);
	}
}

int	interbasecursor::noRowsToReturn() {

	if (outsqlda->sqld==0) {
		return 1;
	}
	return 0;
}

int	interbasecursor::skipRow() {
	return fetchRow();
}

int	interbasecursor::fetchRow() {

	ISC_STATUS	retcode;
	if ((retcode=isc_dsql_fetch(interbaseconn->error,&stmt,1,outsqlda))) {
		// if retcode is 100L, then there are no more rows,
		// otherwise, there is an error... how do I handle this?
		return 0;
	}
	return 1;
}

void	interbasecursor::returnRow() {

	for (int col=0; col<outsqlda->sqld; col++) {

		// handle a null field
		if ((outsqlda->sqlvar[col].sqltype & 1) && 
			field[col].nullindicator==-1) {
			conn->sendNullField();
			continue;
		}


		// handle a non-null field
		if (outsqlda->sqlvar[col].sqltype==SQL_TEXT ||
				outsqlda->sqlvar[col].sqltype==SQL_TEXT+1) {
			int	maxlen=outsqlda->sqlvar[col].sqllen;
			int	reallen=strlen(field[col].textbuffer);
			if (reallen>maxlen) {
				reallen=maxlen;
			}
			conn->sendField(field[col].textbuffer,reallen);
		} else if (outsqlda->sqlvar[col].sqltype==SQL_SHORT ||
				outsqlda->sqlvar[col].sqltype==SQL_SHORT+1) {
			stringbuffer	buffer;
			buffer.append((long)field[col].shortbuffer);
			conn->sendField(buffer.getString(),
					strlen(buffer.getString()));
		} else if (outsqlda->sqlvar[col].sqltype==SQL_FLOAT ||
				outsqlda->sqlvar[col].sqltype==SQL_FLOAT+1) {
			stringbuffer	buffer;
			buffer.append((double)field[col].floatbuffer);
			conn->sendField(buffer.getString(),
					strlen(buffer.getString()));
		} else if (outsqlda->sqlvar[col].sqltype==SQL_DOUBLE ||
				outsqlda->sqlvar[col].sqltype==SQL_DOUBLE+1 ||
				outsqlda->sqlvar[col].sqltype==SQL_D_FLOAT ||
				outsqlda->sqlvar[col].sqltype==SQL_D_FLOAT+1) {
			stringbuffer	buffer;
			buffer.append((double)field[col].doublebuffer);
			conn->sendField(buffer.getString(),
					strlen(buffer.getString()));
		} else if (outsqlda->sqlvar[col].sqltype==SQL_VARYING ||
				outsqlda->sqlvar[col].sqltype==SQL_VARYING+1) {
			// the first 2 bytes are the length in 
			// an SQL_VARYING field
			short	size;
			memcpy((void *)&size,(void *)field[col].textbuffer,
					sizeof(short));
			conn->sendField(field[col].textbuffer+sizeof(short),
					size);

		// Looks like sometimes interbase returns INT64's as
		// SQL_LONG type.  These can be identified because
		// the sqlscale gets set too.  Treat SQL_LONG's with
		// an sqlscale as INT64's.
		} else if ((outsqlda->sqlvar[col].sqltype==SQL_LONG ||
				outsqlda->sqlvar[col].sqltype==SQL_LONG+1) &&
				outsqlda->sqlvar[col].sqlscale==0) {
			stringbuffer	buffer;
			buffer.append((long)field[col].longbuffer);
			conn->sendField(buffer.getString(),
					strlen(buffer.getString()));
		} else if (
		#ifdef SQL_INT64
				(outsqlda->sqlvar[col].sqltype==SQL_INT64 ||
				outsqlda->sqlvar[col].sqltype==SQL_INT64+1) ||
		#endif
				((outsqlda->sqlvar[col].sqltype==SQL_LONG ||
				outsqlda->sqlvar[col].sqltype==SQL_LONG+1) &&
				outsqlda->sqlvar[col].sqlscale)) {
			// int64's are weird.  To the left of the decimal
			// point is the value/10^scale, to the right is
			// value%10^scale
			stringbuffer	buffer;
			if (outsqlda->sqlvar[col].sqlscale) {

				buffer.append((long)field[col].int64buffer/(int)pow(10.0,(double)-outsqlda->sqlvar[col].sqlscale))->append(".");

				stringbuffer	decimal;
				decimal.append((long)field[col].int64buffer%(int)pow(10.0,(double)-outsqlda->sqlvar[col].sqlscale));
			
				// gotta get the right number of decimal places
				for (int i=strlen(decimal.getString());
					i<-outsqlda->sqlvar[col].sqlscale;
					i++) {
					decimal.append("0");
				}
				buffer.append(decimal.getString());
			} else {
				buffer.append((long)field[col].int64buffer);
			}
			conn->sendField(buffer.getString(),
						strlen(buffer.getString()));
		} else if (outsqlda->sqlvar[col].sqltype==SQL_ARRAY ||
			outsqlda->sqlvar[col].sqltype==SQL_ARRAY+1 ||
			outsqlda->sqlvar[col].sqltype==SQL_QUAD ||
			outsqlda->sqlvar[col].sqltype==SQL_QUAD+1) {
			// have to handle arrays for real here...
			conn->sendNullField();
		#ifdef SQL_TIMESTAMP
		} else if (outsqlda->sqlvar[col].sqltype==SQL_TIMESTAMP ||
			outsqlda->sqlvar[col].sqltype==SQL_TIMESTAMP+1) {
			// decode the timestamp
			tm	entry_timestamp;
			isc_decode_timestamp(&field[col].timestampbuffer,
							&entry_timestamp);
		#else
		} else if (outsqlda->sqlvar[col].sqltype==SQL_DATE ||
			outsqlda->sqlvar[col].sqltype==SQL_DATE+1) {
			// decode the timestamp
			tm	entry_timestamp;
			isc_decode_date(&field[col].timestampbuffer,
							&entry_timestamp);
		#endif
			// build a string of "yyyy-mm-dd hh:mm:ss" format
			char	buffer[20];
			sprintf(buffer,"%d-%02d-%02d %02d:%02d:%02d",
					entry_timestamp.tm_year+1900,
					entry_timestamp.tm_mon+1,
					entry_timestamp.tm_mday,
					entry_timestamp.tm_hour,
					entry_timestamp.tm_min,
					entry_timestamp.tm_sec);
			conn->sendField(buffer,19);
		#ifdef SQL_TIMESTAMP
		} else if (outsqlda->sqlvar[col].sqltype==SQL_TYPE_TIME ||
			outsqlda->sqlvar[col].sqltype==SQL_TYPE_TIME+1) {
			// decode the time
			tm	entry_time;
			isc_decode_sql_time(&field[col].timebuffer,
							&entry_time);
			// build a string of "hh:mm:ss" format
			char	buffer[9];
			sprintf(buffer,"%02d:%02d:%02d",
					entry_time.tm_hour,
					entry_time.tm_min,
					entry_time.tm_sec);
			conn->sendField(buffer,8);
		} else if (outsqlda->sqlvar[col].sqltype==SQL_TYPE_DATE ||
			outsqlda->sqlvar[col].sqltype==SQL_TYPE_DATE+1) {
			// decode the date
			tm	entry_date;
			isc_decode_sql_date(&field[col].datebuffer,
							&entry_date);
			// build a string of "yyyy-mm-dd" format
			char	buffer[11];
			sprintf(buffer,"%d:%02d:%02d",
					entry_date.tm_year+1900,
					entry_date.tm_mon+1,
					entry_date.tm_mday);
			conn->sendField(buffer,10);
		#endif
		} else if (outsqlda->sqlvar[col].sqltype==SQL_BLOB ||
				outsqlda->sqlvar[col].sqltype==SQL_BLOB+1) {
			// have to handle blobs for real here...
			conn->sendNullField();
		}
	}
}
