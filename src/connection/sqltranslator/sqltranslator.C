// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqltranslator.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslatordebug.h>

sqltranslator::sqltranslator() {
	debugFunction();
	xmld=NULL;
	tree=NULL;
	sqlrcon=NULL;
	sqlrcur=NULL;
}

sqltranslator::~sqltranslator() {
	debugFunction();
	delete xmld;
}

bool sqltranslator::loadRules(const char *rules) {
	debugFunction();
	delete xmld;
	xmld=new xmldom();
	if (xmld->parseString(rules)) {
		rulesnode=xmld->getRootNode()->
				getFirstTagChild("sqltranslationrules");
		return !rulesnode->isNullNode();
	}
	return false;
}

bool sqltranslator::applyRules(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	tree=querytree;
	return applyRulesToQuery(querytree->getRootNode());
}

bool sqltranslator::applyRulesToQuery(xmldomnode *query) {
	debugFunction();

	for (xmldomnode *rule=rulesnode->getFirstTagChild();
		!rule->isNullNode(); rule=rule->getNextTagSibling()) {

		const char	*rulename=rule->getName();

		if (!charstring::compare(rulename,
					"nativize_datatypes")) {
			if (!nativizeDatatypes(query,rule)) {
				return false;
			}
		} else if (!charstring::compare(rulename,
					"convert_datatypes")) {
			if (!convertDatatypes(query,rule)) {
				return false;
			}
		} else if (!charstring::compare(rulename,
				"trim_columns_compared_to_string_binds")) {
			if (!trimColumnsComparedToStringBinds(query,rule)) {
				return false;
			}
		} else if (!charstring::compare(rulename,
				"nativize_date_times")) {
			if (!nativizeDateTimes(query,rule)) {
				return false;
			}
		}
	}
	return true;
}

bool sqltranslator::nativizeDatatypes(xmldomnode *query, xmldomnode *rule) {
	debugFunction();
	return true;
}

bool sqltranslator::convertDatatypes(xmldomnode *query, xmldomnode *rule) {
	debugFunction();
	return true;
}


bool sqltranslator::nativizeDateTimesInQuery(xmldomnode *querynode,
							xmldomnode *rule) {
	debugFunction();

	// convert this node...
	if (!charstring::compare(querynode->getName(),sqlparser::_verbatim)) {

		// get the value
		const char	*value=querynode->getAttributeValue(
							sqlparser::_value);

		// leave it alone unless it's a string
		if (isString(value)) {

			// copy it and strip off the quotes
			char	*valuecopy=charstring::duplicate(value+1);
			valuecopy[charstring::length(valuecopy)-1]='\0';

			// variables
			int16_t	year=-1;
			int16_t	month=-1;
			int16_t	day=-1;
			int16_t	hour=-1;
			int16_t	minute=-1;
			int16_t	second=-1;
	
			// parse the date/time
			if (parseDateTime(valuecopy,&year,&month,&day,
						&hour,&minute,&second)) {

				// convert it
				char	*converted=convertDateTime(
							year,month,day,
							hour,minute,second);
				if (converted) {

					// repackage as a string
					stringbuffer	output;
					output.append('\'');
					output.append(converted);
					output.append('\'');

					// update the value
					setAttribute(querynode,
							sqlparser::_value,
							output.getString());

					// clean up
					delete[] converted;
				}
			}
	
			// clean up
			delete[] valuecopy;
		}
	}

	// convert child nodes...
	for (xmldomnode *node=querynode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!nativizeDateTimesInQuery(node,rule)) {
			return false;
		}
	}
	return true;
}

bool sqltranslator::nativizeDateTimesInBindVariables(
						xmldomnode *querynode,
						xmldomnode *rule) {
	debugFunction();

	// run through the bind variables...
	for (uint16_t i=0; i<sqlrcur->inbindcount; i++) {

		// get the variable
		bindvar_svr	*bind=&(sqlrcur->inbindvars[i]);

		// ignore non-strings...
		if (bind->type!=STRING_BIND) {
			continue;
		}

		// variables
		int16_t	year=-1;
		int16_t	month=-1;
		int16_t	day=-1;
		int16_t	hour=-1;
		int16_t	minute=-1;
		int16_t	second=-1;
	
		// parse the date/time
		if (!parseDateTime(bind->value.stringval,
					&year,&month,&day,
					&hour,&minute,&second)) {
			continue;
		}

		// attempt to convert the value
		char	*converted=convertDateTime(year,month,day,
							hour,minute,second);
		if (!converted) {
			continue;
		}

		// replace the value with the converted string
		bind->valuesize=charstring::length(converted)+1;
		bind->value.stringval=
			(char *)sqlrcon->bindmappingspool->
				malloc(bind->valuesize);
		charstring::append(bind->value.stringval,converted);
		delete[] converted;
	}

	return true;
}

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

