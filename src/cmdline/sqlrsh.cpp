// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/filesystem.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/process.h>
#include <rudiments/environment.h>
#include <rudiments/datetime.h>
#include <rudiments/signalclasses.h>
#include <rudiments/xmldom.h>
#include <rudiments/stdio.h>
#include <rudiments/character.h>
#include <rudiments/memorypool.h>
#include <config.h>
#include <defaults.h>
#include <defines.h>
#include <version.h>

#ifdef HAVE_READLINE
	#include <rudiments/charstring.h>
	// This is an interesting story...
	// readline 2's include files don't list any parameters for any of
	// the functions.  This is fine for C, but not C++, at least not with
	// the compiler I'm using, even with the extern "C" {} bit.
	// This is fixed in readline 4, but, to maintain compatibility with 
	// readline 2, I define the functions myself.
	extern "C" {
		extern char *readline(char *prompt);
		extern void add_history(char *line);
		extern void read_history(char *file);
		extern void write_history(char *file);
		extern void history_truncate_file(char *file, int line);
	}
#endif

class sqlrshbindvalue {
	public:
		union {
			char	*stringval;
			int64_t	integerval;
			struct {
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
			struct {
				int16_t		year;
				int16_t		month;
				int16_t		day;
				int16_t		hour;
				int16_t		minute;
				int16_t		second;
				int32_t		microsecond;
				const char	*tz;
			} dateval;
		};
		bindvartype_t	type;
		uint32_t	outputstringbindlength;
};

enum sqlrshformat {
	SQLRSH_FORMAT_PLAIN=0,
	SQLRSH_FORMAT_CSV
};

class sqlrshenv {
	public:
			sqlrshenv();
			~sqlrshenv();
		void	 clearbinds(
			dictionary<char *, sqlrshbindvalue *> *binds);

		bool		headers;
		bool		stats;
		uint64_t	rsbs;
		bool		final;
		bool		autocommit;
		char		delimiter;
		dictionary<char *, sqlrshbindvalue *>	inputbinds;
		memorypool	*inbindpool;
		dictionary<char *, sqlrshbindvalue *>	outputbinds;
		char		*cacheto;
		sqlrshformat	format;
};

sqlrshenv::sqlrshenv() {
	headers=true;
	stats=true;
	rsbs=100;
	final=false;
	autocommit=false;
	delimiter=';';
	inbindpool=new memorypool(512,128,100);
	cacheto=NULL;
	format=SQLRSH_FORMAT_PLAIN;
}

sqlrshenv::~sqlrshenv() {
	clearbinds(&inputbinds);
	clearbinds(&outputbinds);
	delete inbindpool;
	delete[] cacheto;
}

void sqlrshenv::clearbinds(dictionary<char *, sqlrshbindvalue *> *binds) {

	for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
					*node=binds->getList()->getFirst();
		node; node=node->getNext()) {

		delete[] node->getValue()->getKey();
		sqlrshbindvalue	*bv=node->getValue()->getValue();
		if (bv->type==BINDVARTYPE_STRING) {
			delete[] bv->stringval;
		}
		delete bv;
	}
	binds->clear();
	inbindpool->deallocate();
}

enum querytype_t {
	SHOW_DATABASES_QUERY=0,
	SHOW_TABLES_QUERY,
	SHOW_COLUMNS_QUERY,
	DESCRIBE_QUERY
};

