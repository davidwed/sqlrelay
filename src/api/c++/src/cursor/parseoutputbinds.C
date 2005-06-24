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
	unsigned short	type;
	unsigned long	length;
	int	count=0;

	// get the bind values
	for (;;) {

		// get the data type
		if (getShort(&type)!=sizeof(unsigned short)) {
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
			if (getLong(&length)!=sizeof(unsigned long)) {
				setError("Failed to get string value length.\n A network error may have occurred.");
				return false;
			}
			outbindvars[count].valuesize=length;
			outbindvars[count].value.stringval=new char[length+1];

			// get the value
			if ((unsigned long)getString(outbindvars[count].value.
						stringval,length)!=length) {
				setError("Failed to get string value.\n A network error may have occurred.");
				return false;
			}
			outbindvars[count].value.stringval[length]=(char)NULL;

		} else if (type==CURSOR_DATA) {

			// get the cursor id
			if (getShort((unsigned short *)
					&(outbindvars[count].value.cursorid))!=
						sizeof(unsigned short)) {
				setError("Failed to get cursor id.\n A network error may have occurred.");
				return false;
			}

		} else {

			unsigned long	totallength=0;
			if (getLong(&totallength)!=sizeof(unsigned long)) {
				setError("Failed to get total length.\n A network error may have occurred.");
				return false;
			}

			// create a buffer to hold the data
			char	*buffer=new char[totallength+1];

			unsigned long	offset=0;
			unsigned long	length;
			for (;;) {

				// get the type of the chunk
				if (getShort(&type)!=
						sizeof(unsigned short)) {
					delete[] buffer;
					setError("Failed to get chunk type.\n A network error may have occurred.");
					return false;
				}

				// check to see if we're done
				if (type==END_LONG_DATA) {
					break;
				}

				// get the length of the chunk
				if (getLong(&length)!=sizeof(unsigned long)) {
					delete[] buffer;
					setError("Failed to get chunk length.\n A network error may have occurred.");
					return false;
				}

				// get the chunk of data
				if ((unsigned long)getString(buffer+offset,
							length)!=length) {
					delete[] buffer;
					setError("Failed to get chunk data.\n A network error may have occurred.");
					return false;
				}

				offset=offset+length;
			}
			// NULL terminate the buffer.  This makes 
			// certain operations safer and won't hurt
			// since the actual length (which doesn't
			// include the NULL) is available from
			// getOutputBindLength.
			buffer[totallength]=(char)NULL;
			outbindvars[count].value.lobval=buffer;
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

	return true;
}
