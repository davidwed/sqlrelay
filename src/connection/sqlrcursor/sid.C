// Copyright (c) 2005 David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <rudiments/charstring.h>

// This method gets the parameters SID needs to 
// determine what to do

void sqlrcursor_svr::sql_injection_detection_parameters() {

	sprintf(sid_query, "select UPPER(sparam), svalue from sidparameters");

	sid_sqlrcur->sendQuery(sid_query);

	ingress_mode = false;
	egress_mode = false;
	listen_mode = false;
	verification_mode = false;
	prevention_mode = false;

	for (uint64_t num_rows = 0;
		num_rows < sid_sqlrcur->rowCount();
		num_rows++)
	{
	
		const char * const *sid_row = sid_sqlrcur->getRow(num_rows);

		bool	param_value =
			( charstring::toInteger(sid_row[1]) == 1 );

		if ( !charstring::compare(sid_row[0], "INGRESS") )
			ingress_mode = param_value;

		if ( !charstring::compare(sid_row[0], "EGRESS") )
			egress_mode = param_value;

		if ( !charstring::compare(sid_row[0], "LISTEN") )
			listen_mode = param_value;

		if ( !charstring::compare(sid_row[0], "VERIFICATION") )
			verification_mode = param_value;

		if ( !charstring::compare(sid_row[0], "PREVENTION") )
			prevention_mode = param_value;
	}
}

// This method performs SQL Injection Detection
// for ingress filtering

bool sqlrcursor_svr::sql_injection_detection_ingress(const char *query) {

	if (!conn->cfgfl->getSidEnabled()) {
		return false;
	}
       
	bool ret_val = false;	// flag indicating a sql injection attack


	// Check for possible buffer overfow attack

	if ( charstring::length(query) > ( BUFSIZ - 1000 ) )
	{

	   sql_injection_detection_log(query,
                              "BO Attack", "Buffer Overflow Attack");
           return true; 

	}

        // parse the SQL statement
	// it will be needed for all modes of operation

        sql_injection_detection_parse_sql(query);

	// if in listening mode, record the sql statement passing thru

        if ( listen_mode ) {

		sql_injection_detection_log(query, sid_parsed_sql,
						"Ingress Listening");

	}

        // if perform ingress filtering,
        // check black list
	// check white list
	// check learning database

        if ( ingress_mode ) {

           // determine if SQL statement is in black list

           if ( sql_injection_detection_ingress_bl(query) == true )
           {
             ret_val = true;
           }

           // determine if SQL statement is in white list

           else if ( sql_injection_detection_ingress_wl(query) == true )
           {
              ret_val = false;
           }
           else  // determine if SQL statement is in learning database
           {
		if ( sql_injection_detection_ingress_ldb() == false )
                  ret_val = true;
		else
		  ret_val = false;
	   }

	   // verification mode set
           // do not send sql statements to the database for processing

	   if ( verification_mode )
	   {
              return true;
           }

           // prevention mode set

           if ( prevention_mode )
           {
              return ret_val;
           }
	
	} 

	return ret_val; 
}

// This method determines if the parsed sql statement
// is in the learning database

bool sqlrcursor_svr::sql_injection_detection_ingress_ldb() {

	bool ret_val = false;

	const char * sid_log_message =  "Ingress_Ldb_Not_Found";

	if ( sql_injection_detection_check_db("sidingressldb") )
	{

		sid_log_message = "Ingress_Ldb_Found";
		ret_val = true;
	}	

	sql_injection_detection_log(sid_parsed_sql,
                                    sid_parsed_sql, sid_log_message);

	return( ret_val );

}

// This method determines if the SQL statement contains
// anything in the white list

bool sqlrcursor_svr::sql_injection_detection_ingress_wl(const char *query) {

	bool ret_val = false;

	const char * sid_log_message = "Ingress_Wl_Not_Found";

	if ( sql_injection_detection_check_db("sidingresswlist") )
	{

		sid_log_message = "Ingress_Wl_Found";
		ret_val = true;

	}	

	sql_injection_detection_log(query,
                                    sid_parsed_sql, sid_log_message);


	return( ret_val );

}

// This method determines if the SQL statement contains
// anything in the black list

bool sqlrcursor_svr::sql_injection_detection_ingress_bl(const char *query) {

	bool ret_val = false;

	const char * sid_log_message = "Ingress_Bl_Not_Found";

	if ( sql_injection_detection_check_db("sidingressblist") )
	{

		sid_log_message = "Ingress_Bl_Found";
		ret_val = true;

	}	
	
	sql_injection_detection_log(query,
                                    sid_parsed_sql, sid_log_message);

	return( ret_val );

}

