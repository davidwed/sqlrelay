// Copyright (c) 1999-2001  David Muse
// See the COPYING file for more information.

#define MAXVAR 256

#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

// we're optimistic that the average query will contain 15 columns whose names
// average 10 characters in length
#define OPTIMISTIC_COLUMN_COUNT 15
#define OPTIMISTIC_AVERAGE_COLUMN_NAME_LENGTH 10
#define OPTIMISTIC_COLUMN_DATA_SIZE OPTIMISTIC_COLUMN_COUNT*\
					OPTIMISTIC_AVERAGE_COLUMN_NAME_LENGTH

// we're optimistic that the average query will contain 15 rows whose fields
// average 15 characters in length
#define OPTIMISTIC_ROW_COUNT 15
#define OPTIMISTIC_AVERAGE_FIELD_LENGTH 15
#define OPTIMISTIC_RESULT_SET_SIZE OPTIMISTIC_COLUMN_COUNT*\
					OPTIMISTIC_ROW_COUNT*\
					OPTIMISTIC_AVERAGE_FIELD_LENGTH

enum columncase {
	MIXED_CASE,
	UPPER_CASE,
	LOWER_CASE
};
