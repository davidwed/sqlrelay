// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlelement.h>

// generic...
const char *sqlelement::name="name";
const char *sqlelement::type="type";
const char *sqlelement::size="size";
const char *sqlelement::value="value";
const char *sqlelement::options="options";
const char *sqlelement::verbatim="verbatim";

// create query...
const char *sqlelement::create_query="create";

// table...
const char *sqlelement::table="table";
const char *sqlelement::temporary="temporary";
const char *sqlelement::if_not_exists="if_not_exists";

// column definitions...
const char *sqlelement::columns="columns";
const char *sqlelement::column="column";
const char *sqlelement::values="values";
const char *sqlelement::length="length";
const char *sqlelement::scale="scale";

// constraints...
const char *sqlelement::constraints="constraints";
const char *sqlelement::unsigned_constraint="unsigned";
const char *sqlelement::zerofill="zerofill";
const char *sqlelement::binary="binary";
const char *sqlelement::character_set="character_set";
const char *sqlelement::collate="collate";
const char *sqlelement::nullable="nullable";
const char *sqlelement::not_nullable="not_nullable";
const char *sqlelement::default_value="default_value";
const char *sqlelement::auto_increment="auto_increment";
const char *sqlelement::unique_key="unique_key";
const char *sqlelement::primary_key="primary_key";
const char *sqlelement::key="key";
const char *sqlelement::comment="comment";
const char *sqlelement::column_format="column_format";
const char *sqlelement::references="references";
const char *sqlelement::match="match";
const char *sqlelement::on_delete="on_delete";
const char *sqlelement::on_update="on_update";

// on commit clause...
const char *sqlelement::on_commit="on_commit";

// table options...
const char *sqlelement::table_options="table_options";

// partition options...
const char *sqlelement::partition_options="partition_options";


// drop...
const char *sqlelement::drop_query="drop";


// insert...
const char *sqlelement::insert_query="insert";
const char *sqlelement::into="into";


// update...
const char *sqlelement::update_query="update";


// delete...
const char *sqlelement::delete_query="delete";


// select...
const char *sqlelement::select_query="select";
const char *sqlelement::unique="unique";
const char *sqlelement::distinct="distinct";
const char *sqlelement::from="from";
const char *sqlelement::where="where";
const char *sqlelement::order_by="order by";
const char *sqlelement::group_by="group by";
