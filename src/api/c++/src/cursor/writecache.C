// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/permissions.h>
#include <rudiments/datetime.h>
#include <defines.h>
#include <datatypes.h>

void sqlrcursor::cacheToFile(const char *filename) {

	cacheon=true;
	cachettl=600;
	if (copyrefs) {
		delete[] cachedestname;
		cachedestname=charstring::duplicate(filename);
	} else {
		cachedestname=(char *)filename;
	}

	// create the index name
	delete[] cachedestindname;
	size_t	cachedestindnamelen=charstring::length(filename)+5;
	cachedestindname=new char[cachedestindnamelen];
	snprintf(cachedestindname,cachedestindnamelen,"%s.ind",filename);
}

void sqlrcursor::setCacheTtl(uint32_t ttl) {
	cachettl=ttl;
}

const char *sqlrcursor::getCacheFileName() {
	return cachedestname;
}

void sqlrcursor::cacheOff() {
	cacheon=false;
}

void sqlrcursor::startCaching() {

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
			datetime	dt;
			dt.getSystemDateAndTime();
			int32_t	expiration=dt.getEpoch()+cachettl;
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

void sqlrcursor::cacheError() {

	if (resumed || !cachedest) {
		return;
	}

	// write the number of returned rows, affected rows 
	// and a zero to terminate the column descriptions
	cachedest->write((uint16_t)NO_ACTUAL_ROWS);
	cachedest->write((uint16_t)NO_AFFECTED_ROWS);
	cachedest->write((uint16_t)END_COLUMN_INFO);
}

void sqlrcursor::cacheNoError() {

	if (resumed || !cachedest) {
		return;
	}

	cachedest->write((uint16_t)NO_ERROR);
}

void sqlrcursor::cacheColumnInfo() {

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
		uint16_t	namelen;
		column		*whichcolumn;
		for (uint32_t i=0; i<colcount; i++) {

			// get the column
			whichcolumn=getColumnInternal(i);

			// write the name
			namelen=charstring::length(whichcolumn->name);
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

void sqlrcursor::cacheOutputBinds(uint32_t count) {

	if (resumed || !cachedest) {
		return;
	}

	// write the variable/value pairs to the cache file
	uint16_t	len;
	for (uint32_t i=0; i<count; i++) {

		cachedest->write(outbindvars[i].type);

		len=charstring::length(outbindvars[i].variable);
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
	cachedest->write((uint16_t)END_BIND_VARS);
}

void sqlrcursor::cacheData() {

	if (!cachedest) {
		return;
	}

	// write the data to the cache file
	uint32_t	rowbuffercount=rowcount-firstrowindex;
	for (uint32_t i=0; i<rowbuffercount; i++) {

		// get the current offset in the cache destination file
		int64_t	position=cachedest->getCurrentPosition();

		// seek to the right place in the index file and write the
		// destination file offset
		cachedestind->setPositionRelativeToBeginning(
			13+sizeof(int32_t)+((firstrowindex+i)*sizeof(int64_t)));
		cachedestind->write(position);

		// write the row to the cache file
		for (uint32_t j=0; j<colcount; j++) {
			uint16_t	type;
			int32_t		len;
			char		*field=getFieldInternal(i,j);
			if (field) {
				type=NORMAL_DATA;
				len=charstring::length(field);
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

void sqlrcursor::finishCaching() {

	if (!cachedest) {
		return;
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Finishing caching.\n");
		sqlrc->debugPreEnd();
	}

	// terminate the result set
	cachedest->write((uint16_t)END_RESULT_SET);

	// close the cache file and clean up
	clearCacheDest();
}

void sqlrcursor::clearCacheDest() {

	// close the cache file and clean up
	if (cachedest) {
		cachedest->close();
		delete cachedest;
		cachedest=NULL;
		cachedestind->close();
		delete cachedestind;
		cachedestind=NULL;
		cacheon=false;
	}
}