class	sqlrsh {
	public:
			sqlrsh();
			~sqlrsh();
		void	execute(int argc, const char **argv);
	private:
		void	startupMessage(sqlrshenv *env,
					const char *host, uint16_t port,
					const char *user);
		void	systemRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env);
		void	userRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env);
		void	runScript(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env, 
					const char *filename, bool returnerror);
		bool	runCommands(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env, 
					const char *commands);
		bool	getCommandFromFileOrString(file *fl,
					const char *string,
					const char **stringpos,
					stringbuffer *cmdbuffer,
					sqlrshenv *env);
		bool	runCommand(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env, const char *command);
		int	commandType(const char *command);
		void	internalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env,
					const char *command);
		void	externalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env, 
					const char *command);
		void	executeQuery(sqlrcursor *sqlrcur, sqlrshenv *env);
		char	*getWild(const char *command);
		char	*getTable(enum querytype_t querytype,
					const char *command);
		void	initStats(sqlrshenv *env);
		void	displayError(sqlrshenv *env,
					const char *message,
					const char *error,
					int64_t errornumber);
		void	displayHeader(sqlrcursor *sqlrcur, sqlrshenv *env);
		void	displayResultSet(sqlrcursor *sqlrcur, sqlrshenv *env);
		void	displayStats(sqlrcursor *sqlrcur, sqlrshenv *env);
		void	ping(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	identify(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	dbversion(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	dbhostname(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	dbipaddress(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	clientversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		void	serverversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	lastinsertid(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	inputbind(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		void	inputbindblob(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		void	outputbind(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		void	printbinds(const char *type,
				dictionary<char *, sqlrshbindvalue *> *binds);
		void	clearbinds(
				dictionary<char *, sqlrshbindvalue *> *binds);
		void	setclientinfo(sqlrconnection *sqlrcon,
						const char *command);
		void	getclientinfo(sqlrconnection *sqlrcon);
		void	responseTimeout(sqlrconnection *sqlrcon,
						const char *command);
		void	cache(sqlrshenv *env, sqlrcursor *sqlrcur,
							const char *command);
		void	openCache(sqlrshenv *env, sqlrcursor *sqlrcur,
							const char *command);
		void	displayHelp(sqlrshenv *env);
		void	interactWithUser(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env);
		void	prompt(unsigned long promptcount);

		sqlrcmdline	*cmdline;
		sqlrpaths	*sqlrpth;

		datetime	start;
};

sqlrsh::sqlrsh() {

	cmdline=NULL;
	sqlrpth=NULL;
}

sqlrsh::~sqlrsh() {
	delete cmdline;
	delete sqlrpth;
}

void sqlrsh::systemRcFile(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
						sqlrshenv *env) {
	runScript(sqlrcon,sqlrcur,env,SYSTEM_SQLRSHRC,false);
}

void sqlrsh::userRcFile(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
						sqlrshenv *env) {

	// get user's home directory
	const char	*home=environment::getValue("HOME");
	if (!home) {
		home="~";
	}

	// build rcfilename
	size_t	userrcfilelen=charstring::length(home)+10+1;
	char	*userrcfile=new char[userrcfilelen];
	charstring::copy(userrcfile,home);
	charstring::append(userrcfile,"/.sqlrshrc");

	// process the file
	runScript(sqlrcon,sqlrcur,env,userrcfile,false);
	delete[] userrcfile;
}

void sqlrsh::runScript(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
			sqlrshenv *env, const char *filename,
			bool returnerror) {

	char	*trimmedfilename=charstring::duplicate(filename);
	charstring::bothTrim(trimmedfilename);

	// open the file
	file	scriptfile;
	if (scriptfile.open(trimmedfilename,O_RDONLY)) {

		// optimize
		filesystem	fs;
		if (fs.initialize(trimmedfilename)) {
			scriptfile.setReadBufferSize(
				fs.getOptimumTransferBlockSize());
		}

		for (;;) {
		
			// get a command
			stringbuffer	command;
			if (!getCommandFromFileOrString(
					&scriptfile,NULL,NULL,&command,env)) {
				break;
			}

			// run the command
			if (!runCommand(sqlrcon,sqlrcur,env,
						command.getString())) {
				break;
			}
		}

		// close the file
		scriptfile.close();
	} else {

		// error message
		if (returnerror) {
			stdoutput.printf("Couldn't open file: %s\n\n",
							trimmedfilename);
		}
	}

	delete[] trimmedfilename;
}

bool sqlrsh::runCommands(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
					sqlrshenv *env, const char *commands) {
	const char	*nextcommand=commands;
	for (;;) {
		stringbuffer	command;
		if (!getCommandFromFileOrString(
				NULL,nextcommand,&nextcommand,&command,env)) {
			break;
		}
		if (!runCommand(sqlrcon,sqlrcur,env,command.getString())) {
			return false;
		}
	}
	return true;
}

bool sqlrsh::getCommandFromFileOrString(file *fl,
					const char *string,
					const char **stringpos,
					stringbuffer *cmdbuffer,
					sqlrshenv *env) {

	bool	ininitialwhitespace=true;
	bool	insinglequotes=false;
	bool	indoublequotes=false;
	char	ch;
	
	for (;;) {

		// get a character from the file or string
		if (fl) {
			if (fl->read(&ch)!=sizeof(ch)) {
				// end of the command...
				// only return false if we're at the
				// beginning, prior to any actual command
				return !ininitialwhitespace;
			}
		} else {
			if (!*string) {
				// end of the command...
				// only return false if we're at the
				// beginning, prior to any actual command
				if (stringpos) {
					*stringpos=string;
				}
				return !ininitialwhitespace;
			}
			ch=*string;
			string++;
		}

		// skip whitespace at the beginning
		if (ininitialwhitespace) {
			if (character::isWhitespace(ch)) {
				continue;
			}
			ininitialwhitespace=false;
		}

		// handle single-quoted strings, with escaping
		if (ch=='\'') {
			if (insinglequotes) {
				cmdbuffer->append(ch);
				if (fl) {
					if (fl->read(&ch)!=sizeof(ch)) {
						return true;
					}
				} else {
					ch=*string;
					string++;
				}
				if (ch!='\'') {
					insinglequotes=false;
				}
			} else {
				insinglequotes=true;
			}
		}

		// handle double-quoted strings, with escaping
		if (ch=='"') {
			if (indoublequotes) {
				cmdbuffer->append(ch);
				if (fl) {
					if (fl->read(&ch)!=sizeof(ch)) {
						return true;
					}
				} else {
					ch=*string;
					string++;
				}
				if (ch!='"') {
					indoublequotes=false;
				}
			} else {
				indoublequotes=true;
			}
		}

		// look for an end of command delimiter
		if (!insinglequotes && !indoublequotes && ch==env->delimiter) {
			if (string && stringpos) {
				*stringpos=string;
			}
			return true;
		}

		// write character to buffer and move on
		cmdbuffer->append(ch);
	}
}

bool sqlrsh::runCommand(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
					sqlrshenv *env, const char *command) {

	int	cmdtype=commandType(command);

	// init stats
	initStats(env);

	if (cmdtype>0) {
		// if the command an internal command, run it as one
		internalCommand(sqlrcon,sqlrcur,env,command);
		return true;
	} else if (cmdtype==0) {
		// if the command is not an internal command, 
		// execute it as a query and display the result set
		externalCommand(sqlrcon,sqlrcur,env,command);
		return true;
	} else {
		// exit
		return false;
	}
}

int sqlrsh::commandType(const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	if (!charstring::compareIgnoringCase(ptr,"headers",7) ||
		!charstring::compareIgnoringCase(ptr,"stats",5) ||
		!charstring::compareIgnoringCase(ptr,"format",6) ||
		!charstring::compareIgnoringCase(ptr,"debug",5) ||
		!charstring::compareIgnoringCase(ptr,"nullsasnulls",12) ||
		!charstring::compareIgnoringCase(ptr,"autocommit",10) ||
		!charstring::compareIgnoringCase(ptr,"final",5) ||
		!charstring::compareIgnoringCase(ptr,"help") ||
		!charstring::compareIgnoringCase(ptr,"ping") ||
		!charstring::compareIgnoringCase(ptr,"identify") ||
		!charstring::compareIgnoringCase(ptr,"dbversion") ||
		!charstring::compareIgnoringCase(ptr,"dbhostname") ||
		!charstring::compareIgnoringCase(ptr,"dbipaddress") ||
		!charstring::compareIgnoringCase(ptr,"clientversion") ||
		!charstring::compareIgnoringCase(ptr,"serverversion") ||
		!charstring::compareIgnoringCase(ptr,"use ",4) ||
		!charstring::compareIgnoringCase(ptr,"currentdb") ||
		!charstring::compareIgnoringCase(ptr,"run",3) ||
		!charstring::compareIgnoringCase(ptr,"@",1) ||
		!charstring::compareIgnoringCase(ptr,"delimiter",9) ||
		!charstring::compareIgnoringCase(ptr,"delimeter",9) ||
		!charstring::compareIgnoringCase(ptr,"inputbind ",10) ||
		!charstring::compareIgnoringCase(ptr,"inputbindblob ",14) ||
		!charstring::compareIgnoringCase(ptr,"outputbind ",11) ||
		!charstring::compareIgnoringCase(ptr,"printinputbind",14) ||
		!charstring::compareIgnoringCase(ptr,"printoutputbind",15) ||
		!charstring::compareIgnoringCase(ptr,"printbinds") ||
		!charstring::compareIgnoringCase(ptr,"clearinputbind",14) ||
		!charstring::compareIgnoringCase(ptr,"clearoutputbind",15) ||
		!charstring::compareIgnoringCase(ptr,"clearbinds") ||
		!charstring::compareIgnoringCase(ptr,"lastinsertid") ||
		!charstring::compareIgnoringCase(ptr,"setclientinfo ",14) ||
		!charstring::compareIgnoringCase(ptr,"getclientinfo") ||
		!charstring::compareIgnoringCase(ptr,
					"setresultsetbuffersize ",23) ||
		!charstring::compareIgnoringCase(ptr,
					"getresultsetbuffersize") ||
		!charstring::compareIgnoringCase(ptr,"endsession") ||
		!charstring::compareIgnoringCase(ptr,"querytree") ||
		!charstring::compareIgnoringCase(ptr,"response timeout",16) ||
		!charstring::compareIgnoringCase(ptr,"cache ",6) ||
		!charstring::compareIgnoringCase(ptr,"opencache ",10)) {

		// return value of 1 is internal command
		return 1;
	}

	// look for an exit command
	if (!charstring::compareIgnoringCase(ptr,"quit",4) ||
		!charstring::compareIgnoringCase(ptr,"exit",4)) {
		return -1;
	}

	// return value of 0 is external command
	return 0;
}

void sqlrsh::internalCommand(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
					sqlrshenv *env, const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	int	cmdtype=0;
	if (!charstring::compareIgnoringCase(ptr,"headers",7)) {
		ptr=ptr+7;
		cmdtype=2;
	} else if (!charstring::compareIgnoringCase(ptr,"stats",5)) {	
		ptr=ptr+5;
		cmdtype=3;
	} else if (!charstring::compareIgnoringCase(ptr,"format",6)) {	
		ptr=ptr+6;
		cmdtype=10;
	} else if (!charstring::compareIgnoringCase(ptr,"debug",5)) {	
		ptr=ptr+5;
		cmdtype=4;
	} else if (!charstring::compareIgnoringCase(ptr,"nullsasnulls",12)) {	
		ptr=ptr+13;
		cmdtype=9;
	} else if (!charstring::compareIgnoringCase(ptr,"autocommit",10)) {	
		ptr=ptr+10;
		cmdtype=8;
	} else if (!charstring::compareIgnoringCase(ptr,"final",5)) {	
		ptr=ptr+5;
		cmdtype=5;
	} else if (!charstring::compareIgnoringCase(ptr,"help")) {	
		displayHelp(env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"ping")) {	
		ping(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"use ",4)) {	
		if (!sqlrcon->selectDatabase(ptr+4)) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"currentdb")) {	
		const char	*currentdb=sqlrcon->getCurrentDatabase();
		if (currentdb) {
			stdoutput.printf("%s\n",currentdb);
		} else if (sqlrcon->errorMessage()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		} else {
			stdoutput.printf("\n");
		}
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"run",3)) {	
		ptr=ptr+3;
		cmdtype=6;
	} else if (!charstring::compareIgnoringCase(ptr,"@",1)) {	
		ptr=ptr+1;
		cmdtype=6;
	} else if (!charstring::compareIgnoringCase(ptr,"delimiter",9) ||
			!charstring::compareIgnoringCase(ptr,"delimeter",9)) {	
		ptr=ptr+9;
		cmdtype=7;
	} else if (!charstring::compareIgnoringCase(ptr,"identify")) {	
		identify(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"dbversion")) {	
		dbversion(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"dbhostname")) {	
		dbhostname(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"dbipaddress")) {	
		dbipaddress(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"clientversion")) {	
		clientversion(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"serverversion")) {	
		serverversion(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"inputbind ",10)) {	
		inputbind(sqlrcur,env,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"inputbindblob ",14)) {	
		inputbindblob(sqlrcur,env,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"outputbind ",11)) {	
		outputbind(sqlrcur,env,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"printbinds")) {	
		printbinds("Input",&env->inputbinds);
		stdoutput.printf("\n");
		printbinds("Output",&env->outputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"clearinputbind",14)) {	
		env->clearbinds(&env->inputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(
					ptr,"clearoutputbind",15)) {	
		env->clearbinds(&env->outputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"clearbinds")) {	
		env->clearbinds(&env->inputbinds);
		env->clearbinds(&env->outputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"lastinsertid")) {	
		if (!lastinsertid(sqlrcon,env)) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"setclientinfo ",14)) {	
		setclientinfo(sqlrcon,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"getclientinfo")) {	
		getclientinfo(sqlrcon);
		return;
	} else if (!charstring::compareIgnoringCase(
					ptr,"setresultsetbuffersize ",23)) {	
		ptr=ptr+23;
		env->rsbs=charstring::toInteger(ptr);
		if (!env->rsbs) {
			env->rsbs=100;
		}
		return;
	} else if (!charstring::compareIgnoringCase(
					ptr,"getresultsetbuffersize")) {	
		stdoutput.printf("%lld\n",(long long)env->rsbs);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"endsession")) {	
		sqlrcon->endSession();
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"querytree")) {	
		xmldom	xmld;
		if (xmld.parseString(sqlrcur->getQueryTree())) {
			xmld.getRootNode()->print(&stdoutput);
		}
		return;
	} else if (!charstring::compareIgnoringCase(
					ptr,"response timeout",16)) {
		responseTimeout(sqlrcon,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"cache ",6)) {
		cache(env,sqlrcur,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"opencache ",10)) {
		openCache(env,sqlrcur,command);
		return;
	} else {
		return;
	}

	// skip white space
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// handle scripts
	if (cmdtype==6) {
		runScript(sqlrcon,sqlrcur,env,ptr,true);
		return;
	}

	// handle debug
	if (cmdtype==4) {
		if (!charstring::compareIgnoringCase(ptr,"on",2)) {
			sqlrcon->debugOn();
			sqlrcon->setDebugFile(NULL);
		} else if (!charstring::compareIgnoringCase(ptr,"off",3)) {
			sqlrcon->debugOff();
			sqlrcon->setDebugFile(NULL);
		} else {
			sqlrcon->debugOn();
			sqlrcon->setDebugFile(ptr);
		}
		return;
	}

	// handle nullsasnulls
	if (cmdtype==9) {
		if (!charstring::compareIgnoringCase(ptr,"on",2)) {
			sqlrcur->getNullsAsNulls();
		} else if (!charstring::compareIgnoringCase(ptr,"off",3)) {
			sqlrcur->getNullsAsEmptyStrings();
		}
		return;
	}

	// handle format
	if (cmdtype==10) {
		if (!charstring::compareIgnoringCase(ptr,"csv",3)) {
			env->format=SQLRSH_FORMAT_CSV;
		} else {
			env->format=SQLRSH_FORMAT_PLAIN;
		}
		return;
	}

	// on or off?
	bool	toggle=false;
	if (!charstring::compareIgnoringCase(ptr,"on",2)) {
		toggle=true;
	}

	// set parameter
	if (cmdtype==2) {
		env->headers=toggle;
	} else if (cmdtype==3) {
		env->stats=toggle;
	} else if (cmdtype==5) {
		env->final=toggle;
	} else if (cmdtype==7) {
		env->delimiter=ptr[0];
		stdoutput.printf("Delimiter set to %c\n",env->delimiter);
	} else if (cmdtype==8) {
		if (toggle) {
			if (sqlrcon->autoCommitOn()) {
				stdoutput.printf("Autocommit set on\n");
			} else {
				displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			}
		} else {
			if (sqlrcon->autoCommitOff()) {
				stdoutput.printf("Autocommit set off\n");
			} else {
				displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			}
		}
	}
}

void sqlrsh::externalCommand(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur, sqlrshenv *env, 
				const char *command) {

	// handle begin, commit and rollback
	if (!charstring::compareIgnoringCase(command,"begin")) {

		if (!sqlrcon->begin()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}

	} else if (!charstring::compareIgnoringCase(command,"commit")) {

		if (!sqlrcon->commit()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}

	} else if (!charstring::compareIgnoringCase(command,"rollback")) {

		if (!sqlrcon->rollback()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}

	} else if (!charstring::compareIgnoringCase(command,"fields ",7)) {

		char	*table=getTable(DESCRIBE_QUERY,command);
		sqlrcur->getColumnList(table,NULL);
		delete[] table;

		for (uint64_t j=0; j<sqlrcur->rowCount(); j++) {
			if (j>0) {
				stdoutput.printf(",");
			}
			stdoutput.printf("%s",sqlrcur->getField(j,(uint32_t)0));
		}
		stdoutput.printf("\n");

		if (env->final) {
			sqlrcon->endSession();
		}

	} else {

		sqlrcur->setResultSetBufferSize(env->rsbs);

		// send the query
		if (!charstring::compareIgnoringCase(command,
						"show databases odbc",19)) {
			char	*wild=getWild(command);
			sqlrcur->getDatabaseList(wild,
					SQLRCLIENTLISTFORMAT_ODBC);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show databases",14)) {
			char	*wild=getWild(command);
			sqlrcur->getDatabaseList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show tables odbc",16)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_ODBC);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show tables",11)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show columns odbc",17)) {
			char	*table=getTable(SHOW_COLUMNS_QUERY,command);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild,
					SQLRCLIENTLISTFORMAT_ODBC);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show columns",12)) {
			char	*table=getTable(SHOW_COLUMNS_QUERY,command);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"describe ",9)) {
			char	*table=getTable(DESCRIBE_QUERY,command);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"reexecute")) {	
			executeQuery(sqlrcur,env);
		} else {
			sqlrcur->prepareQuery(command);
			executeQuery(sqlrcur,env);
		}

		// look for an error
		if (sqlrcur->errorMessage()) {

			// display the error
			displayError(env,NULL,
					sqlrcur->errorMessage(),
					sqlrcur->errorNumber());

		} else {

			// display the header
			displayHeader(sqlrcur,env);

			// display the result set
			displayResultSet(sqlrcur,env);
		}

		if (env->final) {
			sqlrcon->endSession();
		}
	}

	// display statistics
	displayStats(sqlrcur,env);
}

