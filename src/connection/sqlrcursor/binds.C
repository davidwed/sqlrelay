// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrcursor::handleBinds() {
	
	// iterate through the arrays, binding values to variables
	for (int i=0; i<inbindcount; i++) {

		// bind the value to the variable
		if (inbindvars[i].type==STRING_BIND ||
				inbindvars[i].type==NULL_BIND) {
			if (!inputBindString(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return 0;
			}
		} else if (inbindvars[i].type==LONG_BIND) {
			if (!inputBindLong(inbindvars[i].variable,
				inbindvars[i].variablesize,
				(unsigned long *)
					&inbindvars[i].value.longval)) {
				return 0;
			}
		} else if (inbindvars[i].type==DOUBLE_BIND) {
			if (!inputBindDouble(inbindvars[i].variable,
				inbindvars[i].variablesize,
				&inbindvars[i].value.doubleval.value,
				inbindvars[i].value.doubleval.precision,
				inbindvars[i].value.doubleval.scale)) {
				return 0;
			}
		} else if (inbindvars[i].type==BLOB_BIND) {
			if (!inputBindBlob(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return 0;
			}
		} else if (inbindvars[i].type==CLOB_BIND) {
			if (!inputBindClob(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return 0;
			}
		}
	}
	for (int i=0; i<outbindcount; i++) {

		// bind the value to the variable
		if (outbindvars[i].type==STRING_BIND) {
			if (!outputBindString(outbindvars[i].variable,
					outbindvars[i].variablesize,
					outbindvars[i].value.stringval,
					outbindvars[i].valuesize+1,
					&outbindvars[i].isnull)) {
				return 0;
			}
		} else if (outbindvars[i].type==BLOB_BIND) {
			if (!outputBindBlob(outbindvars[i].variable,
					outbindvars[i].variablesize,i,
					&outbindvars[i].isnull)) {
				return 0;
			}
		} else if (outbindvars[i].type==CLOB_BIND) {
			if (!outputBindClob(outbindvars[i].variable,
					outbindvars[i].variablesize,i,
					&outbindvars[i].isnull)) {
				return 0;
			}
		} else if (outbindvars[i].type==CURSOR_BIND) {
			if (!outputBindCursor(outbindvars[i].variable,
						outbindvars[i].variablesize,
						conn->cur[outbindvars[i].value.
								cursorid])) {
				return 0;
			}
		}
	}
	return 1;
}

int	sqlrcursor::inputBindString(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned short valuesize,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindLong(const char *variable,
					unsigned short variablesize,
					unsigned long *value) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindDouble(const char *variable,
					unsigned short variablesize,
					double *value, 
					unsigned short precision,
					unsigned short scale) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindBlob(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned long valuesize,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::inputBindClob(const char *variable,
					unsigned short variablesize,
					const char *value,
					unsigned long valuesize,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindString(const char *variable,
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindBlob(const char *variable,
					unsigned short variablesize,
					int index,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindClob(const char *variable,
					unsigned short variablesize,
					int index,
					short *isnull) {
	// by default, do nothing...
	return 1;
}

int	sqlrcursor::outputBindCursor(const char *variable,
					unsigned short variablesize,
					sqlrcursor *cursor) {
	// by default, do nothing...
	return 1;
}

void	sqlrcursor::returnOutputBindBlob(int index) {
	// by default, do nothing...
	return;
}

void	sqlrcursor::returnOutputBindClob(int index) {
	// by default, do nothing...
	return;
}

void	sqlrcursor::returnOutputBindCursor(int index) {
	// by default, do nothing...
	return;
}
