// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/rawbuffer.h>
#include <defines.h>

bool sqlrcursor::parseOutputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Receiving Output Bind Values: \n");
		sqlrc->debugPreEnd();
	}

	// useful variables
	uint16_t	type;
	uint32_t	length;
	uint16_t	count=0;

	// get the bind values
	for (;;) {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("	getting type...\n");
			sqlrc->debugPreEnd();
		}

		// get the data type
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get data type.\n "
				"A network error may have occurred.");

			return false;
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("	done getting type: ");
			sqlrc->debugPrint((int64_t)type);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	NULL output bind\n");
				sqlrc->debugPreEnd();
			}

			// handle a null value
			outbindvars[count].resultvaluesize=0;
			if (outbindvars[count].type==STRING_BIND) {
				if (returnnulls) {
					outbindvars[count].value.
							stringval=NULL;
				} else {
					outbindvars[count].value.
							stringval=new char[1];
					outbindvars[count].value.
							stringval[0]='\0';
				}
			} else if (outbindvars[count].type==INTEGER_BIND) {
				outbindvars[count].value.integerval=0;
			} else if (outbindvars[count].type==DOUBLE_BIND) {
				outbindvars[count].value.doubleval.value=0;
				outbindvars[count].value.doubleval.precision=0;
				outbindvars[count].value.doubleval.scale=0;
			} else if (outbindvars[count].type==DATE_BIND) {
				outbindvars[count].value.dateval.year=0;
				outbindvars[count].value.dateval.month=0;
				outbindvars[count].value.dateval.day=0;
				outbindvars[count].value.dateval.hour=0;
				outbindvars[count].value.dateval.minute=0;
				outbindvars[count].value.dateval.second=0;
				if (returnnulls) {
					outbindvars[count].
						value.dateval.tz=NULL;
				} else {
					outbindvars[count].
						value.dateval.tz=new char[1];
					outbindvars[count].
						value.dateval.tz[0]='\0';
				}
			} 

			if (sqlrc->debug) {
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching.\n");
			}

		} else if (type==STRING_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	STRING output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the value length
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get string value length.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].resultvaluesize=length;
			outbindvars[count].value.stringval=new char[length+1];

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		length=");
				sqlrc->debugPrint((int64_t)length);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			// get the value
			if ((uint32_t)getString(outbindvars[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.stringval[length]='\0';

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==INTEGER_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	INTEGER output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the value
			if (getLongLong((uint64_t *)&outbindvars[count].
					value.integerval)!=sizeof(uint64_t)) {
				setError("Failed to get integer value.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==DOUBLE_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	DOUBLE output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the value
			if (getDouble(&outbindvars[count].value.
						doubleval.value)!=
						sizeof(double)) {
				setError("Failed to get double value.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the precision
			if (getLong(&outbindvars[count].value.
						doubleval.precision)!=
						sizeof(uint32_t)) {
				setError("Failed to get precision.\n "
					"A network error may have occurred.");
				return false;
			}

			// get the scale
			if (getLong(&outbindvars[count].value.
						doubleval.scale)!=
						sizeof(uint32_t)) {
				setError("Failed to get scale.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==DATE_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	DATE output bind\n");
				sqlrc->debugPreEnd();
			}

			uint16_t	temp;

			// get the year
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.year=(int16_t)temp;

			// get the month
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.month=(int16_t)temp;

			// get the day
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.day=(int16_t)temp;

			// get the hour
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.hour=(int16_t)temp;

			// get the minute
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.minute=(int16_t)temp;

			// get the second
			if (getShort(&temp)!=sizeof(uint16_t)) {
				setError("Failed to get long value.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.second=(int16_t)temp;

			// get the timezone length
			uint16_t	length;
			if (getShort(&length)!=sizeof(uint16_t)) {
				setError("Failed to get timezone length.\n "
					"A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.dateval.tz=new char[length+1];

			// get the timezone
			if ((uint16_t)getString(outbindvars[count].value.
						dateval.tz,length)!=length) {
				setError("Failed to get timezone.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else if (type==CURSOR_DATA) {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	CURSOR output bind\n");
				sqlrc->debugPreEnd();
			}

			// get the cursor id
			if (getShort((uint16_t *)
					&(outbindvars[count].value.cursorid))!=
						sizeof(uint16_t)) {
				setError("Failed to get cursor id.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching\n");
				sqlrc->debugPreEnd();
			}

		} else {

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("	LOB/CLOB ");
				sqlrc->debugPrint("output bind\n");
				sqlrc->debugPreEnd();
			}

			// must be START_LONG_DATA...
			// get the total length of the long data
			uint64_t	totallength;
			if (getLongLong(&totallength)!=sizeof(uint64_t)) {
				setError("Failed to get total length.\n "
					"A network error may have occurred.");
				return false;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		length=");
				sqlrc->debugPrint((int64_t)totallength);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

			// create a buffer to hold the data
			char	*buffer=new char[totallength+1];

			uint64_t	offset=0;
			uint32_t	length;
			for (;;) {

				if (sqlrc->debug) {
					sqlrc->debugPreStart();
					sqlrc->debugPrint("		");
					sqlrc->debugPrint("fetching...\n");
					sqlrc->debugPreEnd();
				}

				// get the type of the chunk
				if (getShort(&type)!=sizeof(uint16_t)) {
					delete[] buffer;
					setError("Failed to get chunk type.\n "
						"A network error may have "
						"occurred.");
					return false;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(uint32_t)) {
					delete[] buffer;
					setError("Failed to get chunk length.\n"
						" A network error may have "
						"occurred.");
					return false;
				}

				// get the chunk of data
				if ((uint32_t)getString(buffer+offset,
							length)!=length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n "
						"A network error may have "
						"occurred.");
					return false;
				}

				offset=offset+length;
			}

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("		");
				sqlrc->debugPrint("done fetching.\n");
				sqlrc->debugPreEnd();
			}

			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getOutputBindLength.
			buffer[totallength]='\0';
			outbindvars[count].value.lobval=buffer;
			outbindvars[count].resultvaluesize=totallength;
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(outbindvars[count].variable);
			sqlrc->debugPrint("=");
			if (outbindvars[count].type==BLOB_BIND) {
				sqlrc->debugPrintBlob(
					outbindvars[count].value.lobval,
					outbindvars[count].resultvaluesize);
			} else if (outbindvars[count].type==CLOB_BIND) {
				sqlrc->debugPrintClob(
					outbindvars[count].value.lobval,
					outbindvars[count].resultvaluesize);
			} else if (outbindvars[count].type==CURSOR_BIND) {
				sqlrc->debugPrint((int64_t)outbindvars[count].
								value.cursorid);
			} else if (outbindvars[count].type==INTEGER_BIND) {
				sqlrc->debugPrint(outbindvars[count].
							value.integerval);
			} else if (outbindvars[count].type==DOUBLE_BIND) {
				sqlrc->debugPrint(outbindvars[count].
						value.doubleval.value);
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)outbindvars[count].
						value.doubleval.precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((int64_t)outbindvars[count].
						value.doubleval.scale);
				sqlrc->debugPrint(")");
			} else {
				sqlrc->debugPrint(outbindvars[count].
							value.stringval);
			}
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		count++;
	}

	// cache the output binds
	cacheOutputBinds(count);

	return true;
}