void sqlrsh::executeQuery(sqlrcursor *sqlrcur, sqlrshenv *env) {

	sqlrcur->clearBinds();

	if (env->inputbinds.getList()->getLength()) {

		for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
				*node=env->inputbinds.getList()->getFirst();
				node; node=node->getNext()) {

			const char	*name=node->getValue()->getKey();
			sqlrshbindvalue	*bv=node->getValue()->getValue();
			if (bv->type==BINDVARTYPE_STRING) {
				sqlrcur->inputBind(name,bv->stringval);
			} else if (bv->type==BINDVARTYPE_INTEGER) {
				sqlrcur->inputBind(name,bv->integerval);
			} else if (bv->type==BINDVARTYPE_DOUBLE) {
				sqlrcur->inputBind(name,bv->doubleval.value,
							bv->doubleval.precision,
							bv->doubleval.scale);
			} else if (bv->type==BINDVARTYPE_DATE) {
				sqlrcur->inputBind(name,
						bv->dateval.year,
						bv->dateval.month,
						bv->dateval.day,
						bv->dateval.hour,
						bv->dateval.minute,
						bv->dateval.second,
						bv->dateval.microsecond,
						bv->dateval.tz);
			} else if (bv->type==BINDVARTYPE_BLOB) {
				sqlrcur->inputBindBlob(name,bv->stringval,
					charstring::length(bv->stringval));
			} else if (bv->type==BINDVARTYPE_NULL) {
				sqlrcur->inputBind(name,(const char *)NULL);
			}
		}
	}

	if (env->outputbinds.getList()->getLength()) {

		for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
			*node=env->outputbinds.getList()->getFirst();
			node; node=node->getNext()) {

			const char	*name=node->getValue()->getKey();
			sqlrshbindvalue	*bv=node->getValue()->getValue();
			if (bv->type==BINDVARTYPE_STRING) {
				// FIXME: make buffer length variable
				sqlrcur->defineOutputBindString(name,
						bv->outputstringbindlength);
			} else if (bv->type==BINDVARTYPE_INTEGER) {
				sqlrcur->defineOutputBindInteger(name);
			} else if (bv->type==BINDVARTYPE_DOUBLE) {
				sqlrcur->defineOutputBindDouble(name);
			} else if (bv->type==BINDVARTYPE_DATE) {
				sqlrcur->defineOutputBindDate(name);
			}
		}
	}

	sqlrcur->executeQuery();

	if (env->outputbinds.getList()->getLength()) {

		for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
			*node=env->outputbinds.getList()->getFirst();
			node; node=node->getNext()) {

			const char	*name=node->getValue()->getKey();
			sqlrshbindvalue	*bv=node->getValue()->getValue();
			if (bv->type==BINDVARTYPE_STRING) {
				delete[] bv->stringval;
				bv->stringval=charstring::duplicate(
					sqlrcur->getOutputBindString(name));
			} else if (bv->type==BINDVARTYPE_INTEGER) {
				bv->integerval=
					sqlrcur->getOutputBindInteger(name);
			} else if (bv->type==BINDVARTYPE_DOUBLE) {
				bv->doubleval.value=
					sqlrcur->getOutputBindDouble(name);
			} else if (bv->type==BINDVARTYPE_DATE) {
				sqlrcur->getOutputBindDate(name,
						&(bv->dateval.year),
						&(bv->dateval.month),
						&(bv->dateval.day),
						&(bv->dateval.hour),
						&(bv->dateval.minute),
						&(bv->dateval.second),
						&(bv->dateval.microsecond),
						&(bv->dateval.tz));
			}
		}
	}
}

