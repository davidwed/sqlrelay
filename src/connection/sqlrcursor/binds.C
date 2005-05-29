// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrcursor::handleBinds() {
	
	// iterate through the arrays, binding values to variables
	for (int16_t i=0; i<inbindcount; i++) {

		// bind the value to the variable
		if (inbindvars[i].type==STRING_BIND ||
				inbindvars[i].type==NULL_BIND) {
			if (!inputBindString(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return false;
			}
		} else if (inbindvars[i].type==LONG_BIND) {
			if (!inputBindLong(inbindvars[i].variable,
				inbindvars[i].variablesize,
				(uint32_t *)&inbindvars[i].value.longval)) {
				return false;
			}
		} else if (inbindvars[i].type==DOUBLE_BIND) {
			if (!inputBindDouble(inbindvars[i].variable,
				inbindvars[i].variablesize,
				&inbindvars[i].value.doubleval.value,
				inbindvars[i].value.doubleval.precision,
				inbindvars[i].value.doubleval.scale)) {
				return false;
			}
		} else if (inbindvars[i].type==BLOB_BIND) {
			if (!inputBindBlob(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return false;
			}
		} else if (inbindvars[i].type==CLOB_BIND) {
			if (!inputBindClob(inbindvars[i].variable,
				inbindvars[i].variablesize,
				inbindvars[i].value.stringval,
				inbindvars[i].valuesize,
				&inbindvars[i].isnull)) {
				return false;
			}
		}
	}
	for (int16_t i=0; i<outbindcount; i++) {

		// bind the value to the variable
		if (outbindvars[i].type==STRING_BIND) {
			if (!outputBindString(outbindvars[i].variable,
					outbindvars[i].variablesize,
					outbindvars[i].value.stringval,
					outbindvars[i].valuesize+1,
					&outbindvars[i].isnull)) {
				return false;
			}
		} else if (outbindvars[i].type==BLOB_BIND) {
			if (!outputBindBlob(outbindvars[i].variable,
					outbindvars[i].variablesize,i,
					&outbindvars[i].isnull)) {
				return false;
			}
		} else if (outbindvars[i].type==CLOB_BIND) {
			if (!outputBindClob(outbindvars[i].variable,
					outbindvars[i].variablesize,i,
					&outbindvars[i].isnull)) {
				return false;
			}
		} else if (outbindvars[i].type==CURSOR_BIND) {
			if (!outputBindCursor(outbindvars[i].variable,
						outbindvars[i].variablesize,
						conn->cur[outbindvars[i].value.
								cursorid])) {
				return false;
			}
		}
	}
	return true;
}

bool sqlrcursor::inputBindString(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint16_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::inputBindLong(const char *variable,
					uint16_t variablesize,
					uint32_t *value) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::inputBindDouble(const char *variable,
					uint16_t variablesize,
					double *value, 
					uint32_t precision,
					uint32_t scale) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::outputBindString(const char *variable,
					uint16_t variablesize,
					char *value, 
					uint16_t valuesize, 
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::outputBindBlob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::outputBindClob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrcursor *cursor) {
	// by default, do nothing...
	return true;
}

void sqlrcursor::returnOutputBindBlob(uint16_t index) {
	// by default, do nothing...
	return;
}

void sqlrcursor::returnOutputBindClob(uint16_t index) {
	// by default, do nothing...
	return;
}

void sqlrcursor::returnOutputBindCursor(uint16_t index) {
	// by default, do nothing...
	return;
}
