// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>
#include <rudiments/snooze.h>
#include <rudiments/charstring.h>
#include <rudiments/datetime.h>
#include <rudiments/file.h>
#include <rudiments/directory.h>
#include <rudiments/process.h>
#include <rudiments/permissions.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

class dirnode {
	friend class sqlrcachemanager;
	private:
			dirnode(const char *dirname);
			dirnode(const char *start, const char *end);
			~dirnode();
		char	*dirname;
		dirnode	*next;
};

class sqlrcachemanager {
	public:
			sqlrcachemanager(int argc, const char **argv);
			~sqlrcachemanager();
		void	scan();
	private:
		void	erase(const char *dirname, const char *filename);
		void	parseCacheDirs(const char *cachedirs);

		int	scaninterval;
		dirnode	*firstdir;
		dirnode	*currentdir;

		sqlrcmdline	*cmdl;
		sqlrpaths	*sqlrpth;

		char	*pidfile;
};

dirnode::dirnode(const char *dirname) {
	this->dirname=charstring::duplicate(dirname);
	next=NULL;
}

dirnode::dirnode(const char *start, const char *end) {
	dirname=charstring::duplicate(start,end-start);
	next=NULL;
}

dirnode::~dirnode() {
	delete[] dirname;
}


sqlrcachemanager::sqlrcachemanager(int argc, const char **argv) {

	cmdl=new sqlrcmdline(argc,argv);
	sqlrpth=new sqlrpaths(cmdl);
	pidfile=NULL;

	// get the scaninterval
	const char	*scanint=cmdl->getValue("-scaninterval");
	if (!charstring::isNullOrEmpty(scanint)) {
		scaninterval=charstring::toInteger(scanint);
	} else {
		scaninterval=DEFAULT_INTERVAL;
	}

	// get the directories to scan
	const char	*cachedirs=cmdl->getValue("-cachedirs");
	parseCacheDirs(cachedirs);
}

sqlrcachemanager::~sqlrcachemanager() {

	// delete the list of dirnames
	currentdir=firstdir;
	while (currentdir) {
		firstdir=currentdir;
		currentdir=currentdir->next;
		delete firstdir;
	}

	delete cmdl;
	delete sqlrpth;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}
}

void sqlrcachemanager::scan() {

	// detach from the controlling tty
	process::detach();

	// create pid file
	pid_t	pid=process::getProcessId();
	charstring::printf(&pidfile,"%ssqlr-cachemanager.%ld.pid",
					sqlrpth->getPidDir(),(long)pid);

	process::createPidFile(pidfile,permissions::ownerReadWrite());

	// scan...
	directory	dir;

	for (;;) {

		// start with the first dir in the list
		currentdir=firstdir;

		while (currentdir) {

			// open directory
			if (dir.open(currentdir->dirname)) {

				// loop through directory, erasing
				for (;;) {
					char	*name=dir.read();
					if (!name) {
						break;
					}
					if (charstring::compare(
							name,".") &&
						charstring::compare(
							name,"..")) {
						erase(currentdir->dirname,name);
					}
					delete[] name;
				}

				// close the directory
				dir.close();
			}

			// move to the next dir in the list
			currentdir=currentdir->next;
		}

		// wait...
		snooze::macrosnooze(scaninterval);
	}

}

void sqlrcachemanager::erase(const char *dirname, const char *filename) {

	// derive the full pathname
	char	*fullpathname=NULL;
	charstring::printf(&fullpathname,"%s/%s",dirname,filename);

	// open the file
	file	fl;
	if (fl.open(fullpathname,O_RDONLY)) {

		// get the "magic" identifier
		char	magicid[13];
		fl.read(magicid,13);
		if (!charstring::compare(magicid,"SQLRELAYCACHE",13)) {

			// get the ttl
			int64_t ttl;
			fl.read(&ttl);
	
			fl.close();
	
			// delete the file if the ttl has expired
			datetime	dt;
			dt.getSystemDateAndTime();
			if (ttl<dt.getEpoch()) {
				file::remove(fullpathname);
			}
		} else {
			fl.close();
		}
	}
	delete[] fullpathname;
}

void sqlrcachemanager::parseCacheDirs(const char *cachedirs) {

	if (!charstring::isNullOrEmpty(cachedirs)) {

		// parse the colon delimited cachedirs string, 
		// create a new dirnode for each directory
		char	*ptr1=(char *)cachedirs;
		char	*ptr2=(char *)cachedirs;
		firstdir=NULL;
		for (;;) {
			if (*ptr2==':' || !(*ptr2)) {
				if (firstdir) {
					currentdir->next=new 
						dirnode(ptr1,ptr2);
					currentdir=currentdir->next;
				} else {
					firstdir=new dirnode(ptr1,ptr2);
					currentdir=firstdir;
				}
				if (*ptr2==':') {
					ptr1=ptr2+1;
				}
			}
			if (!(*ptr2)) {
				return;
			}
			ptr2++;
		}

	} else {

		// in the event that there is no dirnames 
		// string then use the default cache directory
		firstdir=new dirnode(sqlrpth->getCacheDir());
		currentdir=firstdir;
	}
}

sqlrcachemanager	*cacheman;

static void shutDown(int32_t signum) {
	delete cacheman;
	process::exit(0);
}

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s client-side result-set cache manager.\n"
		"\n"
		"To improve the performance of frequently run queries, %s client applications can cache result sets locally and then access them rather than running the query again.  When a result set is cached, a time-to-live (expiration date) is assigned to it. The %s goes through the cached result sets periodically and removes the ones that have expired.\n"
		"\n"
		"Only one %s needs to be run per machine.\n"
		"\n"
		"IMPORTANT NOTE:\n"
		"\n"
		"Since cache managers clean up after %s client applications, they need to be run on machines which run client applications that could cache result sets.  These are likely to be web or application servers, rather than the machines that run the sqlr-listener and sqlr-connection daemons.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		"	-cachedirs dirs		Colon-delimited list of directories to scan\n"
		"				for cache files.\n"
		"\n"
		"	-scaninterval sec	Interval, in seconds, to scan the cache\n"
		"				directories.\n"
		"\n",
		progname,SQL_RELAY,SQL_RELAY,progname,
		progname,SQL_RELAY,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	process::exitOnCrashOrShutDown();

	cacheman=new sqlrcachemanager(argc,argv);
	process::handleShutDown(shutDown);
	cacheman->scan();
}
