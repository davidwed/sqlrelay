// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <ibase.h>

#include "../../src/common/datatypes.h"

#include <iomanip.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define MAX_ITEM_BUFFER_SIZE 4096
#define MAX_SELECT_LIST_SIZE 256

struct fieldstruct {
	short		type;

	short		shortbuffer;
	long		longbuffer;
	float		floatbuffer;
	double		doublebuffer;
	ISC_QUAD	quadbuffer;
	ISC_DATE	datebuffer;
	ISC_TIME	timebuffer;
	ISC_TIMESTAMP	timestampbuffer;
	ISC_INT64	int64buffer;
	char		textbuffer[MAX_ITEM_BUFFER_SIZE+1];

	short		nullindicator;
};

char		dpb[256];
short		dpblength;
isc_db_handle	db;
isc_tr_handle	tr;
isc_stmt_handle	stmt;
XSQLDA	ISC_FAR	*outsqlda;
ISC_BLOB_DESC	to_desc;

fieldstruct	field[MAX_SELECT_LIST_SIZE];

int main(int argc, char **argv) {

	if (argc<5) {
		printf("usage: interbasetest database query iterations\n");
		exit(0);
	}

	char	*database=argv[1];
	char	*query=argv[2];
	int	iterations=atoi(argv[3]);


	time_t	starttime=time(NULL);
	printf("interbasetest running, please wait...\n");
	clock();


	for (int count=0; count<iterations; count++) {

		db=0L;
		tr=0L;

		outsqlda=(XSQLDA ISC_FAR *)malloc
					(XSQLDA_LENGTH(MAX_SELECT_LIST_SIZE));
		outsqlda->version=SQLDA_VERSION1;
		outsqlda->sqln=MAX_SELECT_LIST_SIZE;

		int	querytype=0;

		isc_attach_database(NULL,strlen(database),database,
							&db,dpblength,NULL);
		isc_start_transaction(NULL,&tr,1,&db,0,NULL);
	
		stmt=NULL;
		isc_dsql_allocate_statement(NULL,&db,&stmt);
		isc_dsql_prepare(NULL,&tr,&stmt,strlen(query),
						query,3,outsqlda);
	
		// describe the statement
		outsqlda->sqld=0;
		isc_dsql_describe(NULL,&stmt,1,outsqlda);
		if (outsqlda->sqld>MAX_SELECT_LIST_SIZE) {
			outsqlda->sqld=MAX_SELECT_LIST_SIZE;
		}
	
		for (int i=0; i<outsqlda->sqld; i++) {
	
			// save the actual field type
			field[i].type=outsqlda->sqlvar[i].sqltype;
	
			// handle the null indicator
			outsqlda->sqlvar[i].sqlind=&field[i].nullindicator;
	
			// coerce the datatypes and point where the data 
			// should go
			if (outsqlda->sqlvar[i].sqltype==SQL_TEXT || 
				outsqlda->sqlvar[i].sqltype==SQL_TEXT+1) {
				outsqlda->sqlvar[i].sqldata=
						field[i].textbuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_VARYING ||
				outsqlda->sqlvar[i].sqltype==SQL_VARYING+1) {
				outsqlda->sqlvar[i].sqldata=
						field[i].textbuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_SHORT ||
				outsqlda->sqlvar[i].sqltype==SQL_SHORT+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].shortbuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_LONG || 
				outsqlda->sqlvar[i].sqltype==SQL_LONG+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].longbuffer;
			#ifdef SQL_INT64
			} else if (outsqlda->sqlvar[i].sqltype==SQL_INT64 || 
				outsqlda->sqlvar[i].sqltype==SQL_INT64+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].int64buffer;
			#endif
			} else if (outsqlda->sqlvar[i].sqltype==SQL_FLOAT ||
				outsqlda->sqlvar[i].sqltype==SQL_FLOAT+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].floatbuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_DOUBLE ||
				outsqlda->sqlvar[i].sqltype==SQL_DOUBLE+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].doublebuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_D_FLOAT ||
				outsqlda->sqlvar[i].sqltype==SQL_D_FLOAT+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].doublebuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_ARRAY || 
				outsqlda->sqlvar[i].sqltype==SQL_ARRAY+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].quadbuffer;
			} else if (outsqlda->sqlvar[i].sqltype==SQL_QUAD || 
				outsqlda->sqlvar[i].sqltype==SQL_QUAD+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].quadbuffer;
			#ifdef SQL_TIMESTAMP
			} else if (
				outsqlda->sqlvar[i].sqltype==SQL_TIMESTAMP || 
				outsqlda->sqlvar[i].sqltype==SQL_TIMESTAMP+1) {
			#else
			} else if (outsqlda->sqlvar[i].sqltype==SQL_DATE || 
				outsqlda->sqlvar[i].sqltype==SQL_DATE+1) {
			#endif
				outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].timestampbuffer;
			#ifdef SQL_TIMESTAMP
			} else if (
				outsqlda->sqlvar[i].sqltype==SQL_TYPE_TIME || 
				outsqlda->sqlvar[i].sqltype==SQL_TYPE_TIME+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].timebuffer;
			} else if (
				outsqlda->sqlvar[i].sqltype==SQL_TYPE_DATE || 
				outsqlda->sqlvar[i].sqltype==SQL_TYPE_DATE+1) {
				outsqlda->sqlvar[i].sqldata=
						(char *)&field[i].datebuffer;
			#endif
			} else if (outsqlda->sqlvar[i].sqltype==SQL_BLOB || 
				outsqlda->sqlvar[i].sqltype==SQL_BLOB+1) {
				outsqlda->sqlvar[i].sqltype=SQL_BLOB;
				outsqlda->sqlvar[i].sqldata=(char *)NULL;
			} else {
				outsqlda->sqlvar[i].sqltype=SQL_VARYING;
				outsqlda->sqlvar[i].sqldata=
						field[i].textbuffer;
			}
		}
	
		// execute the query
		isc_dsql_execute(NULL,&tr,&stmt,1,NULL);
	
		// fetch a row
		while (!isc_dsql_fetch(NULL,&stmt,1,outsqlda)) {
	
			for (int col=0; col<outsqlda->sqld; col++) {
	
				// handle a null field
				if ((outsqlda->sqlvar[col].sqltype & 1) && 
					field[col].nullindicator==-1) {
					printf("NULL,");
					continue;
				}
		
				// handle a non-null field
				if (outsqlda->sqlvar[col].sqltype==SQL_TEXT ||
					outsqlda->sqlvar[col].sqltype==
								SQL_TEXT+1) {
					// text fields are easy
					printf("\"");
					printf("%s",field[col].textbuffer);
					printf("\",");
				} else if (outsqlda->sqlvar[col].sqltype==
								SQL_SHORT ||
					outsqlda->sqlvar[col].sqltype==
								SQL_SHORT+1) {
					printf("%d,",field[col].shortbuffer);
				} else if (outsqlda->sqlvar[col].sqltype==
								SQL_LONG ||
					outsqlda->sqlvar[col].sqltype==
								SQL_LONG+1) {
					printf("%d,",field[col].longbuffer);
				} else if (outsqlda->sqlvar[col].sqltype==
								SQL_FLOAT ||
					outsqlda->sqlvar[col].sqltype==
								SQL_FLOAT+1) {
					printf("%0.10f,",
						field[col].floatbuffer);
				} else if (outsqlda->sqlvar[col].sqltype==
							SQL_DOUBLE ||
					outsqlda->sqlvar[col].sqltype==
							SQL_DOUBLE+1 ||
					outsqlda->sqlvar[col].sqltype==
							SQL_D_FLOAT ||
					outsqlda->sqlvar[col].sqltype==
							SQL_D_FLOAT+1) {
					printf("%0.20f,",
						field[col].doublebuffer);
				} else if (
					outsqlda->sqlvar[col].sqltype==
							SQL_VARYING ||
					outsqlda->sqlvar[col].sqltype==
							SQL_VARYING+1) {
					// the first 2 bytes are the length in 
					// an SQL_VARYING field
					short	size;
					memcpy((void *)&size,
						(void *)field[col].textbuffer,
						sizeof(short));
					printf("\"");
					for (int i=0; i<size; i++) {
						printf("%c",(char)field[col].
							textbuffer+
							sizeof(short)+i);
					}
					printf("\",");
				#ifdef SQL_INT64
				} else if (outsqlda->sqlvar[col].sqltype==
							SQL_INT64 ||
					outsqlda->sqlvar[col].sqltype==
							SQL_INT64+1) {
					// int64's are weird.  To the left of 
					// the decimal point is the 
					// value/10^scale, to the
					// right is value%10^scale
					if (outsqlda->sqlvar[col].sqlscale) {
		
						printf("%d.",field[col].int64buffer/(int)pow(10.0,(double)-outsqlda->sqlvar[col].sqlscale));
					
						printf("%d",field[col].int64buffer%(int)pow(10.0,(double)-outsqlda->sqlvar[col].sqlscale));
					} else {
						printf("%d",field[col].
								int64buffer);
					}
					printf(",");
				#endif
				} else if (outsqlda->sqlvar[col].sqltype==
								SQL_ARRAY ||
					outsqlda->sqlvar[col].sqltype==
								SQL_ARRAY+1 ||
					outsqlda->sqlvar[col].sqltype==
								SQL_QUAD ||
					outsqlda->sqlvar[col].sqltype==
								SQL_QUAD+1) {
					// have to handle arrays for 
					// real here...
				#ifdef SQL_TIMESTAMP
				} else if (
					outsqlda->sqlvar[col].sqltype==
							SQL_TIMESTAMP ||
					outsqlda->sqlvar[col].sqltype==
							SQL_TIMESTAMP+1) {
					// decode the timestamp
					tm	entry_timestamp;
					isc_decode_timestamp(
						&field[col].timestampbuffer,
						&entry_timestamp);
				#else
				} else if (outsqlda->sqlvar[col].sqltype==
							SQL_DATE ||
					outsqlda->sqlvar[col].sqltype==
							SQL_DATE+1) {
					// decode the timestamp
					tm	entry_timestamp;
					isc_decode_date(
						&field[col].timestampbuffer,
						&entry_timestamp);
				#endif
					// build a string of 
					// "yyyy-mm-dd hh:mm:ss" 
					// format
					printf("%d-",entry_timestamp.tm_year+1900);
					printf("%02d-",entry_timestamp.tm_mon+1);
					printf("%02d ",entry_timestamp.tm_mday);
					printf("%02d:",entry_timestamp.tm_hour);
					printf("%02d:",entry_timestamp.tm_min);
					printf("%02d,",entry_timestamp.tm_sec);
				#ifdef SQL_TIMESTAMP
				} else if (
					outsqlda->sqlvar[col].sqltype==
							SQL_TYPE_TIME ||
					outsqlda->sqlvar[col].sqltype==
							SQL_TYPE_TIME+1) {
					// decode the time
					tm	entry_time;
					isc_decode_sql_time(
						&field[col].timebuffer,
						&entry_time);
					printf("%02d:",entry_time.tm_hour);
					printf("%02d:",entry_time.tm_min);
					printf("%02d,",entry_time.tm_sec);
				} else if (
					outsqlda->sqlvar[col].sqltype==
							SQL_TYPE_DATE ||
					outsqlda->sqlvar[col].sqltype==
							SQL_TYPE_DATE+1) {
					// decode the date
					tm	entry_date;
					isc_decode_sql_date(
						&field[col].datebuffer,
						&entry_date);
					// build a string of "yyyy-mm-dd" 
					// format
					printf("%d-",entry_date.tm_year+1900);
					printf("%02d-",entry_date.tm_mon+1);
					printf("%02d ",entry_date.tm_mday);
				#endif
				} else if (outsqlda->sqlvar[col].sqltype==
								SQL_BLOB ||
					outsqlda->sqlvar[col].sqltype==
								SQL_BLOB+1) {
					// have to handle blobs for 
					// real here...
				}
			}
			cout << ENDL;
		}
		
		isc_dsql_free_statement(NULL,&stmt,DSQL_drop);
	
		isc_detach_database(NULL,&db);
		free(outsqlda);
	}
}
