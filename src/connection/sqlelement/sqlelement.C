// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlelement.h>

// generic...
const char *sqlelement::_name="name";
const char *sqlelement::_type="type";
const char *sqlelement::_size="size";
const char *sqlelement::_value="value";
const char *sqlelement::_options="options";
const char *sqlelement::_verbatim="verbatim";

// create query...
const char *sqlelement::_create="create";
const char *sqlelement::_create_temporary="create_temporary";

// table...
const char *sqlelement::_table="table";
const char *sqlelement::_if_not_exists="if_not_exists";

// column definitions...
const char *sqlelement::_columns="columns";
const char *sqlelement::_column="column";
const char *sqlelement::_values="values";
const char *sqlelement::_length="length";
const char *sqlelement::_scale="scale";

// constraints...
const char *sqlelement::_constraints="constraints";
const char *sqlelement::_unsigned="unsigned";
const char *sqlelement::_zerofill="zerofill";
const char *sqlelement::_binary="binary";
const char *sqlelement::_character_set="character_set";
const char *sqlelement::_collate="collate";
const char *sqlelement::_null="null";
const char *sqlelement::_not_null="not_null";
const char *sqlelement::_default="default";
const char *sqlelement::_auto_increment="auto_increment";
const char *sqlelement::_unique_key="unique_key";
const char *sqlelement::_primary_key="primary_key";
const char *sqlelement::_key="key";
const char *sqlelement::_comment="comment";
const char *sqlelement::_column_format="column_format";
const char *sqlelement::_references="references";
const char *sqlelement::_match="match";
const char *sqlelement::_on_delete="on_delete";
const char *sqlelement::_on_update="on_update";
const char *sqlelement::_on_commit="on_commit";
const char *sqlelement::_as="as";


// drop...
const char *sqlelement::_drop="drop";
const char *sqlelement::_drop_temporary="drop_temporary";
const char *sqlelement::_if_exists="if exists";
const char *sqlelement::_table_name_list="table_name_list";
const char *sqlelement::_table_name_list_item="table_name_list_item";
const char *sqlelement::_restrict="restrict";
const char *sqlelement::_cascade="cascade";


// insert...
const char *sqlelement::_insert="insert";
const char *sqlelement::_into="into";


// update...
const char *sqlelement::_update="update";


// delete...
const char *sqlelement::_delete="delete";


// select...
const char *sqlelement::_select="select";
const char *sqlelement::_unique="unique";
const char *sqlelement::_distinct="distinct";
const char *sqlelement::_from="from";
const char *sqlelement::_where="where";
const char *sqlelement::_order_by="order by";
const char *sqlelement::_group_by="group by";