const char * const *sqltranslator::getShortMonths() {
	return shortmonths;
}

const char * const *sqltranslator::getLongMonths() {
	return longmonths;
}

bool sqltranslator::parseDateTime(const char *datetime,
			int16_t *year, int16_t *month, int16_t *day,
			int16_t *hour, int16_t *minute, int16_t *second) {
printf("parsing: \"%s\"\n",datetime);

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

			// the section with :'s is the time...

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

				*hour=charstring::toInteger(timeparts[0]);
				*minute=charstring::toInteger(timeparts[1]);
				*second=charstring::toInteger(timeparts[2]);
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

				*month=charstring::toInteger(dateparts[0]);
				*day=charstring::toInteger(dateparts[1]);
				*year=charstring::toInteger(dateparts[2]);
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
							*month=i;
						}
					}
					*year=charstring::toInteger(
								dateparts[2]);
				} else {

					*year=charstring::toInteger(
								dateparts[0]);
					*month=charstring::toInteger(
								dateparts[1]);
					*day=charstring::toInteger(
								dateparts[2]);
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

	// manage 2-digit years
	if (*year!=-1) {
		if (*year<50) {
			*year=*year+2000;
		} else if (*year<100) {
			*year=*year+1900;
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

	// If there were 2 parts then we should have found all components.
	// If there was 1 part then we should have found one set or the other.
	return ((partcount==2 &&
			*year!=-1 && *month!=-1 && *day!=-1 &&
			*hour!=-1 && *minute!=-1 && *second!=-1) ||
		(partcount==1 &&
			((*year!=-1 && *month!=-1 && *day!=-1) ||
			(*hour!=-1 && *minute!=-1 && *second!=-1))));
}

char *sqltranslator::convertDateTime(
			int16_t year, int16_t month, int16_t day,
			int16_t hour, int16_t minute, int16_t second) {

	// buffer
	stringbuffer	output;

	// convert date
	bool	datepart=false;
	if (year!=-1 && month!=-1 && day!=-1) {
		char	newdate[12];
		snprintf(newdate,12,"%02d-%s-%04d",
				day,shortmonths[month-1],year);
		output.append(newdate);
		datepart=true;
	}

	// convert time
	if (hour!=-1 && minute!=-1 && second!=-1) {
		char	newtime[10];
		snprintf(newtime,10,"%02d:%02d:%02d",hour,minute,second);
		if (datepart) {
			output.append(' ');
		}
		output.append(newtime);
	}
printf("converted: \"%s\"\n",output.getString());
	return output.detachString();
}

bool sqltranslator::trimColumnsComparedToStringBinds(xmldomnode *query,
							xmldomnode *rule) {
	debugFunction();
	return true;
}

bool sqltranslator::nativizeDateTimes(xmldomnode *query, xmldomnode *rule) {
	debugFunction();
	return nativizeDateTimesInBindVariables(query,rule) &&
			nativizeDateTimesInQuery(query,rule);
}

xmldomnode *sqltranslator::newNode(xmldomnode *parentnode, const char *type) {
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->appendChild(retval);
	return retval;
}

xmldomnode *sqltranslator::newNode(xmldomnode *parentnode,
				const char *type, const char *value) {
	xmldomnode	*node=newNode(parentnode,type);
	setAttribute(node,sqlparser::_value,value);
	return node;
}

xmldomnode *sqltranslator::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type) {

	// find the position after "node"
	uint64_t	position=1;
	for (xmldomnode *child=parentnode->getChild((uint64_t)0);
		!child->isNullNode(); child=child->getNextSibling()) {
		if (child==node) {
			break;
		}
		position++;
	}

	// create a new node and insert it at that position
	xmldomnode	*retval=new xmldomnode(tree,parentnode->getNullNode(),
						TAG_XMLDOMNODETYPE,type,NULL);
	parentnode->insertChild(retval,position);
	return retval;
}

xmldomnode *sqltranslator::newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value) {
	xmldomnode	*retval=newNodeAfter(parentnode,node,type);
	setAttribute(retval,sqlparser::_value,value);
	return retval;
}

void sqltranslator::setAttribute(xmldomnode *node,
				const char *name, const char *value) {
	// FIXME: I shouldn't have to do this.
	// setAttribute should append it automatically
	if (node->getAttribute(name)!=node->getNullNode()) {
		node->setAttributeValue(name,value);
	} else {
		node->appendAttribute(name,value);
	}
}

bool sqltranslator::isString(const char *value) {
	size_t	length=charstring::length(value);
	return ((value[0]=='\'' && value[length-1]=='\'') ||
			(value[0]=='"' && value[length-1]=='"'));
}