char *sqlrsh::getWild(const char *command) {
	const char	*wildptr=charstring::findFirst(command,"'");
	if (!wildptr) {
		return NULL;
	}
	wildptr++;
	const char	*endptr=charstring::findLast(wildptr,"'");
	if (!endptr) {
		return NULL;
	}

	// unescape single quotes
	stringbuffer	output;
	for (const char *ch=wildptr; ch<endptr; ch++) {
		if (*ch=='\'' && *(ch+1)=='\'') {
			ch++;
		}
		output.append(*ch);
	}

	return output.detachString();
}

char *sqlrsh::getTable(querytype_t querytype, const char *command) {
	const char	*tableptr=NULL;
	if (querytype==SHOW_COLUMNS_QUERY) {
		tableptr=charstring::findFirst(command," in ");
		if (!tableptr) {
			return NULL;
		}
		tableptr=tableptr+4;
		const char	*endptr=charstring::findFirst(tableptr," ");
		if (!endptr) {
			return charstring::duplicate(tableptr);
		}
		return charstring::duplicate(tableptr,endptr-tableptr);
	} else if (querytype==DESCRIBE_QUERY) {
		tableptr=charstring::findFirst(command," ");
		if (!tableptr) {
			return NULL;
		}
		return charstring::duplicate(tableptr+1);
	}
	return NULL;
}

void sqlrsh::initStats(sqlrshenv *env) {

	if (!env->stats) {
		return;
	}

	start.getSystemDateAndTime();
}

void sqlrsh::displayError(sqlrshenv *env,
				const char *message,
				const char *error,
				int64_t errornumber) {
	if (!charstring::isNullOrEmpty(message)) {
		stdoutput.printf("%s\n",message);
	}
	stdoutput.printf("%lld:\n",(long long)errornumber);
	if (!charstring::isNullOrEmpty(error)) {
		stdoutput.printf("%s\n\n",error);
	}
}

void sqlrsh::displayHeader(sqlrcursor *sqlrcur, sqlrshenv *env) {

	if (!env->headers) {
		return;
	}

	// display column names
	uint32_t	charcount=0;
	uint32_t	colcount=sqlrcur->colCount();
	const char	*name;
	uint32_t	namelen;
	uint32_t	longest;

	if (!colcount) {
		return;
	}

	// iterate through columns
	for (uint32_t ci=0; ci<sqlrcur->colCount(); ci++) {

		// put a comma or extra space between field names
		if (ci) {
			if (env->format==SQLRSH_FORMAT_CSV) {
				stdoutput.write(',');
			} else {
				stdoutput.write(' ');
			}
			charcount=charcount+1;
		}

		// write the column name
		if (env->format==SQLRSH_FORMAT_CSV) {
			stdoutput.write('\"');
		}
		name=sqlrcur->getColumnName(ci);
		stdoutput.write(name);
		if (env->format==SQLRSH_FORMAT_CSV) {
			stdoutput.write('\"');
		}
		namelen=charstring::length(name);

		// space-pad after the name, if necessary
		if (env->format==SQLRSH_FORMAT_PLAIN) {
			longest=sqlrcur->getLongest(ci);
			if (namelen>longest) {
				longest=namelen;
			}
			charcount=charcount+longest;

			// pad after the name with spaces
			for (uint32_t j=namelen; j<longest; j++) {
				stdoutput.write(' ');
			}
		} else {
			charcount=charcount+namelen+2;
		}
	}
	stdoutput.printf("\n");

	// display delimiter
	for (uint32_t i=0; i<charcount; i++) {
		stdoutput.printf("=");
	}
	stdoutput.printf("\n");
}

