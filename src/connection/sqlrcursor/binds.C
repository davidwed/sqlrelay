// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrcursor_svr::supportsNativeBinds() {
	return true;
}

bool sqlrcursor_svr::handleBinds() {
	
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
		} else if (inbindvars[i].type==INTEGER_BIND) {
			if (!inputBindInteger(inbindvars[i].variable,
				inbindvars[i].variablesize,
				&inbindvars[i].value.integerval)) {
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
		} else if (outbindvars[i].type==INTEGER_BIND) {
			if (!outputBindInteger(outbindvars[i].variable,
					outbindvars[i].variablesize,
					&outbindvars[i].value.integerval,
					&outbindvars[i].isnull)) {
				return false;
			}
		} else if (outbindvars[i].type==DOUBLE_BIND) {
			if (!outputBindDouble(outbindvars[i].variable,
				outbindvars[i].variablesize,
				&outbindvars[i].value.doubleval.value,
				&outbindvars[i].value.doubleval.precision,
				&outbindvars[i].value.doubleval.scale,
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

bool sqlrcursor_svr::inputBindString(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint16_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::inputBindInteger(const char *variable,
					uint16_t variablesize,
					int64_t *value) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::inputBindDouble(const char *variable,
					uint16_t variablesize,
					double *value, 
					uint32_t precision,
					uint32_t scale) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::outputBindString(const char *variable,
					uint16_t variablesize,
					char *value, 
					uint16_t valuesize, 
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::outputBindInteger(const char *variable,
					uint16_t variablesize,
					int64_t *value,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::outputBindDouble(const char *variable,
					uint16_t variablesize,
					double *value,
					uint32_t *precision,
					uint32_t *scale,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::outputBindBlob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::outputBindClob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrcursor_svr::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrcursor_svr *cursor) {
	// by default, do nothing...
	return true;
}

void sqlrcursor_svr::returnOutputBindBlob(uint16_t index) {
	// by default, do nothing...
	return;
}

void sqlrcursor_svr::returnOutputBindClob(uint16_t index) {
	// by default, do nothing...
	return;
}

void sqlrcursor_svr::returnOutputBindCursor(uint16_t index) {
	// by default, do nothing...
	return;
}
