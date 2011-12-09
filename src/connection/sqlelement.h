// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLELEMENT
#define SQLELEMENT

class sqlelement {
	public:
		// generic...
		static const char *name;
		static const char *type;
		static const char *size;
		static const char *value;
		static const char *options;
		static const char *verbatim;

		// create query...
		static const char *create_query;

		// table...
		static const char *table;
		static const char *temporary;
		static const char *if_not_exists;

		// column definitions...
		static const char *columns;
		static const char *column;
		static const char *values;
		static const char *length;
		static const char *scale;

		// constraints...
		static const char *constraints;
		static const char *unsigned_constraint;
		static const char *zerofill;
		static const char *binary;
		static const char *character_set;
		static const char *collate;
		static const char *nullable;
		static const char *not_nullable;
		static const char *default_value;
		static const char *auto_increment;
		static const char *unique_key;
		static const char *primary_key;
		static const char *key;
		static const char *comment;
		static const char *column_format;
		static const char *references;
		static const char *match;
		static const char *on_delete;
		static const char *on_update;

		// on commit clause...
		static const char *on_commit;

		// table options...
		static const char *table_options;

		// partition options...
		static const char *partition_options;


		// drop...
		static const char *drop_query;


		// insert...
		static const char *insert_query;
		static const char *into;


		// update...
		static const char *update_query;


		// delete...
		static const char *delete_query;


		// select...
		static const char *select_query;
		static const char *unique;
		static const char *distinct;
		static const char *from;
		static const char *where;
		static const char *order_by;
		static const char *group_by;
};

#endif
