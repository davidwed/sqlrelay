// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLELEMENT
#define SQLELEMENT

class sqlelement {
	public:
		// generic...
		static const char *_name;
		static const char *_type;
		static const char *_size;
		static const char *_value;
		static const char *_verbatim;

		// create query...
		static const char *_create;
		static const char *_create_temporary;

		// table...
		static const char *_table;
		static const char *_if_not_exists;

		// column definitions...
		static const char *_columns;
		static const char *_column;
		static const char *_values;
		static const char *_length;
		static const char *_scale;

		// constraints...
		static const char *_constraints;
		static const char *_unsigned;
		static const char *_zerofill;
		static const char *_binary;
		static const char *_character_set;
		static const char *_collate;
		static const char *_null;
		static const char *_not_null;
		static const char *_default;
		static const char *_auto_increment;
		static const char *_unique_key;
		static const char *_primary_key;
		static const char *_key;
		static const char *_comment;
		static const char *_column_format;
		static const char *_references;
		static const char *_match;
		static const char *_on_delete;
		static const char *_on_update;

		// on commit clause...
		static const char *_on_commit;
		static const char *_as;


		// drop...
		static const char *_drop;
		static const char *_drop_temporary;
		static const char *_if_exists;
		static const char *_table_name_list;
		static const char *_table_name_list_item;
		static const char *_restrict;
		static const char *_cascade;


		// insert...
		static const char *_insert;
		static const char *_into;


		// update...
		static const char *_update;


		// delete...
		static const char *_delete;


		// select...
		static const char *_select;
		static const char *_unique;
		static const char *_distinct;
		static const char *_from;
		static const char *_where;
		static const char *_order_by;
		static const char *_group_by;
};

#endif
