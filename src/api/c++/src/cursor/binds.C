// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/rawbuffer.h>
#include <defines.h>

uint16_t sqlrcursor::countBindVariables() const {

	if (!queryptr) {
		return 0;
	}

	char	lastchar='\0';
	bool	inquotes=false;

	uint16_t	paramcount=0;

	for (const char *ptr=queryptr; *ptr; ptr++) {

		if (*ptr=='\'' && lastchar!='\\') {
			if (inquotes) {
				inquotes=false;
			} else {
				inquotes=true;
			}
		}

		// If we're not inside of a quoted string and we run into
		// a ?, : (for oracle-style bind's), @ (for sybase-style
		// binds) or $ (for postgresql-style binds) and the previous
		// character was whitespace, or a comma, left parenthesis or
		// equal sign then we must have found a bind variable.
		if (!inquotes &&
			(*ptr=='?' || *ptr==':' || *ptr=='@' || *ptr=='$') &&
			(lastchar==' ' || lastchar=='	' ||
			lastchar=='\n' || lastchar=='\r' ||
			lastchar=='=' || lastchar==',' || lastchar=='(')) {
			paramcount++;
		}

		lastchar=*ptr;
	}

	return paramcount;
}

void sqlrcursor::clearVariables() {

	// setting the bind/substitution variable 
	// counts to 0 effectively clears them
	subcount=0;
	clearBinds();
}

void sqlrcursor::initVariables() {

	// initialize the bind and substitution variables
	for (int16_t i=0; i<MAXVAR; i++) {
		subvars[i].variable=NULL;
		subvars[i].value.stringval=NULL;
		subvars[i].type=STRING_BIND;
		inbindvars[i].variable=NULL;
		inbindvars[i].value.stringval=NULL;
		inbindvars[i].type=STRING_BIND;
		outbindvars[i].variable=NULL;
		outbindvars[i].value.stringval=NULL;
		outbindvars[i].type=STRING_BIND;
	}
}

void sqlrcursor::deleteVariables() {

	// if we were copying values, delete them
	if (copyrefs) {
		for (int16_t i=0; i<MAXVAR; i++) {
			delete[] inbindvars[i].variable;
			if (inbindvars[i].type==STRING_BIND) {
				delete[] inbindvars[i].value.stringval;
			}
			if (inbindvars[i].type==BLOB_BIND ||
				inbindvars[i].type==CLOB_BIND) {
				delete[] inbindvars[i].value.lobval;
			}
			delete[] outbindvars[i].variable;
			delete[] subvars[i].variable;
			if (subvars[i].type==STRING_BIND) {
				delete[] subvars[i].value.stringval;
			}
		}
	}

	// output binds are deleted no matter what
	for (int16_t i=0; i<MAXVAR; i++) {
		if (outbindvars[i].type==STRING_BIND) {
			delete[] outbindvars[i].value.stringval;
		}
		if (outbindvars[i].type==BLOB_BIND ||
			outbindvars[i].type==CLOB_BIND) {
			delete[] outbindvars[i].value.lobval;
		}
	}
}

void sqlrcursor::substitution(const char *variable, const char *value) {
	if (subcount<MAXVAR && variable && variable[0]) {
		stringVar(&subvars[subcount],variable,value);
		subcount++;
	}
}

void sqlrcursor::substitution(const char *variable, int64_t value) {
	if (subcount<MAXVAR && variable && variable[0]) {
		integerVar(&subvars[subcount],variable,value);
		subcount++;
	}
}

void sqlrcursor::substitution(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (subcount<MAXVAR && variable && variable[0]) {
		doubleVar(&subvars[subcount],variable,value,precision,scale);
		subcount++;
	}
}

void sqlrcursor::clearBinds() {
	inbindcount=0;
	outbindcount=0;
}

void sqlrcursor::inputBindBlob(const char *variable, const char *value,
							uint32_t size) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		lobVar(&inbindvars[inbindcount],variable,value,size,BLOB_BIND);
		inbindvars[inbindcount].send=true;
		inbindcount++;
	}
}

void sqlrcursor::inputBindClob(const char *variable, const char *value,
							uint32_t size) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		lobVar(&inbindvars[inbindcount],variable,value,size,CLOB_BIND);
		inbindvars[inbindcount].send=true;
		inbindcount++;
	}
}

void sqlrcursor::inputBind(const char *variable, const char *value) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		stringVar(&inbindvars[inbindcount],variable,value);
		inbindvars[inbindcount].send=true;
		inbindcount++;
	}
}

void sqlrcursor::inputBind(const char *variable, int64_t value) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		integerVar(&inbindvars[inbindcount],variable,value);
		inbindvars[inbindcount].send=true;
		inbindcount++;
	}
}

