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
	int	count=0;

	// get the bind values
	for (;;) {

		// get the data type
		if (getShort(&type)!=sizeof(uint16_t)) {
			setError("Failed to get data type.\n A network error may have occurred.");

			return false;
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
			if (getLong(&length)!=sizeof(uint32_t)) {
				setError("Failed to get string value length.\n A network error may have occurred.");
				return false;
			}
			outbindvars[count].valuesize=length;
			outbindvars[count].value.stringval=new char[length+1];

			// get the value
			if ((uint32_t)getString(outbindvars[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.stringval[length]=(char)NULL;

		} else if (type==CURSOR_DATA) {

			// get the cursor id
			if (getShort((uint16_t *)
					&(outbindvars[count].value.cursorid))!=
						sizeof(uint16_t)) {
				setError("Failed to get cursor id.\n A network error may have occurred.");
				return false;
			}

		} else {

			char	*buffer=NULL;
			char	*oldbuffer=NULL;
			uint32_t	totallength=0;
			uint32_t	length;
			for (;;) {

				// get the type of the chunk
				if (getShort(&type)!=sizeof(uint16_t)) {
					setError("Failed to get chunk type.\n A network error may have occurred.");
					return false;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(uint32_t)) {
					delete[] buffer;
					setError("Failed to get chunk length.\n A network error may have occurred.");
					return false;
				}

				// create a buffer to hold the chunk
				buffer=new char[totallength+length+1];
				if (totallength) {
					rawbuffer::copy(buffer,oldbuffer,
								totallength);
					delete[] oldbuffer;
					oldbuffer=buffer;
					buffer=buffer+totallength;
				} else {
					oldbuffer=buffer;
				}
				totallength=totallength+length;

				// get the chunk of data
				if ((uint32_t)getString(buffer,length)!=
								length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n A network error may have occurred.");
					return false;
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
				sqlrc->debugPrint((int32_t)outbindvars[count].
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

	return true;
}
