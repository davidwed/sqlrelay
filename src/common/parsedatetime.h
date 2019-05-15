// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#ifndef PARSE_DATE_H
#define PARSE_DATE_H

#include <rudiments/charstring.h>
#include <rudiments/stdio.h>

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

static int16_t adjustHour(int16_t hour, const char *timestring) {
	if (hour<12 && charstring::contains(timestring,"PM")) {
		return hour+12;
	} else if (hour==12 && charstring::contains(timestring,"AM")) {
		return hour-12;
	}
	return hour;
}

static int32_t fractionToMicroseconds(const char *fraction) {

	int32_t	val=charstring::toInteger(fraction);
	if (!val) {
		return 0;
	}

	size_t	len=charstring::length(fraction);
	while (len<6) {
		val=val*10;
		len++;
	}
	while (len>6) {
		val=val/10;
		len--;
	}
	return val;
}

static bool parseDateTime(const char *datetime, bool ddmm, bool yyyyddmm,
			const char *datedelimiters,
			int16_t *year, int16_t *month, int16_t *day,
			int16_t *hour, int16_t *minute, int16_t *second,
			int32_t *microsecond, bool *isnegative) {

	bool	supportslashdelimiteddate=
			charstring::contains(datedelimiters,'/');
	bool	supportdashdelimiteddate=
			charstring::contains(datedelimiters,'-');
	bool	supportdotdelimiteddate=
			charstring::contains(datedelimiters,'.');
	bool	supportcolondelimiteddate=
			charstring::contains(datedelimiters,':');

	// initialize date/time parts
	*year=-1;
	*month=-1;
	*day=-1;
	*hour=-1;
	*minute=-1;
	*second=-1;
	*microsecond=-1;
	*isnegative=false;

	// different db's format dates very differently

	// split on a space
	char		**parts;
	uint64_t	partcount;
	charstring::split(datetime," ",1,true,&parts,&partcount);

	// there should be:
	// one (date/time only),
	// two (date and time),
	// three (eg. Feb 02 2012) parts,
	// or four (eg. Feb 02 2012 01:03:04:000AM)
	// parts
	if (!partcount || partcount>4) {
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
		delete[] parts;
		return false;
	}

	// initialize the return value;
	bool	retval=true;

	// 4-part dates are very different than the rest
	// sybase and ms sql server return these
	if (partcount==3 || partcount==4) {

		// part 1 is the month
		*month=0;
		for (int i=0; shortmonths[i]; i++) {
			if (!charstring::compareIgnoringCase(
						parts[0],shortmonths[i]) ||
				!charstring::compareIgnoringCase(
						parts[0],longmonths[i])) {
				*month=i+1;
			}
		}
		if (!*month) {
			retval=false;
		}

		// part 2 is the day
		*day=charstring::toInteger(parts[1]);

		// part 3 is the year
		*year=charstring::toInteger(parts[2]);

		// part 4 could be the time, we'll split it below...
	}

	// parse the parts
	for (uint64_t i=(partcount>2)?3:0; i<partcount && retval; i++) {

		if (charstring::contains(parts[i],':')) {

			// the section with :'s is probably the time...

			// split on :
			char		**timeparts;
			uint64_t	timepartcount;
			charstring::split(parts[i],":",1,true,
						&timeparts,&timepartcount);
	
			// there could be:
			// 2 parts, all numbers,
			//     (02:03)
			// 2 parts with AM/PM in part 2,
			//     (02:03AM)
			// 3 parts, all numbers,
			//     (02:03:04)
			// 3 parts with a decimal fraction and AM/PM in part 3,
			//     (02:03:04.123AM)
			// 3 parts with no decimal fraction and AM/PM in part 3,
			//     (02:03:04AM)
			// 3 parts with a decimal fraction and no AM/PM,
			//     (14:03:04.123)
			// 4 parts with a fractional part 4 and AM/PM,
			//     (02:03:04:123AM)
			// 4 parts with a fractional part 4 and no AM/PM
			//     (14:03:04:123)
			if (timepartcount==2 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1])) {

				*hour=charstring::toInteger(timeparts[0]);
				*minute=charstring::toInteger(timeparts[1]);
				*second=0;
				*microsecond=0;

			} else if (timepartcount==2 &&
				charstring::isNumber(timeparts[0]) &&
				((charstring::contains(timeparts[1],"AM") &&
				!*(charstring::findFirst(timeparts[1],"AM")+2))
				||
				(charstring::contains(timeparts[1],"PM") &&
				!*(charstring::findFirst(timeparts[1],"PM")+2)))
				) {

				if (timeparts[0][0]=='-') {
					*isnegative=true;
					*hour=charstring::toInteger(
							timeparts[0]+1);
				} else {
					*hour=charstring::toInteger(
							timeparts[0]);
				}
				*minute=charstring::toInteger(timeparts[1]);
				*second=0;
				*microsecond=0;
				*hour=adjustHour(*hour,timeparts[1]);

			} else if (timepartcount==3 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				charstring::isNumber(timeparts[2]) &&
				!charstring::contains(timeparts[2],'.')) {

				// well, if the first or last part is 4 digit
				// then it's a date (firebird uses
				// colon-delimited dates) otherwise it's a time
				if (supportcolondelimiteddate &&
					charstring::length(timeparts[0])==4) {
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
				} else if (supportcolondelimiteddate &&
					charstring::length(timeparts[2])==4) {
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
					if (timeparts[0][0]=='-') {
						*isnegative=true;
						*hour=charstring::toInteger(
								timeparts[0]+1);
					} else {
						*hour=charstring::toInteger(
								timeparts[0]);
					}
					*minute=charstring::toInteger(
								timeparts[1]);
					*second=charstring::toInteger(
								timeparts[2]);
					*microsecond=0;
				}

			} else if (timepartcount==3 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				charstring::contains(timeparts[2],'.') &&
				((charstring::contains(timeparts[2],"AM") &&
				!*(charstring::findFirst(timeparts[2],"AM")+2))
				||
				(charstring::contains(timeparts[2],"PM") &&
				!*(charstring::findFirst(timeparts[2],"PM")+2)))
				) {

				if (timeparts[0][0]=='-') {
					*isnegative=true;
					*hour=charstring::toInteger(
							timeparts[0]+1);
				} else {
					*hour=charstring::toInteger(
							timeparts[0]);
				}
				*minute=charstring::toInteger(timeparts[1]);
				*second=charstring::toInteger(timeparts[2]);
				const char	*dot=
					charstring::findFirst(timeparts[2],'.');
				*microsecond=fractionToMicroseconds(dot+1);
				*hour=adjustHour(*hour,timeparts[2]);

			} else if (timepartcount==3 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				((charstring::contains(timeparts[2],"AM") &&
				!*(charstring::findFirst(timeparts[2],"AM")+2))
				||
				(charstring::contains(timeparts[2],"PM") &&
				!*(charstring::findFirst(timeparts[2],"PM")+2)))
				) {

				if (timeparts[0][0]=='-') {
					*isnegative=true;
					*hour=charstring::toInteger(
							timeparts[0]+1);
				} else {
					*hour=charstring::toInteger(
							timeparts[0]);
				}
				*minute=charstring::toInteger(timeparts[1]);
				*second=charstring::toInteger(timeparts[2]);
				*microsecond=0;
				*hour=adjustHour(*hour,timeparts[2]);

			} else if (timepartcount==3 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				charstring::contains(timeparts[2],'.')) {

				if (timeparts[0][0]=='-') {
					*isnegative=true;
					*hour=charstring::toInteger(
							timeparts[0]+1);
				} else {
					*hour=charstring::toInteger(
							timeparts[0]);
				}
				*minute=charstring::toInteger(timeparts[1]);
				*second=charstring::toInteger(timeparts[2]);
				const char	*dot=
					charstring::findFirst(timeparts[2],'.');
				*microsecond=fractionToMicroseconds(dot+1);

			} else if (timepartcount==4 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				charstring::isNumber(timeparts[2]) &&
				((charstring::contains(timeparts[3],"AM") &&
				!*(charstring::findFirst(timeparts[3],"AM")+2))
				||
				(charstring::contains(timeparts[3],"PM") &&
				!*(charstring::findFirst(timeparts[3],"PM")+2)))
				) {

				if (timeparts[0][0]=='-') {
					*isnegative=true;
					*hour=charstring::toInteger(
							timeparts[0]+1);
				} else {
					*hour=charstring::toInteger(
							timeparts[0]);
				}
				*minute=charstring::toInteger(timeparts[1]);
				*second=charstring::toInteger(timeparts[2]);
				*microsecond=
					fractionToMicroseconds(timeparts[3]);
				*hour=adjustHour(*hour,timeparts[3]);

			} else if (timepartcount==4 &&
				charstring::isNumber(timeparts[0]) &&
				charstring::isNumber(timeparts[1]) &&
				charstring::isNumber(timeparts[2]) &&
				charstring::isNumber(timeparts[3])) {

				if (timeparts[0][0]=='-') {
					*isnegative=true;
					*hour=charstring::toInteger(
							timeparts[0]+1);
				} else {
					*hour=charstring::toInteger(
							timeparts[0]);
				}
				*minute=charstring::toInteger(timeparts[1]);
				*second=charstring::toInteger(timeparts[2]);
				*microsecond=
					fractionToMicroseconds(timeparts[3]);

			} else {
				retval=false;
			}

			// clean up
			for (uint64_t j=0; j<timepartcount; j++) {
				delete[] timeparts[j];
			}
			delete[] timeparts;

		} else if (supportslashdelimiteddate &&
				charstring::contains(parts[i],'/')) {

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
			} else {
				retval=false;
			}

			// clean up
			for (uint64_t j=0; j<datepartcount; j++) {
				delete[] dateparts[j];
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
					for (int j=0; shortmonths[j]; j++) {
						if (!charstring::
							compareIgnoringCase(
								dateparts[1],
								shortmonths[j]) 
							||
							!charstring::
							compareIgnoringCase(
								dateparts[1],
								longmonths[j]))
						{
							*month=j+1;
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
			} else {
				retval=false;
			}

			// clean up
			for (uint64_t j=0; j<datepartcount; j++) {
				delete[] dateparts[j];
			}
			delete[] dateparts;

		} else if (supportdashdelimiteddate &&
				charstring::contains(parts[i],'-')) {

			// the section with -'s is the date...
			// (a time can also start with a - (indicating that
			// it's a negative interval) but that should should
			// have been caught above because it also contain :'s)

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
					for (int j=0; shortmonths[j]; j++) {
						if (!charstring::
							compareIgnoringCase(
								dateparts[1],
								shortmonths[j]) 
							||
							!charstring::
							compareIgnoringCase(
								dateparts[1],
								longmonths[j]))
						{
							*month=j+1;
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
						if (yyyyddmm) {
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
			} else {
				retval=false;
			}

			// clean up
			for (uint64_t j=0; j<datepartcount; j++) {
				delete[] dateparts[j];
			}
			delete[] dateparts;
		} else {
			retval=false;
		}
	}

	// clean up
	for (uint64_t j=0; j<partcount; j++) {
		delete[] parts[j];
	}
	delete[] parts;

	// Hmmm... this causes problems for validation of bad months
	/*if (retval) {

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
	}*/

	return retval;
}

#ifdef NEED_CONVERT_DATE_TIME
static char *convertDateTime(const char *format,
			int16_t year, int16_t month, int16_t day,
			int16_t hour, int16_t minute, int16_t second,
			int32_t microsecond, bool isnegative) {

	// if no format was passed in
	if (!format) {
		return NULL;
	}

	// normalize times
	day=(day>0)?day:1;
	month=(month>0)?month:1;
	year=(year>0)?year:1;
	hour=(hour>0)?hour:0;
	minute=(minute>0)?minute:0;
	second=(second>0)?second:0;
	microsecond=(microsecond>0)?microsecond:0;

	// output buffer
	stringbuffer	output;

	// work buffer
	char		buf[5];

	// run through the format string
	const char	*ptr=format;
	while (*ptr) {

		if (!charstring::compare(ptr,"DD",2)) {
			charstring::printf(buf,5,"%02d",day);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"MM",2)) {
			charstring::printf(buf,5,"%02d",month);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"MON",3)) {
			output.append(shortmonths[month-1]);
			ptr=ptr+3;
		} else if (!charstring::compare(ptr,"Month",5)) {
			output.append(longmonths[month-1]);
			ptr=ptr+3;
		} else if (!charstring::compare(ptr,"YYYY",4)) {
			charstring::printf(buf,5,"%04d",year);
			output.append(buf);
			ptr=ptr+4;
		} else if (!charstring::compare(ptr,"YY",2)) {
			charstring::printf(buf,5,"%04d",year);
			output.append(buf+2);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"HH24",4)) {
			charstring::printf(buf,5,"%02d",hour);
			output.append(buf);
			ptr=ptr+4;
		} else if (!charstring::compare(ptr,"HH",2)) {
			charstring::printf(buf,5,"%s%02d",
						(isnegative)?"-":"",
						(hour<13)?hour:hour-12);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"MI",2)) {
			charstring::printf(buf,5,"%02d",minute);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"SS",2)) {
			charstring::printf(buf,5,"%02d",second);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"FFFFFF",6)) {
			charstring::printf(buf,5,"%06d",microsecond);
			output.append(buf);
			ptr=ptr+6;
		} else if (!charstring::compare(ptr,"FFFFF",5)) {
			charstring::printf(buf,5,"%05d",microsecond/10);
			output.append(buf);
			ptr=ptr+5;
		} else if (!charstring::compare(ptr,"FFFF",4)) {
			charstring::printf(buf,5,"%04d",microsecond/100);
			output.append(buf);
			ptr=ptr+4;
		} else if (!charstring::compare(ptr,"FFF",3)) {
			charstring::printf(buf,5,"%03d",microsecond/1000);
			output.append(buf);
			ptr=ptr+3;
		} else if (!charstring::compare(ptr,"FF",2)) {
			charstring::printf(buf,5,"%02d",microsecond/10000);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"F",1)) {
			charstring::printf(buf,5,"%01d",microsecond/100000);
			output.append(buf);
			ptr=ptr+2;
		} else if (!charstring::compare(ptr,"AM",2)) {
			output.append((hour<12)?"AM":"PM");
			ptr=ptr+2;
		} else {
			output.append(*ptr);
			ptr=ptr+1;
		}
	}

	return output.detachString();
}
#endif

#endif