void sqlrcursor::inputBind(const char *variable, double value, 
				uint32_t precision, uint32_t scale) {
	if (inbindcount<MAXVAR && variable && variable[0]) {
		doubleVar(&inbindvars[inbindcount],variable,value,
						precision, scale);
		inbindvars[inbindcount].send=true;
		inbindcount++;
	}
}

void sqlrcursor::substitutions(const char **variables, const char **values) {
	uint16_t	index=0;
	while (variables[index] && subcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			stringVar(&subvars[subcount],
					variables[index],values[index]);
			subcount++;
		}
		index++;
	}
}

void sqlrcursor::substitutions(const char **variables, const int64_t *values) {
	uint16_t	index=0;
	while (variables[index] && subcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			integerVar(&subvars[subcount],
					variables[index],values[index]);
			subcount++;
		}
		index++;
	}
}

void sqlrcursor::substitutions(const char **variables, const double *values, 
					const uint32_t *precisions,
					const uint32_t *scales) {
	uint16_t	index=0;
	while (variables[index] && subcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			doubleVar(&subvars[subcount],
					variables[index],
					values[index],
					precisions[index],
					scales[index]);
			subcount++;
		}
		index++;
	}
}

void sqlrcursor::inputBinds(const char **variables, const char **values) {
	uint16_t	index=0;
	while (variables[index] && inbindcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			stringVar(&inbindvars[inbindcount],
					variables[index],values[index]);
			inbindvars[inbindcount].send=true;
			inbindcount++;
		}
		index++;
	}
}

void sqlrcursor::inputBinds(const char **variables, const int64_t *values) {
	uint16_t	index=0;
	while (variables[index] && inbindcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			integerVar(&inbindvars[inbindcount],
					variables[index],values[index]);
			inbindvars[inbindcount].send=true;
			inbindcount++;
		}
		index++;
	}
}

void sqlrcursor::inputBinds(const char **variables, const double *values, 
					const uint32_t *precisions,
					const uint32_t *scales) {
	uint16_t	index=0;
	while (variables[index] && inbindcount<MAXVAR) {
		if (variables[index] && variables[index][0]) {
			doubleVar(&inbindvars[inbindcount],
					variables[index],
					values[index],
					precisions[index],
					scales[index]);
			inbindvars[inbindcount].send=true;
			inbindcount++;
		}
		index++;
	}
}

void sqlrcursor::stringVar(bindvar *var, const char *variable,
						const char *value) {

	initVar(var,variable);

	// store the value, handle NULL values too
	if (value) {
		if (copyrefs) {
			var->value.stringval=charstring::duplicate(value);
		} else {
			var->value.stringval=(char *)value;
		}
		var->valuesize=charstring::length(value);
		var->type=STRING_BIND;
	} else {
		var->type=NULL_BIND;
	}
}

void sqlrcursor::integerVar(bindvar *var, const char *variable, int64_t value) {
	initVar(var,variable);
	var->type=INTEGER_BIND;
	var->value.integerval=value;
}

void sqlrcursor::doubleVar(bindvar *var, const char *variable, double value,
					uint32_t precision, uint32_t scale) {
	initVar(var,variable);
	var->type=DOUBLE_BIND;
	var->value.doubleval.value=value;
	var->value.doubleval.precision=precision;
	var->value.doubleval.scale=scale;
}

void sqlrcursor::lobVar(bindvar *var, const char *variable,
			const char *value, uint32_t size, bindtype type) {

	initVar(var,variable);

	// Store the value, handle NULL values too.
	// For LOB's empty strings are handled as NULL's as well, this is
	// probably not right, but I can't get empty string lob binds to work.
	if (value && size>0) {
		if (copyrefs) {
			var->value.lobval=new char[size];
			rawbuffer::copy(var->value.lobval,value,size);
		} else {
			var->value.lobval=(char *)value;
		}
		var->valuesize=size;
		var->type=type;
	} else {
		var->type=NULL_BIND;
	}
}

void sqlrcursor::initVar(bindvar *var, const char *variable) {

	// clear any old variable name that was stored and assign the new 
	// variable name also clear any old value that was stored in this 
	// variable
	if (copyrefs) {
		delete[] var->variable;
		var->variable=charstring::duplicate(variable);

		if (var->type==STRING_BIND &&
				var->value.stringval) {
			delete[] var->value.stringval;
		} else if ((var->type==BLOB_BIND ||
				var->type==CLOB_BIND) &&
				var->value.lobval) {
			delete[] var->value.lobval;
		}
	} else {
		var->variable=(char *)variable;
	}
}

