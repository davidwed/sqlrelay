// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class period {
	public:
		uint16_t	start;
		uint16_t	end;
};

class daypart {
	public:
		uint16_t	starthour;
		uint16_t	startminute;
		uint16_t	endhour;
		uint16_t	endminute;
};

class rule {
	public:
		rule(bool allow, const char *when);
		rule(bool allow,
			const char *years,
			const char *months,
			const char *daysofmonth,
			const char *daysofweek,
			const char *dayparts);
		~rule();

		bool	allowed(datetime *dt, bool currentlyallowed);
		
	private:
		void	init(bool allow,
				const char *years,
				const char *months,
				const char *daysofmonth,
				const char *daysofweek,
				const char *dayparts);
		void	splitTimePart(linkedlist< period * > *periods,
						const char *timepartlist);
		void	splitDayParts(const char *daypartlist);
		bool	inPeriods(linkedlist< period * > *periods,
						int32_t timepart);
		bool	inDayParts(int32_t hour, int32_t minute);

		bool			allow;
		linkedlist< period * >	years;
		linkedlist< period * >	months;
		linkedlist< period * >	daysofmonth;
		linkedlist< period * >	daysofweek;
		linkedlist< daypart * >	dayparts;
};

rule::rule(bool allow, const char *when) {

	char		**whenparts;
	uint64_t	whenpartscount;
	charstring::split(when," ",true,&whenparts,&whenpartscount);

	if (whenpartscount==5) {
		init(allow,whenparts[0],whenparts[1],
			whenparts[2],whenparts[3],whenparts[4]);
	}

	for (uint64_t i=0; i<whenpartscount; i++) {
		delete[] whenparts[i];
	}
	delete[] whenparts;
}

rule::rule(bool allow, const char *years,
			const char *months,
			const char *daysofmonth,
			const char *daysofweek,
			const char *dayparts) {
	init(allow,years,months,daysofmonth,daysofweek,dayparts);
}

rule::~rule() {
	for (linkedlistnode< period * > *n=years.getFirst();
						n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< period * > *n=months.getFirst();
						n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< period * > *n=daysofmonth.getFirst();
						n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< period * > *n=daysofweek.getFirst();
						n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< daypart * > *n=dayparts.getFirst();
						n; n=n->getNext()) {
		delete n->getValue();
	}
}

bool rule::allowed(datetime *dt, bool currentlyallowed) {

	// if ths rule applies...
	if (inPeriods(&years,dt->getYear()) &&
		inPeriods(&months,dt->getMonth()) &&
		inPeriods(&daysofmonth,dt->getDayOfMonth()) &&
		inPeriods(&daysofweek,dt->getDayOfWeek()) &&
		inDayParts(dt->getHour(),dt->getMinutes())) {

		// if the rule contradicts the current state then
		// return the opposite of the current state
		if (allow!=currentlyallowed) {
			return !currentlyallowed;
		}
	}

	// if the rule doesn't apply or didn't contradict
	// the current state then return the current state
	return currentlyallowed;
}

void rule::init(bool allow,
			const char *years,
			const char *months,
			const char *daysofmonth,
			const char *daysofweek,
			const char *dayparts) {
	this->allow=allow;
	splitTimePart(&(this->years),years);
	splitTimePart(&(this->months),months);
	splitTimePart(&(this->daysofmonth),daysofmonth);
	splitTimePart(&(this->daysofweek),daysofweek);
	splitDayParts(dayparts);
}

bool rule::inPeriods(linkedlist< period * > *periods, int32_t timepart) {

	#ifdef DEBUG_MESSAGES
		if (periods==&years) {
			stdoutput.printf("years...\n");
		}
		if (periods==&months) {
			stdoutput.printf("months...\n");
		}
		if (periods==&daysofmonth) {
			stdoutput.printf("daysofmonth...\n");
		}
		if (periods==&daysofweek) {
			stdoutput.printf("daysofweek...\n");
		}
	#endif
	
	for (linkedlistnode< period * > *pn=periods->getFirst();
						pn; pn=pn->getNext()) {

		period	*p=pn->getValue();

		debugPrintf("	%d>=%d && %d<=%d - ",
				timepart,p->start,timepart,p->end);

		if (timepart>=p->start && timepart<=p->end) {
			debugPrintf("yes\n");
			return true;
		}

		debugPrintf("no\n");
	}
	return false;
}

bool rule::inDayParts(int32_t hour, int32_t minute) {

	debugPrintf("dayparts...\n");

	for (linkedlistnode< daypart * > *dpn=dayparts.getFirst();
						dpn; dpn=dpn->getNext()) {

		daypart	*dp=dpn->getValue();

		debugPrintf("	%d:%02d>=%hd:%02hd && ",
				hour,minute,dp->starthour,dp->startminute);
		debugPrintf("%d:%02d<=%hd:%02hd - ",
				hour,minute,dp->endhour,dp->endminute);

		if (hour>=dp->starthour && minute>=dp->startminute &&
			hour<=dp->endhour && minute<=dp->endminute) {
			debugPrintf("yes\n");
			return true;
		}

		debugPrintf("no\n");
	}
	return false;
}