void sqlrsh::displayResultSet(sqlrcursor *sqlrcur, sqlrshenv *env) {

	uint32_t	colcount=sqlrcur->colCount();
	if (!colcount) {
		return;
	}

	uint32_t	namelen;
	uint32_t	longest;
	const char	*field;
	uint32_t	fieldlength;

	uint32_t	i=0;
	while (!(sqlrcur->endOfResultSet() && i==sqlrcur->rowCount())) {
		for (uint32_t j=0; j<colcount; j++) {

			// put a comma or extra space between fields
			if (j) {
				if (env->format==SQLRSH_FORMAT_CSV) {
					stdoutput.write(',');
				} else {
					stdoutput.write(' ');
				}
			}

			// get the field
			field=sqlrcur->getField(i,j);
			fieldlength=sqlrcur->getFieldLength(i,j);
			if (!field) {
				field="NULL";
				fieldlength=4;
			}

			// write the field
			if (env->format==SQLRSH_FORMAT_CSV) {
				stdoutput.write('\"');
			}
			stdoutput.write(field);
			if (env->format==SQLRSH_FORMAT_CSV) {
				stdoutput.write('\"');
			}

			// space-pad after the field, if necessary
			if (env->format==SQLRSH_FORMAT_PLAIN) {
				longest=sqlrcur->getLongest(j);
				if (env->headers) {
					namelen=charstring::length(
						sqlrcur->getColumnName(j));
					if (namelen>longest) {
						longest=namelen;
					}
				}
				for (uint32_t k=fieldlength; k<longest; k++) {
					stdoutput.write(' ');
				}
			}
		}
		stdoutput.write('\n');
		i++;
	}
}

void sqlrsh::displayStats(sqlrcursor *sqlrcur, sqlrshenv *env) {

	if (!env->stats) {
		return;
	}

	// calculate elapsed time
	datetime	end;
	end.getSystemDateAndTime();
	uint64_t	startusec=start.getEpoch()*1000000+
					start.getMicroseconds();
	uint64_t	endusec=end.getEpoch()*1000000+
					end.getMicroseconds();
	double		time=((double)(endusec-startusec))/1000000;

	// display stats
	stdoutput.printf("	Rows Returned   : ");
	stdoutput.printf("%lld\n",(long long)sqlrcur->rowCount());
	stdoutput.printf("	Fields Returned : ");
	stdoutput.printf("%lld\n",
			(long long)sqlrcur->rowCount()*sqlrcur->colCount());
	stdoutput.printf("	Elapsed Time    : ");
	stdoutput.printf("%.6f sec\n",time);
	stdoutput.printf("\n");
}

void sqlrsh::ping(sqlrconnection *sqlrcon, sqlrshenv *env) {
	bool	result=sqlrcon->ping();
	if (result) {
		stdoutput.printf("	The database is up.\n");
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
	} else {
		stdoutput.printf("	The database is down.\n");
	}
}

bool sqlrsh::lastinsertid(sqlrconnection *sqlrcon, sqlrshenv *env) {
	bool		retval=false;
	uint64_t	id=sqlrcon->getLastInsertId();
	if (id!=0 || !sqlrcon->errorMessage()) {
		stdoutput.printf("%lld\n",(long long)id);
		retval=true;
	}
	return retval;
}

void sqlrsh::identify(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->identify();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
	} else {
		stdoutput.printf("\n");
	}
}

void sqlrsh::dbversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->dbVersion();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
	} else {
		stdoutput.printf("\n");
	}
}

void sqlrsh::dbhostname(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->dbHostName();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
	} else {
		stdoutput.printf("\n");
	}
}

void sqlrsh::dbipaddress(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->dbIpAddress();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
	} else {
		stdoutput.printf("\n");
	}
}

void sqlrsh::clientversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	stdoutput.printf("%s\n",sqlrcon->clientVersion());
}

void sqlrsh::serverversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->serverVersion();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
	} else {
		stdoutput.printf("\n");
	}
}

void sqlrsh::inputbind(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// sanity check
	const char	*ptr=command+10;
	const char	*space=charstring::findFirst(ptr,' ');
	if (!space) {
		stdoutput.printf("usage: inputbind [variable] = [value]\n");
		return;
	}

	// get the variable name
	char	*variable=charstring::duplicate(ptr,space-ptr);

	// move on
	ptr=space;
	if (*(ptr+1)=='=' && *(ptr+2)==' ') {
		ptr=ptr+3;
	} else if (!charstring::compareIgnoringCase(ptr+1,"is null")) {
		ptr=NULL;
	} else {
		stdoutput.printf("usage: inputbind [variable] = [value]\n");
		stdoutput.printf("       inputbind [variable] is null\n");
		return;
	}
		
	// get the value
	char	*value=charstring::duplicate(ptr);
	charstring::bothTrim(value);
	size_t	valuelen=charstring::length(value);

	// if the bind variable is already defined, clear it...
	sqlrshbindvalue	*bv=NULL;
	if (env->inputbinds.getValue(variable,&bv)) {
		if (bv->type==BINDVARTYPE_STRING) {
			delete[] bv->stringval;
		}
		delete bv;
	}

	// define the variable
	bv=new sqlrshbindvalue;

	// first handle nulls, then...
	// anything enclosed in quotes is a string
	// if it's unquoted, check to see if it's an integer, float or date
	// if it's not, then it's a string
	if (!value) {
		bv->type=BINDVARTYPE_NULL;
	} else if ((value[0]=='\'' && value[valuelen-1]=='\'') ||
			(value[0]=='"' && value[valuelen-1]=='"')) {

		bv->type=BINDVARTYPE_STRING;

		// trim off quotes
		char	*newvalue=charstring::duplicate(value+1);
		newvalue[valuelen-2]='\0';
		delete[] value;

		// unescape the string
		bv->stringval=charstring::unescape(newvalue);
		delete[] newvalue;

	} else if (charstring::contains(value,"/") && 
			charstring::contains(value,":")) {

		datetime	dt;
		dt.initialize(value);
		bv->type=BINDVARTYPE_DATE;
		bv->dateval.year=dt.getYear();
		bv->dateval.month=dt.getMonth();
		bv->dateval.day=dt.getDayOfMonth();
		bv->dateval.hour=dt.getHour();
		bv->dateval.minute=dt.getMinutes();
		bv->dateval.second=dt.getSeconds();
		bv->dateval.microsecond=charstring::toInteger(
					charstring::findLast(value,":")+1);
		char	*tz=(char *)env->inbindpool->allocate(
				charstring::length(dt.getTimeZoneString())+1);
		charstring::copy(tz,dt.getTimeZoneString());
		bv->dateval.tz=tz;
		delete[] value;

	} else if (charstring::isInteger(value)) {
		bv->type=BINDVARTYPE_INTEGER;
		bv->integerval=charstring::toInteger(value);
		delete[] value;
	} else if (charstring::isNumber(value)) {
		bv->type=BINDVARTYPE_DOUBLE;
		bv->doubleval.value=charstring::toFloat(value);
		bv->doubleval.precision=valuelen-((value[0]=='-')?2:1);
		bv->doubleval.scale=
			charstring::findFirst(value,'.')-value+
			((value[0]=='-')?0:1);
		delete[] value;
	} else {
		bv->type=BINDVARTYPE_STRING;
		bv->stringval=value;
	}

	// put the bind variable in the list
	env->inputbinds.setValue(variable,bv);
}