void sqlrcursor::defineOutputBindString(const char *variable,
						uint32_t length) {
	defineOutputBindGeneric(variable,STRING_BIND,length);
}

void sqlrcursor::defineOutputBindInteger(const char *variable) {
	defineOutputBindGeneric(variable,INTEGER_BIND,sizeof(int64_t));
}

void sqlrcursor::defineOutputBindDouble(const char *variable) {
	defineOutputBindGeneric(variable,DOUBLE_BIND,sizeof(double));
}

void sqlrcursor::defineOutputBindBlob(const char *variable) {
	defineOutputBindGeneric(variable,BLOB_BIND,0);
}

void sqlrcursor::defineOutputBindClob(const char *variable) {
	defineOutputBindGeneric(variable,CLOB_BIND,0);
}

void sqlrcursor::defineOutputBindCursor(const char *variable) {
	defineOutputBindGeneric(variable,CURSOR_BIND,0);
}

void sqlrcursor::defineOutputBindGeneric(const char *variable,
				bindtype type, uint32_t valuesize) {

	if (outbindcount<MAXVAR && variable && variable[0]) {

		// clean up old values
		if (outbindvars[outbindcount].type==STRING_BIND) {
			delete[] outbindvars[outbindcount].value.stringval;
		} else if (outbindvars[outbindcount].type==BLOB_BIND ||
			outbindvars[outbindcount].type==CLOB_BIND) {
			delete[] outbindvars[outbindcount].value.lobval;
		}
		if (copyrefs) {
			// clean up old variable and set new variable
			delete[] outbindvars[outbindcount].variable;
			outbindvars[outbindcount].variable=
					charstring::duplicate(variable);
		} else {
			outbindvars[outbindcount].variable=(char *)variable;
		}
		outbindvars[outbindcount].type=type;
		outbindvars[outbindcount].value.stringval=NULL;
		outbindvars[outbindcount].value.lobval=NULL;
		outbindvars[outbindcount].valuesize=valuesize;
		outbindvars[outbindcount].send=true;
		outbindcount++;
	}
}

const char *sqlrcursor::getOutputBindString(const char *variable) {

	if (variable) {
		for (int16_t i=0; i<outbindcount; i++) {
			if (!charstring::compare(outbindvars[i].variable,
								variable)) {
				if (outbindvars[i].type==STRING_BIND) {
					return outbindvars[i].value.stringval;
				} else {
					return outbindvars[i].value.lobval;
				}
			}
		}
	}
	return NULL;
}

uint32_t sqlrcursor::getOutputBindLength(const char *variable) {

	if (variable) {
		for (int16_t i=0; i<outbindcount; i++) {
			if (!charstring::compare(outbindvars[i].variable,
								variable)) {
				return outbindvars[i].valuesize;
			}
		}
	}
	return 0;
}

int64_t sqlrcursor::getOutputBindInteger(const char *variable) {

	if (variable) {
		for (int16_t i=0; i<outbindcount; i++) {
			if (!charstring::compare(
				outbindvars[i].variable,variable) &&
					outbindvars[i].type==INTEGER_BIND) {
					return outbindvars[i].value.integerval;
			}
		}
	}
	return -1;
}

double sqlrcursor::getOutputBindDouble(const char *variable) {

	if (variable) {
		for (int16_t i=0; i<outbindcount; i++) {
			if (!charstring::compare(
				outbindvars[i].variable,variable) &&
					outbindvars[i].type==DOUBLE_BIND) {
					return outbindvars[i].
						value.doubleval.value;
			}
		}
	}
	return -1.0;
}

sqlrcursor *sqlrcursor::getOutputBindCursor(const char *variable) {

	if (!outputBindCursorIdIsValid(variable)) {
		return NULL;
	}
	uint16_t	bindcursorid=getOutputBindCursorId(variable);
	sqlrcursor	*bindcursor=new sqlrcursor(sqlrc);
	bindcursor->attachToBindCursor(bindcursorid);
	return bindcursor;
}

bool sqlrcursor::outputBindCursorIdIsValid(const char *variable) {
	if (variable) {
		for (int16_t i=0; i<outbindcount; i++) {
			if (!charstring::compare(outbindvars[i].variable,
								variable)) {
				return true;
			}
		}
	}
	return false;
}

uint16_t sqlrcursor::getOutputBindCursorId(const char *variable) {

	if (variable) {
		for (int16_t i=0; i<outbindcount; i++) {
			if (!charstring::compare(outbindvars[i].variable,
								variable)) {
				return outbindvars[i].value.cursorid;
			}
		}
	}
	return 0;
}

void sqlrcursor::validateBinds() {
	validatebinds=true;
}
