// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

sqlrschedulerule::sqlrschedulerule(bool allow, const char *when) {

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

sqlrschedulerule::sqlrschedulerule(bool allow,
					const char *years,
					const char *months,
					const char *daysofmonth,
					const char *daysofweek,
					const char *dayparts) {
	init(allow,years,months,daysofmonth,daysofweek,dayparts);
}

sqlrschedulerule::~sqlrschedulerule() {
	for (linkedlistnode< sqlrscheduleperiod * > *n=years.getFirst();
							n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< sqlrscheduleperiod * > *n=months.getFirst();
							n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< sqlrscheduleperiod * > *n=daysofmonth.getFirst();
							n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< sqlrscheduleperiod * > *n=daysofweek.getFirst();
							n; n=n->getNext()) {
		delete n->getValue();
	}
	for (linkedlistnode< sqlrscheduledaypart * > *n=dayparts.getFirst();
							n; n=n->getNext()) {
		delete n->getValue();
	}
}

bool sqlrschedulerule::allowed(datetime *dt, bool currentlyallowed) {

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

void sqlrschedulerule::init(bool allow,
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

bool sqlrschedulerule::inPeriods(
			linkedlist< sqlrscheduleperiod * > *periods,
			int32_t timepart) {

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
	
	for (linkedlistnode< sqlrscheduleperiod * > *pn=periods->getFirst();
							pn; pn=pn->getNext()) {

		sqlrscheduleperiod	*p=pn->getValue();

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

bool sqlrschedulerule::inDayParts(int32_t hour, int32_t minute) {

	debugPrintf("dayparts...\n");

	for (linkedlistnode< sqlrscheduledaypart * >
				*dpn=dayparts.getFirst();
				dpn; dpn=dpn->getNext()) {

		sqlrscheduledaypart	*dp=dpn->getValue();

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

void sqlrschedulerule::splitTimePart(
				linkedlist< sqlrscheduleperiod * > *periods,
				const char *timepartlist) {

	// handle *'s
	if (!charstring::compare(timepartlist,"*")) {
		sqlrscheduleperiod	*p=new sqlrscheduleperiod;
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
		sqlrscheduleperiod	*p=new sqlrscheduleperiod;
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

void sqlrschedulerule::splitDayParts(const char *daypartlist) {

	// handle *'s
	if (!charstring::compare(daypartlist,"*")) {
		sqlrscheduledaypart	*dp=new sqlrscheduledaypart;
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
		sqlrscheduledaypart	*dp=new sqlrscheduledaypart;
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




sqlrschedule::sqlrschedule(xmldomnode *parameters) {
	this->parameters=parameters;
}

sqlrschedule::~sqlrschedule() {
	for (linkedlistnode< sqlrschedulerule * > *r=rules.getFirst();
							r; r=r->getNext()) {
		delete r->getValue();
	}
}

bool sqlrschedule::init(sqlrserverconnection *sqlrcon) {
	return true;
}

bool sqlrschedule::allowed(sqlrserverconnection *sqlrcon, const char *user) {
	return true;
}

void sqlrschedule::addRule(bool allow, const char *when) {
	rules.append(new sqlrschedulerule(allow,when));
}

void sqlrschedule::addRule(bool allow,
				const char *years, const char *months,
				const char *daysofmonth, const char *daysofweek,
				const char *dayparts) {
	rules.append(new sqlrschedulerule(allow,years,months,
				daysofmonth,daysofweek,dayparts));
}

bool sqlrschedule::rulesAllow(datetime *dt, bool currentlyallowed) {
	for (linkedlistnode< sqlrschedulerule * > *r=rules.getFirst();
							r; r=r->getNext()) {
		currentlyallowed=r->getValue()->allowed(dt,currentlyallowed);
	}
	return currentlyallowed;
}