void sqlrsh::inputbindblob(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// sanity check
	const char	*ptr=command+14;
	const char	*space=charstring::findFirst(ptr,' ');
	if (!space) {
		stdoutput.printf("usage: inputbindblob [variable] = [value]\n");
		return;
	}

	// get the variable name
	char	*variable=charstring::duplicate(ptr,space-ptr);

	// move on
	ptr=space;
	if (*(ptr+1)=='=' && *(ptr+2)==' ') {
		ptr=ptr+3;
	} else if (!charstring::compareIgnoringCase(ptr+1,"is null")) {
		ptr=NULL;
	} else {
		stdoutput.printf("usage: inputbindblob [variable] = [value]\n");
		stdoutput.printf("       inputbindblob [variable] is null\n");
		return;
	}
		
	// get the value
	char	*value=charstring::duplicate(ptr);
	charstring::bothTrim(value);
	size_t	valuelen=charstring::length(value);

	// if the bind variable is already defined, clear it...
	sqlrshbindvalue	*bv=NULL;
	if (env->inputbinds.getValue(variable,&bv)) {
		if (bv->type==BINDVARTYPE_STRING) {
			delete[] bv->stringval;
		}
		delete bv;
	}

	// define the variable
	bv=new sqlrshbindvalue;

	// first handle nulls, then...
	// anything enclosed in quotes is a string
	// if it's unquoted, check to see if it's an integer, float or date
	// if it's not, then it's a string
	if (!value) {
		bv->type=BINDVARTYPE_NULL;
	} else if ((value[0]=='\'' && value[valuelen-1]=='\'') ||
			(value[0]=='"' && value[valuelen-1]=='"')) {

		bv->type=BINDVARTYPE_BLOB;

		// trim off quotes
		char	*newvalue=charstring::duplicate(value+1);
		newvalue[valuelen-2]='\0';
		delete[] value;

		// unescape the string
		bv->stringval=charstring::unescape(newvalue);
		delete[] newvalue;

	} else {
		bv->type=BINDVARTYPE_BLOB;
		bv->stringval=value;
	}

	// put the bind variable in the list
	env->inputbinds.setValue(variable,bv);
}

void sqlrsh::outputbind(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// split the command on ' '
	char		**parts;
	uint64_t	partcount;
	charstring::split(command," ",true,&parts,&partcount);

	// sanity check...
	bool	sane=true;
	if (partcount>2 && !charstring::compare(parts[0],"outputbind")) {

		// if the bind variable is already defined, clear it...
		sqlrshbindvalue	*bv=NULL;
		if (env->outputbinds.getValue(parts[1],&bv)) {
			if (bv->type==BINDVARTYPE_STRING) {
				delete[] bv->stringval;
			}
			delete bv;
		}

		// define the variable
		bv=new sqlrshbindvalue;

		if (!charstring::compareIgnoringCase(
						parts[2],"string") &&
						partcount==4) {
			bv->type=BINDVARTYPE_STRING;
			bv->stringval=NULL;
			bv->outputstringbindlength=
				charstring::toInteger(parts[3]);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"integer") &&
						partcount==3) {
			bv->type=BINDVARTYPE_INTEGER;
			bv->integerval=0;
		} else if (!charstring::compareIgnoringCase(
						parts[2],"double") &&
						partcount==5) {
			bv->type=BINDVARTYPE_DOUBLE;
			bv->doubleval.value=0.0;
			bv->doubleval.precision=
				charstring::toInteger(parts[3]);
			bv->doubleval.scale=
				charstring::toInteger(parts[4]);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"date") &&
						partcount==3) {
			bv->type=BINDVARTYPE_DATE;
			bv->dateval.year=0;
			bv->dateval.month=0;
			bv->dateval.day=0;
			bv->dateval.hour=0;
			bv->dateval.minute=0;
			bv->dateval.second=0;
			bv->dateval.microsecond=0;
			bv->dateval.tz="";
		} else {
			sane=false;
		}

		// put the bind variable in the list
		if (sane) {
			env->outputbinds.setValue(parts[1],bv);
		}

	} else {
		sane=false;
	}

	// clean up
	if (sane) {
		delete[] parts[0];
	} else {
		stdoutput.printf("usage: outputbind "
				"[variable] [type] [length] [scale]\n");
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
	}
	delete[] parts;
}

void sqlrsh::printbinds(const char *type,
			dictionary<char *, sqlrshbindvalue *> *binds) {

	stdoutput.printf("%s bind variables:\n",type);

	for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
					*node=binds->getList()->getFirst();
		node; node=node->getNext()) {

		stdoutput.printf("    %s ",node->getValue()->getKey());
		sqlrshbindvalue	*bv=node->getValue()->getValue();
		if (bv->type==BINDVARTYPE_STRING) {
			stdoutput.printf("(STRING) = %s\n",bv->stringval);
		} else if (bv->type==BINDVARTYPE_INTEGER) {
			stdoutput.printf("(INTEGER) = %lld\n",
						(long long)bv->integerval);
		} else if (bv->type==BINDVARTYPE_DOUBLE) {
			stdoutput.printf("(DOUBLE %d,%d) = %*.*f\n",
						bv->doubleval.precision,
						bv->doubleval.scale,
						(int)bv->doubleval.precision,
						(int)bv->doubleval.scale,
						bv->doubleval.value);
		} else if (bv->type==BINDVARTYPE_DATE) {
			stdoutput.printf("(DATE) = %02d/%02d/%04d "
						"%02d:%02d:%02d:%03d %s\n",
						bv->dateval.month,
						bv->dateval.day,
						bv->dateval.year,
						bv->dateval.hour,
						bv->dateval.minute,
						bv->dateval.second,
						bv->dateval.microsecond,
						bv->dateval.tz);
		} else if (bv->type==BINDVARTYPE_BLOB) {
			stdoutput.printf("(BLOB) = ");
			stdoutput.safePrint(bv->stringval,
					charstring::length(bv->stringval));
			stdoutput.printf("\n");
		} else if (bv->type==BINDVARTYPE_NULL) {
			stdoutput.printf("NULL\n");
		}
	}
}

void sqlrsh::setclientinfo(sqlrconnection *sqlrcon, const char *command) {
	sqlrcon->setClientInfo(command+14);
}

void sqlrsh::getclientinfo(sqlrconnection *sqlrcon) {
	const char	*ci=sqlrcon->getClientInfo();
	stdoutput.printf("%s\n",(ci)?ci:"");
}

void sqlrsh::responseTimeout(sqlrconnection *sqlrcon, const char *command) {

	// skip to timeout itself
	const char	*value=command+16;
	while (character::isWhitespace(*value)) {
		value++;
	}

	// get seconds
	uint32_t	sec=charstring::toInteger(value);

	// get milliseconds
	char	msecbuf[5];
	bytestring::set(msecbuf,'0',4);
	msecbuf[4]='\0';
	const char	*dot=charstring::findFirst(value,'.');
	if (dot) {
		value=dot+1;
		for (uint8_t i=0; i<4 && *value; i++) {
			msecbuf[i]=*value;
			value++;
		}
	}
	uint32_t	msec=charstring::toInteger(msecbuf);

	// set timeout
	sqlrcon->setResponseTimeout(sec,msec);
	stdoutput.printf("Response Timeout set to %d.%04d seconds\n",sec,msec);
}

void sqlrsh::cache(sqlrshenv *env, sqlrcursor *sqlrcur, const char *command) {

	// move to file name
	const char	*ptr=command+6;

	// skip whitespace
	while (*ptr==' ') {
		ptr++;
	}

	// bail if no file name was given
	if (!*ptr) {
		stdoutput.printf("	No file name given\n\n");
		return;
	}

	// build filename
	stringbuffer	fn;
	fn.append(sqlrpth->getCacheDir());
	bool	inquotes=false;
	while (*ptr) {
		if (*ptr=='"') {
			inquotes=!inquotes;
		}
		if (*ptr==' ' && !inquotes) {
			break;
		}
		fn.append(*ptr);
		ptr++;
	}
	delete[] env->cacheto;
	env->cacheto=fn.detachString();

	// find ttl
	while (*ptr==' ') {
		ptr++;
	}
	uint32_t	cachettl=600;
	if (*ptr) {
		cachettl=charstring::toInteger(ptr);
	}

	stdoutput.printf("	Caching To       : %s\n",env->cacheto);
	stdoutput.printf("	Cache TTL Set To : %lld seconds\n\n",cachettl);

	// begin caching
	sqlrcur->cacheToFile(env->cacheto);
	sqlrcur->setCacheTtl(cachettl);
}

