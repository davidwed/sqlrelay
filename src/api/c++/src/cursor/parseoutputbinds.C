// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int sqlrcursor::parseOutputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Receiving Output Bind Values: \n");
		sqlrc->debugPreEnd();
	}

	// useful variables
	unsigned short	type;
	unsigned long	length;
	int	count=0;

	// get the bind values
	for (;;) {

		// get the data type
		if (getShort(&type)!=sizeof(unsigned short)) {
			return -1;
		}

		// check for end of bind values
		if (type==END_BIND_VARS) {

			break;

		} else if (type==NULL_DATA) {

			// handle a null value
			if (returnnulls) {
				outbindvars[count].value.stringval=NULL;
			} else {
				outbindvars[count].value.stringval=new char[1];
				outbindvars[count].value.stringval[0]=
								(char)NULL;
			}

		} else if (type==NORMAL_DATA) {

			// get the value length
			if (getLong(&length)!=sizeof(unsigned long)) {
				return -1;
			}
			outbindvars[count].valuesize=length;
			outbindvars[count].value.stringval=new char[length+1];
			if ((unsigned long)getString(outbindvars[count].value.
						stringval,length)!=length) {
				return -1;
			}
			outbindvars[count].value.stringval[length]=(char)NULL;

		} else if (type==CURSOR_DATA) {

			if (getShort((unsigned short *)
					&(outbindvars[count].value.cursorid))!=
						sizeof(unsigned short)) {
				return -1;
			}

		} else {

			char	*buffer=NULL;
			char	*oldbuffer=NULL;
			unsigned long	totallength=0;
			unsigned long	length;
			while (1) {

				// get the type of the chunk
				if (getShort(&type)!=
						sizeof(unsigned short)) {
					return -1;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(unsigned long)) {
					delete[] buffer;
					return -1;
				}

				buffer=new char[totallength+length+1];
				if (totallength) {
					memcpy(buffer,oldbuffer,totallength);
					delete[] oldbuffer;
					oldbuffer=buffer;
					buffer=buffer+totallength;
				} else {
					oldbuffer=buffer;
				}
				totallength=totallength+length;

				if ((unsigned long)getString(buffer,length)!=
								length) {
					delete[] buffer;
					return -1;
				}

				// NULL terminate the buffer.  This makes 
				// certain operations safer and won't hurt
				// since the actual length (which doesn't
				// include the NULL) is available from
				// getOutputBindLength.
				buffer[length]=(char)NULL;
			}
			outbindvars[count].value.lobval=oldbuffer;
			outbindvars[count].valuesize=totallength;
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(outbindvars[count].variable);
			sqlrc->debugPrint("=");
			if (outbindvars[count].type==BLOB_BIND) {
				sqlrc->debugPrintBlob(
					outbindvars[count].value.lobval,
					outbindvars[count].valuesize);
			} else if (outbindvars[count].type==CLOB_BIND) {
				sqlrc->debugPrintClob(
					outbindvars[count].value.lobval,
					outbindvars[count].valuesize);
			} else if (outbindvars[count].type==CURSOR_BIND) {
				sqlrc->debugPrint((long)outbindvars[count].
								value.cursorid);
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

	return 1;
}