void rule::splitTimePart(linkedlist< period * > *periods,
					const char *timepartlist) {

	// handle *'s
	if (!charstring::compare(timepartlist,"*")) {
		period	*p=new period;
		if (periods==&years) {
			p->start=0;
			// FIXME: is there a macro for this?
			p->end=65535;
		}
		if (periods==&months) {
			p->start=1;
			p->end=12;
		}
		if (periods==&daysofmonth) {
			p->start=1;
			p->end=31;
		}
		if (periods==&daysofweek) {
			p->start=1;
			p->end=7;
		}
		periods->append(p);
		return;
	}

	// split timepartlist on comma
	char		**timeparts;
	uint64_t	timepartscount;
	charstring::split(timepartlist,",",true,&timeparts,&timepartscount);

	// for each of those...
	for (uint64_t i=0; i<timepartscount; i++) {

		// split them on dash
		char		**timepartparts;
		uint64_t	timepartpartscount;
		charstring::split(timeparts[i],"-",true,
					&timepartparts,
					&timepartpartscount);

		// create a new period, set the start/end
		// and add it to the list of periods
		period	*p=new period;
		p->start=charstring::toInteger(timepartparts[0]);
		if (timepartpartscount>1) {
			p->end=charstring::toInteger(timepartparts[1]);
		} else {
			p->end=p->start;
		}
		periods->append(p);

		// clean up
		for (uint64_t j=0; j<timepartpartscount; j++) {
			delete[] timepartparts[j];
		}
		delete[] timepartparts;
		delete[] timeparts[i];
	}

	// clean up
	delete[] timeparts;
}

void rule::splitDayParts(const char *daypartlist) {

	// handle *'s
	if (!charstring::compare(daypartlist,"*")) {
		daypart	*dp=new daypart;
		dp->starthour=0;
		dp->startminute=0;
		dp->endhour=23;
		dp->endminute=59;
		dayparts.append(dp);
		return;
	}

	// split daypartlist on comma
	char		**dayparts;
	uint64_t	daypartscount;
	charstring::split(daypartlist,",",true,&dayparts,&daypartscount);

	// for each of those...
	for (uint64_t i=0; i<daypartscount; i++) {

		// split them on dash
		char		**daypartparts;
		uint64_t	daypartpartscount;
		charstring::split(dayparts[i],"-",true,
					&daypartparts,
					&daypartpartscount);

		// create a new daypart, set the start/end
		// hour/minute and add it to the list of periods
		daypart	*dp=new daypart;
		dp->starthour=charstring::toInteger(daypartparts[0]);
		const char	*minute=
				charstring::findFirst(daypartparts[0],":");
		if (minute) {
			minute++;
		} else {
			minute="0";
		}
		dp->startminute=charstring::toInteger(minute);

		if (daypartpartscount>1) {
			dp->endhour=charstring::toInteger(daypartparts[1]);
			minute=charstring::findFirst(daypartparts[1],":");
			if (minute) {
				minute++;
			} else {
				minute="0";
			}
			dp->endminute=charstring::toInteger(minute);
		} else {
			dp->endhour=dp->starthour;
			dp->endminute=dp->startminute;
		}
		this->dayparts.append(dp);

		// clean up
		for (uint64_t j=0; j<daypartpartscount; j++) {
			delete[] daypartparts[j];
		}
		delete[] daypartparts;
		delete[] dayparts[i];
	}

	// clean up
	delete[] dayparts;
}

class SQLRSERVER_DLLSPEC sqlrschedule_cron : public sqlrschedule {
	public:
			sqlrschedule_cron(xmldomnode *parameters);
			~sqlrschedule_cron();

		bool	allowed(sqlrserverconnection *sqlrcon,
						const char *user);
	private:
		bool	enabled;
		bool	defaultallow;

		linkedlist< rule * >	rules;
};

sqlrschedule_cron::sqlrschedule_cron(xmldomnode *parameters) :
						sqlrschedule(parameters) {

	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	defaultallow=charstring::compareIgnoringCase(
			parameters->getAttributeValue("default"),"deny");

	// parse the rules
	for (xmldomnode *rn=parameters->getFirstTagChild("rules")->
							getFirstTagChild();
					!rn->isNullNode();
					rn=rn->getNextTagSibling()) {

		if (!charstring::compare(rn->getName(),"allow") ||
			!charstring::compare(rn->getName(),"deny")) {

			rules.append(new rule(
				charstring::compareIgnoringCase(
						rn->getName(),"deny"),
						rn->getAttributeValue("when")));
		}
	}
}

sqlrschedule_cron::~sqlrschedule_cron() {
	for (linkedlistnode< rule * > *t=rules.getFirst(); t; t=t->getNext()) {
		delete t->getValue();
	}
}

bool sqlrschedule_cron::allowed(sqlrserverconnection *sqlrcon,
							const char *user) {

	if (!enabled) {
		debugPrintf("module disabled\n");
		return true;
	}

	// FIXME: handle GSS/TLS users...

	// do we care about this user?
	bool	found=false;
	debugPrintf("user...\n");
	for (xmldomnode *un=parameters->
				getFirstTagChild("users")->
				getFirstTagChild("user");
			!un->isNullNode();
			un=un->getNextTagSibling("user")) {

		debugPrintf("	%s=%s - ",user,un->getAttributeValue("user"));
		if (!charstring::compare(user,un->getAttributeValue("user"))) {
			found=true;
			debugPrintf("yes\n");
			break;
		}
		debugPrintf("no\n");
	}
	if (!found) {
		debugPrintf("	user not found, not applying any schedule\n\n");
		return true;
	}

	// compare date/time to schedule rules
	datetime	dt;
	dt.getSystemDateAndTime();

	bool	currentlyallowed=defaultallow;
	for (linkedlistnode< rule * > *t=rules.getFirst(); t; t=t->getNext()) {
		currentlyallowed=t->getValue()->allowed(&dt,currentlyallowed);
	}
	return currentlyallowed;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrschedule *new_sqlrschedule_cron(
						xmldomnode *parameters) {
		return new sqlrschedule_cron(parameters);
	}
}