void sqlrsh::openCache(sqlrshenv *env,
			sqlrcursor *sqlrcur, const char *command) {

	// move to file name
	command=command+10;

	// skip whitespace
	while (*command==' ') {
		command++;
	}

	// bail if no file name was given
	if (!*command) {
		return;
	}

	// if the file name starts with a slash then use it as-is, otherwise
	// prepend the default cache directory.
	stringbuffer	fn;
	fn.append(sqlrpth->getCacheDir())->append(command);

	// open the cached result set
	sqlrcur->openCachedResultSet(fn.getString());

	// display the header
	displayHeader(sqlrcur,env);

	// display the result set
	displayResultSet(sqlrcur,env);

	// display statistics
	displayStats(sqlrcur,env);
}

void sqlrsh::displayHelp(sqlrshenv *env) {

	stdoutput.printf("\n");
	stdoutput.printf("	To run a query, simply type it at the prompt,\n"
			"	followed by a semicolon.  Queries may be \n"
			"	split over multiple lines.\n\n");
	stdoutput.printf("	ping			- ");
	stdoutput.printf("pings the database\n");
	stdoutput.printf("	identify		- ");
	stdoutput.printf("returns the type of database\n");
	stdoutput.printf("	dbversion		- ");
	stdoutput.printf("returns the version of the database\n");
	stdoutput.printf("	dbhostname		- ");
	stdoutput.printf("returns the host name of the database\n");
	stdoutput.printf("	dbipaddress		- ");
	stdoutput.printf("returns the ip address of the database\n");
	stdoutput.printf("	clientversion		- ");
	stdoutput.printf("returns the version of the SQL Relay\n");
	stdoutput.printf("\t\t\t\t  client library\n");
	stdoutput.printf("	serverversion		- ");
	stdoutput.printf("returns the version of the SQL Relay server\n");
	stdoutput.printf("	use [database]		- ");
	stdoutput.printf("change the current database/schema\n");
	stdoutput.printf("	currentdb		- ");
	stdoutput.printf("shows the current database/schema\n");
	stdoutput.printf("	run script		- ");
	stdoutput.printf("runs commands contained in file \"script\"\n");
	stdoutput.printf("	headers on|off		- ");
	stdoutput.printf("toggles column descriptions before result set\n");
	stdoutput.printf("	stats on|off		- ");
	stdoutput.printf("toggles statistics after result set\n");
	stdoutput.printf("	format plain|csv	- ");
	stdoutput.printf("sets output format to plain or csv\n");
	stdoutput.printf("	debug on|off		- ");
	stdoutput.printf("toggles debug messages\n");
	stdoutput.printf("	nullsasnulls on|off	- ");
	stdoutput.printf("toggles getting nulls as nulls\n"
			"					"
			"(rather than as empty strings)\n");
	stdoutput.printf("	autocommit on|off	- ");
	stdoutput.printf("toggles autocommit\n");
	stdoutput.printf("	final on|off		- ");
	stdoutput.printf("toggles use of one session per query\n");
	stdoutput.printf("	delimiter [character]	- ");
	stdoutput.printf("sets delimiter character to [character]\n\n");
	stdoutput.printf("	response timeout [sec.msec]   - ");
	stdoutput.printf("sets response timeout to [sec.msec]\n\n");
	stdoutput.printf("	inputbind ...                 - ");
	stdoutput.printf("defines an input bind variable\n");
	stdoutput.printf("		inputbind [variable] is null\n");
	stdoutput.printf("		inputbind [variable] = [stringvalue]\n");
	stdoutput.printf("		inputbind [variable] = [integervalue]\n");
	stdoutput.printf("		inputbind [variable] = [doublevalue]\n");
	stdoutput.printf("		inputbind [variable] = [MM/DD/YYYY HH:MM:SS:uS TZN]\n");
	stdoutput.printf("		inputbindblob [variable] = [value]\n");
	stdoutput.printf("	outputbind ...                 - ");
	stdoutput.printf("defines an output bind variable\n");
	stdoutput.printf("		outputbind [variable] string [length]\n");
	stdoutput.printf("		outputbind [variable] integer\n");
	stdoutput.printf("		outputbind [variable] double [precision] [scale}\n");
	stdoutput.printf("		outputbind [variable] date\n");
	stdoutput.printf("	printbinds                     - ");
	stdoutput.printf("prints all bind variables\n");
	stdoutput.printf("	clearinputbind [variable]      - ");
	stdoutput.printf("clears an input bind variable\n");
	stdoutput.printf("	clearoutputbind [variable]     - ");
	stdoutput.printf("clears an output bind variable\n");
	stdoutput.printf("	clearbinds                     - ");
	stdoutput.printf("clears all bind variables\n");
	stdoutput.printf("	reexecute                      - ");
	stdoutput.printf("reexecutes the previous query\n\n");
	stdoutput.printf("	lastinsertid                   - ");
	stdoutput.printf("returns the value of the most recently\n");
	stdoutput.printf("\t\t\t\t\t updated auto-increment or identity\n");
	stdoutput.printf("\t\t\t\t\t column, if the database supports it\n\n");
	stdoutput.printf("	show databases [like pattern]		-\n");
	stdoutput.printf("		returns a list of known databases/schemas\n");
	stdoutput.printf("	show tables [like pattern]		-\n");
	stdoutput.printf("		returns a list of known tables\n");
	stdoutput.printf("	show columns in table [like pattern]	-\n");
	stdoutput.printf("		returns a list of column metadata for the table \"table\"\n");
	stdoutput.printf("	describe table				-\n");
	stdoutput.printf("		returns a list of column metadata for the table \"table\"\n");
	stdoutput.printf("	fields table				-\n");
	stdoutput.printf("		returns a list of column names for the table \"table\"\n\n");
	stdoutput.printf("	setclientinfo info	- sets the client info\n");
	stdoutput.printf("	getclientinfo		- displays the client info\n\n");
	stdoutput.printf("	setresultsetbuffersize size	- fetch size rows at a time\n");
	stdoutput.printf("	getresultsetbuffersize 		- shows rows fetched at a time\n\n");
	stdoutput.printf("	endsession		- ends the current session\n\n");
	stdoutput.printf("	cache [filename] [ttl]	- caches the next result set to \"filename\"\n	                      	  with ttl of \"ttl\"\n");
	stdoutput.printf("	opencache [filename] 	- opens and displays cached result set \n				  in \"filename\"\n\n");
	stdoutput.printf("	exit/quit		- ");
	stdoutput.printf("exits\n\n");
	stdoutput.printf("	All commands must be followed by the delimiter: %c\n",
								env->delimiter);
}

void sqlrsh::startupMessage(sqlrshenv *env, const char *host,
					uint16_t port, const char *user) {

	stdoutput.printf("SQLRShell - ");
	stdoutput.printf("Version %s\n",SQLR_VERSION);
	stdoutput.printf("	Connected to: ");
	stdoutput.printf("%s:%d as %s\n\n",host,port,user);
	stdoutput.printf("	type help; for help.\n\n");
}