// This method determines if a value is in a database
// It checks both ingress/egress databases
// True - sql statement found in the sid db
// False - sql statement not found in the sid db

bool sqlrcursor_svr::sql_injection_detection_check_db(const char *sid_db) {

	bool ret_val = false;
	bool valid_list = false;
	int32_t i, target_size;
	const char *target_sql = NULL;

	// determine what list to search for

	if ( !charstring::compare(sid_db, "sidingressblist", 15) )
	{

		sprintf(sid_query, "select sql_state from sidingressblist where sql_state =  \'");

		target_sql = sid_parsed_sql;

		valid_list = true;

	}

	if ( !charstring::compare(sid_db, "sidingresswlist", 15) )
	{

		sprintf(sid_query, "select sql_state from sidingresswlist where sql_state =  \'");

		target_sql = sid_parsed_sql;

		valid_list = true;

	}

	if ( !charstring::compare(sid_db, "sidegressblist", 14) )
	{

		sprintf(sid_query, "select sql_state from sidegressblist where sql_state =  \'");

		target_sql = sid_parsed_results;

		valid_list = true;

	}

	if ( !charstring::compare(sid_db, "sidegresswlist", 14) )
	{

		sprintf(sid_query, "select sql_state from sidegresswlist where sql_state =  \'");

		target_sql = sid_parsed_results;

		valid_list = true;

	}

	if ( !charstring::compare(sid_db, "sidingressldb", 13) )
	{

		sprintf(sid_query, "select parsed_sql from sidingressldb where parsed_sql =  \'");


		target_sql = sid_parsed_sql;

		valid_list = true;

	}

	if ( !charstring::compare(sid_db, "sidegressldb", 12) )
	{

		sprintf(sid_query, "select parsed_sql from sidegressldb where parsed_sql =  \'");

		target_sql = sid_parsed_results;

		valid_list = true;

	}

	// determine if a valid list was provided

	if ( valid_list ) 
	{

		// Assign the sql_state target value in the
		// sid query by assigning each and every
		// character except the null character

		for ( i = 0, 
	      		target_size = charstring::length(target_sql);
              		i < target_size ; i++ )
		{
			sprintf(sid_query, "%s%c", sid_query, target_sql[i]);
		}

		sprintf(sid_query, "%s\'", sid_query);

		sid_sqlrcur->sendQuery(sid_query);


		// if rows returned for sql statement
		// then target sql found

		if ( sid_sqlrcur->rowCount() )
		{
			ret_val = true;
		} 

	}

	return ret_val;

}

// This method maintains a log for SID
// The SQL statement, parsed sql and message will be logged

void sqlrcursor_svr::sql_injection_detection_log(const char *query, 
					      const char *parsed_sql,
					      const char *log_message) {
	int32_t i, buf_len;

	char param1[BUFSIZ], param2[BUFSIZ], param3[BUFSIZ];

	sprintf(param1, "%s", query);
	sprintf(param2, "%s", parsed_sql);
	sprintf(param3, "%s", log_message);

	sprintf(sid_query, "insert into sidlog (sql_state, parsed_sql, sid_flag) values (\"");

	for ( i = 0, buf_len = charstring::length(param1); i < buf_len; i++ )
	{
	   if ( param1[i] != '\0' )
	      sprintf(sid_query, "%s%c", sid_query, param1[i]);
	} 

	sprintf(sid_query, "%s\", \"", sid_query);

	for ( i = 0, buf_len = charstring::length(param2); i < buf_len; i++ )
	{
	   if ( param2[i] != '\0' )
	      sprintf(sid_query, "%s%c", sid_query, param2[i]);
	} 

	sprintf(sid_query, "%s\", \"", sid_query);

	for ( i = 0, buf_len = charstring::length(param3); i < buf_len; i++ )
	{
	   if ( param3[i] != '\0' )
	      sprintf(sid_query, "%s%c", sid_query, param3[i]);
	} 

	sprintf(sid_query, "%s\")", sid_query);

	sid_sqlrcur->sendQuery(sid_query);

}

// This method parses the SQL statement and represents
// it in a format for the learning database

