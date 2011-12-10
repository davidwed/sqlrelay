// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::space(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr," ");
}

bool sqlparser::comma(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,",");
}

bool sqlparser::leftParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"(");
}

bool sqlparser::rightParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,")");
}

bool sqlparser::createClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"create ");
}

bool sqlparser::temporaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"temporary ",
		"temp ",
		"global temp ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::tableClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"table ");
}

bool sqlparser::ifNotExistsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"if not exists ");
}

bool sqlparser::unsignedClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"unsigned");
}

bool sqlparser::zeroFillClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"zeroFill");
}

bool sqlparser::binaryClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"binary");
}

bool sqlparser::characterSetClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"character set ",
		"char set ",
		"charset ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::collateClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"collate ",
		"collation ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::nullClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"nullable",
		"null",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::notNullClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"not nullable",
		"not null",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::defaultClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"default value ",
		"default ",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::autoIncrementClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"auto_increment");
}

bool sqlparser::uniqueKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"unique key",
		"unique",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::primaryKeyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"primary key");
}

bool sqlparser::keyClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"key");
}

bool sqlparser::commentClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"comment ");
}

bool sqlparser::columnFormatClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"column_format ");
}

bool sqlparser::referencesClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"references ");
}

bool sqlparser::matchClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"match ");
}

bool sqlparser::onDeleteClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on delete ");
}

bool sqlparser::onUpdateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on update ");
}

bool sqlparser::referenceOptionClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"restrict",
		"cascade",
		"set null",
		"no action",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::onCommitClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"on commit ");
}

bool sqlparser::onCommitOptionClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"delete rows",
		"preserve rows",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::asClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"as ");
}



bool sqlparser::dropClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"drop ");
}

bool sqlparser::ifExistsClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"if exists ");
}

bool sqlparser::restrictClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"restrict");
}

bool sqlparser::cascadeClause(const char *ptr, const char **newptr) {
	debugFunction();
	const char *parts[]={
		"cascade",
		"cascade constraints",
		NULL
	};
	return comparePart(ptr,newptr,parts);
}

bool sqlparser::insertClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"insert ");
}

bool sqlparser::updateClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"update ");
}

bool sqlparser::deleteClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"delete ");
}

bool sqlparser::selectClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"select ");
}

bool sqlparser::uniqueClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"unique ");
}

bool sqlparser::distinctClause(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"distinct ");
}