void sqlrsh::interactWithUser(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
							sqlrshenv *env) {

	// init some variables
	stringbuffer	command;
	stringbuffer	prmpt;
	int		exitprogram=0;
	uint32_t	promptcount;

	while (!exitprogram) {

		// prompt the user
		promptcount=0;
		
		// get the command
		bool	done=false;
		while (!done) {
			#ifdef HAVE_READLINE
				prmpt.append(promptcount);
				prmpt.append("> ");
				char	*cmd=readline(const_cast<char *>(
							prmpt.getString()));
				prmpt.clear();
				if (!charstring::isNullOrEmpty(cmd)) {
					add_history(cmd);
				} else {
					stdoutput.printf("\n");
				}
			#else
				prompt(promptcount);
				char	cmd[1024];
				ssize_t	bytes=stdinput.read(cmd,1024);
				cmd[bytes-1]='\0';
				#ifdef ADD_NEWLINE_AFTER_READ_FROM_STDIN
					stdoutput.printf("\n");
				#endif
			#endif
			size_t	len=charstring::length(cmd);
			command.append(cmd);
			done=(cmd[len-1]==env->delimiter);
			if (!done) {
				promptcount++;
				command.append('\n');
			}
			#ifdef HAVE_READLINE
				delete[] cmd;
			#endif
		}

		char	*cmd=command.detachString();

		// run the command
		if (!runCommands(sqlrcon,sqlrcur,env,cmd)) {
			exitprogram=1;
		}

		// clean up
		delete[] cmd;
	}
}

void sqlrsh::prompt(unsigned long promptcount) {
	stdoutput.printf("%ld> ",promptcount);
}

void sqlrsh::execute(int argc, const char **argv) {

	cmdline=new sqlrcmdline(argc,argv);
	sqlrpth=new sqlrpaths(cmdline);
	sqlrconfigs	sqlrcfgs(sqlrpth);

	const char	*configurl=sqlrpth->getConfigUrl();
	const char	*id=cmdline->getValue("-id");
	const char	*host=cmdline->getValue("-host");
	uint16_t	port=charstring::toInteger(
				(cmdline->found("-port"))?
				cmdline->getValue("-port"):DEFAULT_PORT);
	const char	*socket=cmdline->getValue("-socket");
	const char	*user=cmdline->getValue("-user");
	const char	*password=cmdline->getValue("-password");
	const char	*script=cmdline->getValue("-script");
	const char	*command=cmdline->getValue("-command");
	
	if (charstring::isNullOrEmpty(id) ||
		charstring::isNullOrEmpty(host) ||
		charstring::isNullOrEmpty(socket)) {

		stdoutput.printf("usage:\n"
			" %ssh -host host -port port -socket socket "
			"-user user -password password \\\n"
			"        [-script script | -command command] [-quiet] "
			"[-format plain|csv] \\\n"
			"        [-resultsetbuffersize rows]\n"
			"  or\n"
			" %ssh [-config config] -id id \\\n"
			"        [-script script | -command command] [-quiet] "
			"[-format plain|csv] \\\n"
			"        [-resultsetbuffersize rows]\n",
			SQLR,SQLR);
		process::exit(1);
	}

	sqlrconfig	*cfg=sqlrcfgs.load(configurl,id);
	if (cfg) {

		// get the host/port/socket/username/password
		host="localhost";
		port=cfg->getDefaultPort();
		socket=cfg->getDefaultSocket();
		linkedlistnode< usercontainer * >	*firstuser=
					cfg->getUserList()->getFirst();
		if (firstuser) {
			usercontainer	*currentnode=firstuser->getValue();
			user=currentnode->getUser();
			password=currentnode->getPassword();
		}
	}

	// connect to sql relay
	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	// set up an sqlrshenv
	sqlrshenv	env;

	// handle quiet flag
	if (cmdline->found("-quiet")) {
		env.headers=false;
		env.stats=false;
	}

	// handle the result set format
	if (!charstring::compare(cmdline->getValue("-format"),"csv")) {
		env.format=SQLRSH_FORMAT_CSV;
	}

	// handle the result set buffer size
	if (cmdline->found("-resultsetbuffersize")) {
		env.rsbs=charstring::toInteger(
				cmdline->getValue("-resultsetbuffersize"));
	}

	// process RC files
	systemRcFile(&sqlrcon,&sqlrcur,&env);
	userRcFile(&sqlrcon,&sqlrcur,&env);


	#ifdef HAVE_READLINE

		// handle the history file
		char		*filename=NULL;
		const char	*home=environment::getValue("HOME");
		if (!charstring::isNullOrEmpty(home)) {
			filename=new char[charstring::length(home)+16+1];
			charstring::copy(filename,home);
			charstring::append(filename,"/.sqlrsh_history");

			// create the history file if it doesn't exist now
			file	historyfile;
			if (historyfile.open(filename,
				O_WRONLY|O_CREAT|O_APPEND,
				permissions::evalPermString("rw-rw-r--"))) {
				historyfile.close();
				read_history(filename);
			}
		}
	#endif

	if (!charstring::isNullOrEmpty(script)) {
		// if a script was specified, run it
		runScript(&sqlrcon,&sqlrcur,&env,script,true);
	} else if (!charstring::isNullOrEmpty(command)) {
		// if a command was specified, run it
		runCommands(&sqlrcon,&sqlrcur,&env,command);
	} else {
		// otherwise go into interactive mode
		startupMessage(&env,host,port,user);
		interactWithUser(&sqlrcon,&sqlrcur,&env);
	}

	// clean up
	#ifdef HAVE_READLINE
		if (!charstring::isNullOrEmpty(home)) {
			write_history(filename);
			history_truncate_file(filename,100);
			delete[] filename;
		}
	#endif
}

static void helpmessage() {
	stdoutput.printf(
		"%ssh is the SQL Relay command line database shell.\n"
		"\n"
		"It can be used interactively, or non-interactively to run queries directly from the command line, or scripts containing queries.\n"
		"\n"
		"Usage: %ssh [OPTIONS]\n"
		"\n"
		"Options:\n"
		"\n"
		CONNECTIONOPTIONS
		"\n"
		"Command options:\n"
		"	-script filename	name of file containing commands/queries to run\n"
		"	-command \"commands\"	semicolon-separated commands/queries to run\n"
		"	-quiet			omit headers and stats in output\n"
		"	-format plain|csv	sets output format to plain or csv\n"
		"	-resultsetbuffersize rows\n"
		"				fetch result sets using the specified number of\n"
		"				rows at once\n"
		"\n"
		"Examples:\n"
		"\n"
		"Interactive session with server at svr:9000 as usr/pwd.\n"
		"\n"
		"	%ssh -host svr -port 9000 -user usr -password pwd\n"
		"\n"
		"Interactive session with local server on socket /tmp/svr.sock as usr/pwd.\n"
		"\n"
		"	%ssh -socket /tmp/svr.sock -user usr -password pwd\n"
		"\n"
		"Interactive session using connection info and credentials from instance myinst, as defined in the default configuration file.\n"
		"	%ssh -id myinst\n"
		"\n"
		"Interactive session using connection info and credentials from instance myinst, as defined in the config file ./myconfig.conf\n"
		"\n"
		"	%ssh -config ./myconfig.conf -id myinst\n"
		"\n"
		"Non-interactive session, running commands from ./script.sql\n"
		"\n"
		"	%ssh -id myinst -script ./script.sql\n"
		"\n"
		"Non-interactive session, running query \"select * from mytable\" with csv output.\n"
		"\n"
		"	%ssh -id myinst -command \"select * from mytable\" -quiet -format csv\n"
		"\n"
		REPORTBUGS,
		SQLR,SQLR,SQLR,SQLR,SQLR,SQLR,SQLR,SQLR);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	#ifdef SIGPIPE
	// ignore SIGPIPE
	signalset	set;
	set.removeAllSignals();
	set.addSignal(SIGPIPE);
	signalmanager::ignoreSignals(&set);
	#endif

	sqlrsh	s;
	s.execute(argc,argv);
	process::exit(0);
}
