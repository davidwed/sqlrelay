// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/permissions.h>
#include <time.h>
#include <defines.h>
#include <datatypes.h>

void	sqlrcursor::cacheToFile(const char *filename) {

	cacheon=1;
	cachettl=600;
	if (copyrefs) {
		delete[] cachedestname;
		cachedestname=strdup(filename);
	} else {
		cachedestname=(char *)filename;
	}

	// create the index name
	delete[] cachedestindname;
	cachedestindname=new char[strlen(filename)+5];
	sprintf(cachedestindname,"%s.ind",filename);
}

void	sqlrcursor::setCacheTtl(int ttl) {
	cachettl=ttl;
}

char	*sqlrcursor::getCacheFileName() {
	return cachedestname;
}

void	sqlrcursor::cacheOff() {
	cacheon=0;
}

void	sqlrcursor::startCaching() {

	if (!resumed) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Resuming caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}

	// create the cache file, truncate it unless we're 
	// resuming a previous session
	cachedest=new file();
	cachedestind=new file();
	if (!resumed) {
		cachedest->open(cachedestname,O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
		cachedestind->open(cachedestindname,O_RDWR|O_TRUNC|O_CREAT,
					permissions::ownerReadWrite());
	} else {
		cachedest->open(cachedestname,O_RDWR|O_CREAT|O_APPEND);
		cachedestind->open(cachedestindname,O_RDWR|O_CREAT|O_APPEND);
	}

	if (cachedest && cachedestind) {

		if (!resumed) {

			// write "magic" identifier to head of files
			cachedest->write("SQLRELAYCACHE",13);
			cachedestind->write("SQLRELAYCACHE",13);
			
			// write ttl to files
			long	expiration=time(NULL)+cachettl;
			cachedest->write(expiration);
			cachedestind->write(expiration);
		}

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Error caching data to ");
			sqlrc->debugPrint(cachedestname);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// in case of an error, clean up
		clearCacheDest();
	}
}

void	sqlrcursor::cacheError() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows, affected rows 
	// and a zero to terminate the column descriptions
	cachedest->write((unsigned short)NO_ACTUAL_ROWS);
	cachedest->write((unsigned short)NO_AFFECTED_ROWS);
	cachedest->write((unsigned short)END_COLUMN_INFO);
}

void	sqlrcursor::cacheNoError() {

	if (resumed || !cachedest) {
		return;
	}

	cachedest->write((unsigned short)NO_ERROR);
}

void	sqlrcursor::cacheColumnInfo() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows
	cachedest->write(knowsactualrows);
	if (knowsactualrows==ACTUAL_ROWS) {
		cachedest->write(actualrows);
	}

	// write the number of affected rows
	cachedest->write(knowsaffectedrows);
	if (knowsaffectedrows==AFFECTED_ROWS) {
		cachedest->write(affectedrows);
	}

	// write whether or not the column info is is cached
	cachedest->write(sentcolumninfo);

	// write the column count
	cachedest->write(colcount);

	// write column descriptions to the cache file
	if (sendcolumninfo==SEND_COLUMN_INFO && 
			sentcolumninfo==SEND_COLUMN_INFO) {

		// write column type format
		cachedest->write(columntypeformat);

		// write the columns themselves
		unsigned short	namelen;
		column	*whichcolumn;
		for (unsigned long i=0; i<colcount; i++) {

			// get the column
			whichcolumn=getColumnInternal(i);

			// write the name
			namelen=strlen(whichcolumn->name);
			cachedest->write(namelen);
			cachedest->write(whichcolumn->name,namelen);

			// write the type
			if (columntypeformat==COLUMN_TYPE_IDS) {
				cachedest->write(whichcolumn->type);
			} else {
				cachedest->write(whichcolumn->typestringlength);
				cachedest->write(whichcolumn->typestring,
						whichcolumn->typestringlength);
			}

			// write the length, precision and scale
			cachedest->write(whichcolumn->length);
			cachedest->write(whichcolumn->precision);
			cachedest->write(whichcolumn->scale);

			// write the flags
			cachedest->write(whichcolumn->nullable);
			cachedest->write(whichcolumn->primarykey);
			cachedest->write(whichcolumn->unique);
			cachedest->write(whichcolumn->partofkey);
			cachedest->write(whichcolumn->unsignednumber);
			cachedest->write(whichcolumn->zerofill);
			cachedest->write(whichcolumn->binary);
			cachedest->write(whichcolumn->autoincrement);
		}
	}
}

void	sqlrcursor::cacheOutputBinds(int count) {

	if (resumed || !cachedest) {
		return;
	}

	// write the variable/value pairs to the cache file
	unsigned short	len;
	for (int i=0; i<count; i++) {

		cachedest->write((unsigned short)outbindvars[i].type);

		len=strlen(outbindvars[i].variable);
		cachedest->write(len);
		cachedest->write(outbindvars[i].variable,len);

		len=outbindvars[i].valuesize;
		cachedest->write(len);
		if (outbindvars[i].type==STRING_BIND) {
			cachedest->write(outbindvars[i].value.stringval,len);
		} else if (outbindvars[i].type!=NULL_BIND) {
			cachedest->write(outbindvars[i].value.lobval,len);
		}
	}

	// terminate the list of output binds
	cachedest->write((unsigned short)END_BIND_VARS);
}

void	sqlrcursor::cacheData() {

	if (!cachedest) {
		return;
	}

	// write the data to the cache file
	int	rowbuffercount=rowcount-firstrowindex;
	for (int i=0; i<rowbuffercount; i++) {

		// get the current offset in the cache destination file
		long	position=cachedest->getCurrentPosition();

		// seek to the right place in the index file and write the
		// destination file offset
		cachedestind->setPositionRelativeToBeginning(
			13+sizeof(long)+((firstrowindex+i)*sizeof(long)));
		cachedestind->write(position);

		// write the row to the cache file
		for (unsigned long j=0; j<colcount; j++) {
			unsigned short	type;
			long	len;
			char	*field=getFieldInternal(i,j);
			if (field) {
				type=NORMAL_DATA;
				len=strlen(field);
				cachedest->write(type);
				cachedest->write(len);
				if (len>0) {
					cachedest->write(field);
				}
			} else {
				type=NULL_DATA;
				cachedest->write(type);
			}
		}
	}

	if (endofresultset) {
		finishCaching();
	}
}

void	sqlrcursor::finishCaching() {

	if (!cachedest) {
		return;
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Finishing caching.\n");
		sqlrc->debugPreEnd();
	}

	// terminate the result set
	cachedest->write((unsigned short)END_RESULT_SET);

	// close the cache file and clean up
	clearCacheDest();
}

void	sqlrcursor::clearCacheDest() {

	// close the cache file and clean up
	if (cachedest) {
		cachedest->close();
		delete cachedest;
		cachedest=NULL;
		cachedestind->close();
		delete cachedestind;
		cachedestind=NULL;
		cacheon=0;
	}
}
