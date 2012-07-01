// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information
#ifndef PARSE_DATE_H
#define PARSE_DATE_H

#include <rudiments/charstring.h>
#include <stdio.h>

static const char *shortmonths[]={
	"JAN",
	"FEB",
	"MAR",
	"APR",
	"MAY",
	"JUN",
	"JUL",
	"AUG",
	"SEP",
	"OCT",
	"NOV",
	"DEC",
	NULL
};

static const char *longmonths[]={
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
	NULL
};

static bool parseDateTime(const char *datetime, bool ddmm,
			bool supportdotdelimiteddate,
			int16_t *year, int16_t *month, int16_t *day,
			int16_t *hour, int16_t *minute, int16_t *second) {

	// initialize date/time parts
	*year=-1;
	*month=-1;
	*day=-1;
	*hour=-1;
	*minute=-1;
	*second=-1;

	// different db's format dates very differently

	// split on a space
	char		**parts;
	uint64_t	partcount;
	charstring::split(datetime," ",1,true,&parts,&partcount);

	// there should only be one or two parts
	if (partcount>2) {
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
		delete[] parts;
		return false;
	}

	// parse the parts
	for (uint64_t i=0; i<partcount; i++) {

		if (charstring::contains(parts[i],':')) {

			// the section with :'s is probably the time...

			// split on :
			char		**timeparts;
			uint64_t	timepartcount;
			charstring::split(parts[i],":",1,true,
						&timeparts,&timepartcount);
	
			// there must be three parts, all numbers
			if (timepartcount==3 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				charstring::isNumber(timeparts[2])) {

				// well, if the first or last part is 4 digit
				// then it's a date (firebird uses
				// colon-delimited dates) otherwise it's a time
				if (charstring::length(timeparts[0])==4) {
					*year=charstring::toInteger(
								timeparts[0]);
					if (ddmm) {
						*day=charstring::toInteger(
								timeparts[1]);
						*month=charstring::toInteger(
								timeparts[2]);
					} else {
						*month=charstring::toInteger(
								timeparts[1]);
						*day=charstring::toInteger(
								timeparts[2]);
					}
				} else if (charstring::length(
							timeparts[2])==4) {
					if (ddmm) {
						*day=charstring::toInteger(
								timeparts[0]);
						*month=charstring::toInteger(
								timeparts[1]);
					} else {
						*day=charstring::toInteger(
								timeparts[0]);
						*month=charstring::toInteger(
								timeparts[1]);
					}
					*year=charstring::toInteger(
								timeparts[2]);
				} else {
					*hour=charstring::toInteger(
								timeparts[0]);
					*minute=charstring::toInteger(
								timeparts[1]);
					*second=charstring::toInteger(
								timeparts[2]);
				}
			}

			// clean up
			for (uint64_t i=0; i<timepartcount; i++) {
				delete[] timeparts[i];
			}
			delete[] timeparts;

		} else if (charstring::contains(parts[i],'/')) {

			// the section with /'s is the date...

			// split on /
			char		**dateparts;
			uint64_t	datepartcount;
			charstring::split(parts[i],"/",1,true,
						&dateparts,&datepartcount);

			// assume month/day, but in some countries
			// they do it the other way around
			// I'm not sure how to decide...

			// there must be three parts, all numbers
			if (datepartcount==3 &&
				charstring::isNumber(dateparts[0]) &&
				charstring::isNumber(dateparts[1]) &&
				charstring::isNumber(dateparts[2])) {

				// it could be yyyy/xx/xx or xx/xx/yyyy
				if (charstring::length(dateparts[0])==4) {
					*year=charstring::toInteger(
								dateparts[0]);
					if (ddmm) {
						*day=charstring::toInteger(
								dateparts[1]);
						*month=charstring::toInteger(
								dateparts[2]);
					} else {
						*month=charstring::toInteger(
								dateparts[1]);
						*day=charstring::toInteger(
								dateparts[2]);
					}
				} else {
					if (ddmm) {
						*day=charstring::toInteger(
								dateparts[0]);
						*month=charstring::toInteger(
								dateparts[1]);
					} else {
						*month=charstring::toInteger(
								dateparts[0]);
						*day=charstring::toInteger(
								dateparts[1]);
					}
					*year=charstring::toInteger(
								dateparts[2]);
				}
			}

			// clean up
			for (uint64_t i=0; i<datepartcount; i++) {
				delete[] dateparts[i];
			}
			delete[] dateparts;

		} else if (charstring::contains(parts[i],'-')) {

			// the section with -'s is the date...

			// split on -
			char		**dateparts;
			uint64_t	datepartcount;
			charstring::split(parts[i],"-",1,true,
						&dateparts,&datepartcount);

			// there must be three parts, 0 and 2 must be numbers
			if (datepartcount==3 &&
				charstring::isNumber(dateparts[0]) &&
				charstring::isNumber(dateparts[2])) {

				// some dates have a non-numeric month in part 2
				if (!charstring::isNumber(dateparts[1])) {

					*day=charstring::toInteger(
								dateparts[0]);
					for (int i=0; shortmonths[i]; i++) {
						if (!charstring::
							compareIgnoringCase(
								dateparts[1],
								shortmonths[i]) 
							||
							!charstring::
							compareIgnoringCase(
								dateparts[1],
								longmonths[i]))
						{
							*month=i+1;
						}
					}
					*year=charstring::toInteger(
								dateparts[2]);
				} else {

					// it could be yyyy-xx-xx or xx-xx-yyyy
					if (charstring::length(
							dateparts[0])==4) {
						*year=charstring::toInteger(
								dateparts[0]);
						if (ddmm) {
							*day=
							charstring::toInteger(
								dateparts[1]);
							*month=
							charstring::toInteger(
								dateparts[2]);
						} else {
							*month=
							charstring::toInteger(
								dateparts[1]);
							*day=
							charstring::toInteger(
								dateparts[2]);
						}
					} else {
						if (ddmm) {
							*day=
							charstring::toInteger(
								dateparts[0]);
							*month=
							charstring::toInteger(
								dateparts[1]);
						} else {
							*month=
							charstring::toInteger(
								dateparts[0]);
							*day=
							charstring::toInteger(
								dateparts[1]);
						}
						*year=charstring::toInteger(
								dateparts[2]);
					}
				}
			}

			// clean up
			for (uint64_t i=0; i<datepartcount; i++) {
				delete[] dateparts[i];
			}
			delete[] dateparts;

		} else if (supportdotdelimiteddate &&
				charstring::contains(parts[i],'.')) {

			// the section with .'s is the date...

			// split on .
			char		**dateparts;
			uint64_t	datepartcount;
			charstring::split(parts[i],".",1,true,
						&dateparts,&datepartcount);

			// there must be three parts, 0 and 2 must be numbers
			if (datepartcount==3 &&
				charstring::isNumber(dateparts[0]) &&
				charstring::isNumber(dateparts[2])) {

				// some dates have a non-numeric month in part 2
				if (!charstring::isNumber(dateparts[1])) {

					*day=charstring::toInteger(
								dateparts[0]);
					for (int i=0; shortmonths[i]; i++) {
						if (!charstring::
							compareIgnoringCase(
								dateparts[1],
								shortmonths[i]) 
							||
							!charstring::
							compareIgnoringCase(
								dateparts[1],
								longmonths[i]))
						{
							*month=i+1;
						}
					}
					*year=charstring::toInteger(
								dateparts[2]);
				} else {

					// it could be yyyy.xx.xx or xx.xx.yyyy
					if (charstring::length(
							dateparts[0])==4) {
						*year=charstring::toInteger(
								dateparts[0]);
						if (ddmm) {
							*day=
							charstring::toInteger(
								dateparts[1]);
							*month=
							charstring::toInteger(
								dateparts[2]);
						} else {
							*month=
							charstring::toInteger(
								dateparts[1]);
							*day=
							charstring::toInteger(
								dateparts[2]);
						}
					} else {
						if (ddmm) {
							*day=
							charstring::toInteger(
								dateparts[0]);
							*month=
							charstring::toInteger(
								dateparts[1]);
						} else {
							*month=
							charstring::toInteger(
								dateparts[0]);
							*day=
							charstring::toInteger(
								dateparts[1]);
						}
						*year=charstring::toInteger(
								dateparts[2]);
					}
				}
			}

			// clean up
			for (uint64_t i=0; i<datepartcount; i++) {
				delete[] dateparts[i];
			}
			delete[] dateparts;
		}
	}

	// clean up
	for (uint64_t i=0; i<partcount; i++) {
		delete[] parts[i];
	}
	delete[] parts;

	// manage bad years
	if (*year!=-1) {
		if (*year<50) {
			*year=*year+2000;
		} else if (*year<100) {
			*year=*year+1900;
		} else if (*year>9999) {
			*year=9999;
		}
	}

	// manage bad months
	if (*month!=-1) {
		if (*month<1) {
			*month=1;
		} else if (*month>12) {
			*month=12;
		}
	}

	return true;
}

#endif
