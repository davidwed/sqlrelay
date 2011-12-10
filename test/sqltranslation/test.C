// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <stdio.h>
#include <sqlparser.h>
#include <oracle8sqltranslator.h>
#include <oracle8sqlwriter.h>

void printQueryTree(xmldom *tree) {
	const char	*xml=tree->getRootNode()->xml()->getString();
	int16_t		indent=0;
	bool		endtag;
	for (const char *ptr=xml; *ptr; ptr++) {
		if (*ptr=='<') {
			if (*(ptr+1)=='/') {
				indent=indent-2;
				endtag=true;
			}
			for (uint16_t i=0; i<indent; i++) {
				printf(" ");
			}
		}
		printf("%c",*ptr);
		if (*ptr=='>') {
			printf("\n");
			if (*(ptr-1)!='/' && !endtag) {
				indent=indent+2;
			}
			endtag=false;
		}
	}
}

main() {

	const char *translationrules=
			"<sqltranslationrules>"
			"	<temp_tables_preserve_rows_by_default/>"
			"	<nativize_datatypes/>"
			"	<convert_datatypes>"
			"		<convert from=\"varchar\" to=\"varchar2\"/>"
			"	</convert_datatypes>"
			"	<trim_columns_compared_to_string_binds/>"
			"</sqltranslationrules>";

	oraclesqltranslator	sqlt;
	sqlt.loadRules(translationrules);
	

	const char * const	queries[]={
		"create temporary table if not exists test "
		"	(bitcol bit(8) not null default 50 auto_increment unique key primary key key comment 'this is a bitcol' column_format fixed, "
		"	tinyintcol tinyint(2) unsigned zerofill, "
		"	smallintcol smallint(4), "
		"	mediumintcol mediumint(8), "
		"	intcol int(16), "
		"	integercol integer(32), "
		"	bigintcol bigint(64), "
		"	realcol real(10,5), "
		"	doublecol double(10,5), "
		"	floatcol float(10,5), "
		"	decimalcol1 decimal(10), "
		"	decimalcol2 decimal(10,5), "
		"	numericcol1 numeric(10), "
		"	numericcol2 numeric(10,5), "
		"	datecol date, "
		"	timecol time, "
		"	timestampcol timestamp, "
		"	datetimecol datetime, "
		"	yearcol year, "
		"	charcol char(100) character set utf8 collate blah, "
		"	varcharcol varchar(100), "
		"	binarycol binary(100), "
		"	varbinarycol varbinary(100), "
		"	tinyblobcol tinyblob, "
		"	blobcol blob, "
		"	mediumblobcol mediumblob, "
		"	longblobcol longblob, "
		"	tinytextcol tinytext binary character set utf8 collate blah, "
		"	textcol text, "
		"	mediumtextcol mediumtext, "
		"	longtextcol longtext, "
		"	enumcol enum(hello, hi, goodbye, bye) character set utf8 collate blah, "
		"	setcol set(hello, hi, goodbye, bye), "
		"	blah blahtype(blah) blah not null blah unsigned blah(blah), "
		"	foreign integer references foreign_table (col1, col2, col3) match full on delete set null on update cascade "
		") "
		//"	on commit preserve rows "
		"	engine = innodb "
		"	auto_increment = 50 "
		"	avg_row_length = 100 "
		"	default character set = utf8 "
		"	checksum = 1 "
		"	default collate = blah "
		"	comment = 'oh yeah!' "
		"	... ",
		"drop table test",
		"insert into table test values (1)",
		"update table test set col1=2",
		"delete from table test",
		"select * from table test",
		NULL
	};
	
	for (uint16_t i=0; queries[i]; i++) {

		printf("parsing:\n%s\n",queries[i]);

		// parse the query
		sqlparser	sqlp;
		sqlp.parse(queries[i]);
		xmldom		*tree=sqlp.detachTree();

		// apply rules
		printf("\nbefore rules:\n");
		printQueryTree(tree);
		sqlt.applyRules(tree);
		printf("\nafter rules:\n");
		printQueryTree(tree);
		printf("\n");

		// write the query back out
		oraclesqlwriter	sqlw;
		stringbuffer	translatedquery;
		sqlw.write(tree,&translatedquery);
		printf("translated:\n%s\n\n\n",translatedquery.getString());

		delete tree;
	}
}
