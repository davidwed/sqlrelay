// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/logger.h>
#include <rudiments/process.h>
#include <rudiments/sys.h>
#include <rudiments/signalclasses.h>
#include <rudiments/error.h>

class sqlrserverbaseprivate {
	friend class sqlrserverbase;
	private:
		logger			_lg;
		stdoutdestination	_sod;
		bool			_debug;
		uint16_t		_indent;
		stringbuffer		_logbuffer;
};

static signalhandler		alarmhandler;
static volatile sig_atomic_t	alarmrang=0;

sqlrserverbase::sqlrserverbase() {
	pvt=new sqlrserverbaseprivate;
	pvt->_debug=false;
	pvt->_indent=0;
	pvt->_lg.setLogLevel(1);
	pvt->_lg.addLogDestination(&pvt->_sod);
}

sqlrserverbase::~sqlrserverbase() {
	delete pvt;
}

void sqlrserverbase::init() {

	// set a handler for SIGALRMs
	#ifdef SIGALRM
	alarmhandler.setHandler(alarmHandler);
	alarmhandler.handleSignal(SIGALRM);
	#endif
}

void sqlrserverbase::setDebug(bool debug) {
	pvt->_debug=debug;
}

bool sqlrserverbase::getDebug() {
	return pvt->_debug;
}

void sqlrserverbase::debugStart(const char *title, ...) {
	if (!pvt->_debug) {
		return;
	}
	pvt->_logbuffer.clear();
	if (!pvt->_indent) {
		pvt->_logbuffer.append((uint64_t)process::getProcessId());
		pvt->_logbuffer.append(": ");
	}
	va_list	argp;
	va_start(argp,title);
	pvt->_logbuffer.printf(title,&argp);
	va_end(argp);
	pvt->_lg.start(1,NULL,pvt->_indent,pvt->_logbuffer.getString());
	pvt->_indent++;
}

void sqlrserverbase::debugWrite(const char *string, ...) {
	if (!pvt->_debug) {
		return;
	}
	va_list	argp;
	va_start(argp,string);
	pvt->_lg.write(1,NULL,pvt->_indent,string,&argp);
	va_end(argp);
}

void sqlrserverbase::debugHexDump(const byte_t *data, uint64_t size) {
	if (!pvt->_debug) {
		return;
	}
	pvt->_logbuffer.clear();
	pvt->_logbuffer.printHex(data,size,0);
	pvt->_lg.write(1,NULL,0,pvt->_logbuffer.getString());
}

void sqlrserverbase::debugEnd() {
	if (!pvt->_debug) {
		return;
	}
	pvt->_indent--;
	pvt->_lg.end(1,(const char *)NULL,pvt->_indent);
}

bool sqlrserverbase::semWait(semaphoreset *semset,
					int32_t index,
					thread *thr,
					bool withundo,
					int32_t	timeout,
					bool *timedout) {

	bool	result=true;
	if (timedout) {
		*timedout=false;
	}
	if (timeout>0 && semset->supportsTimedSemaphoreOperations()) {
		if (withundo) {
			result=semset->waitWithUndo(index,timeout,0);
		} else {
			result=semset->wait(index,timeout,0);
		}
		if (timedout) {
			*timedout=(!result && error::getErrorNumber()==EAGAIN);
		}
	} else if (timeout>0 && !thr && sys::getSignalsInterruptSystemCalls()) {
		// We can't use this when using threads because alarmrang isn't
		// thread-local and there's no way to make it be.  Also, the
		// alarm doesn't reliably interrupt the wait() when it's called
		// from a thread, at least not on Linux.  Hopefully platforms
		// that supports threads also supports timed semaphore ops.
		semset->setRetryInterruptedOperations(false);
		alarmrang=0;
		signalmanager::alarm(timeout);
		do {
			if (withundo) {
				result=semset->waitWithUndo(index);
			} else {
				result=semset->wait(index);
			}
		} while (!result && error::getErrorNumber()==EINTR &&
				!process::getShutDownFlag() &&
				alarmrang!=1);
		if (timedout) {
			*timedout=(alarmrang==1);
		}
		signalmanager::alarm(0);
		semset->setRetryInterruptedOperations(true);
	} else {
		if (withundo) {
			result=semset->waitWithUndo(index);
		} else {
			result=semset->wait(index);
		}
	}

	return result;
}

void sqlrserverbase::alarmHandler(int32_t signum) {
	alarmrang=1;
}