void sqlrcursor_svr::sql_injection_detection_parse_sql(const char *query) {

	uint32_t  temp_querylength;
	int32_t quote_count = 0;
	uint32_t i;
	uint32_t j;
	bool equal_sign;

	equal_sign = false;

	i = 0;
	j = 0;

	temp_querylength = charstring::length(query);

        for ( i = 0; i < BUFSIZ; i++ )
	{
		sid_parsed_sql[i] = '\0';
	}


        for ( i = 0; i < temp_querylength; i++ )
	{
           if ( query[i] == '\'' )
	   {
	      quote_count = quote_count + 1; 

	      if ( quote_count == 2 )
                 quote_count = 0;
	   } 
	   else if ( ( query[i] != ' ' ) && ( query[i] != ',' ) )
           {
              if (  quote_count == 0 )
	      {
		 if ( j < BUFSIZ )
		 {
                    sid_parsed_sql[j] = query[i];
		 }
                 j = j + 1;

		 if ( query[i] == '=' )
		 {

			while ( (query[i + 1] == ' ') && ((i + 1) < BUFSIZ) ) 
			   i++;	

			while ( (isdigit(query[i + 1])) && ((i + 1) < BUFSIZ))
			  i++;

		 }
              } 
           }
	}

	sid_parsed_sql[j] = '\0';

}

// This method performs egress filtering for SID

bool sqlrcursor_svr::sql_injection_detection_egress() {

	if (!conn->cfgfl->getSidEnabled()) {
		return false;
	}
       
	bool ret_val = false;	// flag indicating a sql injection attack

        // parse the SQL results 
	// it will be needed for all modes of operation

        sql_injection_detection_parse_results(colCount(), columnNames());

	// Detect if a buffer overflow attack

	if ( charstring::length(sid_parsed_results) > ( BUFSIZ - 1000 ) )
	{

		sql_injection_detection_log("BO Attack", sid_parsed_results,
                                            "Buffer Overflow Attack");
		
		return false;
	}

	// if in listening mode, record the sql resuts passing thru

        if ( listen_mode ) {

		sql_injection_detection_log("Egress Listening",
						sid_parsed_results,
						"Egress Listening");


	}

        // if perform egress filtering,
        // check black list
	// check white list
	// check learning database

        if ( egress_mode ) {

           // determine if SQL statement is in black list

           if ( sql_injection_detection_egress_bl() == true )
           {
             ret_val = true;
           }

           // determine if SQL statement is in white list

           else if ( sql_injection_detection_egress_wl() == true )
           {
              ret_val = false;
           }
           else  // determine if SQL statement is in learning database
           {
		if ( sql_injection_detection_egress_ldb() == false )
                  ret_val = true;
	   }

	   // Verification mode is not an option when
	   // analyzing the result set due to the fact
           // that the SQL statement has already processed

           // prevention mode set

           if ( prevention_mode )
           {
              return ret_val;
           }
	
	} 

	return false; 
}

// This method parses the SQL results

void sqlrcursor_svr::sql_injection_detection_parse_results(int32_t num_fields,
					const char * const *field_names) {

	int32_t i;

	for ( i = 0; i < BUFSIZ; i++ )
	{
		sid_parsed_results[i] = '\0';
	}

	for (i=0; i < num_fields; i++)
	{

		if ( ( charstring::length(sid_parsed_results) +
		     charstring::length(field_names[i]) ) < BUFSIZ )

			sprintf(sid_parsed_results, "%s%s",
				sid_parsed_results, field_names[i]);

	}
	
}

// This method performs egress black list processing

bool sqlrcursor_svr::sql_injection_detection_egress_bl() {

	bool ret_val = false;

	const char * sid_log_message = "Egress_Bl_Not_Found";

	if ( sql_injection_detection_check_db("sidegressblist") )
	{

		sid_log_message = "Egress_Bl_Found";
		ret_val = true;

	}	
	
	sql_injection_detection_log(sid_log_message,
				    sid_parsed_results, sid_log_message);


	return( ret_val );

}

// This method performs egress white list processing

bool sqlrcursor_svr::sql_injection_detection_egress_wl() {

	bool ret_val = false;

	const char * sid_log_message = "Egress_Wl_Not_Found";

	if ( sql_injection_detection_check_db("sidegresswlist") )
	{

		sid_log_message = "Egress_Wl_Found";
		ret_val = true;

	}	
	
	sql_injection_detection_log(sid_log_message, 
                                    sid_parsed_results, sid_log_message);

	return( ret_val );

}

// This method performs egress filtering for learning db processing

bool sqlrcursor_svr::sql_injection_detection_egress_ldb() {

	bool ret_val = false;

	const char * sid_log_message = "Egress_Ldb_Not_Found";

	if ( sql_injection_detection_check_db("sidegressldb") )
	{

		sid_log_message = "Egress_Ldb_Found";
		ret_val = true;

	}	

	sql_injection_detection_log(sid_log_message, 
                                    sid_parsed_results, sid_log_message);

	return( ret_val );

}
