// Copyright (c) 1999-2018 David Muse
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
#include <rudiments/prompt.h>
#include <config.h>
#include <defaults.h>
#define NEED_IS_BIT_TYPE_CHAR 1
#define NEED_IS_NUMBER_TYPE_CHAR 1
#define NEED_IS_FLOAT_TYPE_CHAR 1
#define NEED_IS_NONSCALE_FLOAT_TYPE_CHAR 1
#include <datatypes.h>
#include <defines.h>
#include <parsedatetime.h>
#include <version.h>
// FIXME: use rudiments locale class instead
#include <locale.h>
#include <math.h>

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
				bool		isnegative;
			} dateval;
		};
		sqlrclientbindvartype_t	type;
		uint32_t		outputstringbindlength;

		void	print() {}
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
		bool		divider;
		bool		stats;
		uint64_t	rsbs;
		bool		final;
		bool		autocommit;
		bool		lazyfetch;
		char		delimiter;
		dictionary<char *, sqlrshbindvalue *>	inputbinds;
		memorypool	inbindpool;
		dictionary<char *, sqlrshbindvalue *>	outputbinds;
		dictionary<char *, sqlrshbindvalue *>	inputoutputbinds;
		char		*cacheto;
		sqlrshformat	format;
		bool		getasnumber;
		bool		noelapsed;
		bool		nextresultset;
};

sqlrshenv::sqlrshenv() {
	headers=true;
	divider=true;
	stats=true;
	rsbs=100;
	final=false;
	autocommit=false;
	lazyfetch=false;
	delimiter=';';
	cacheto=NULL;
	format=SQLRSH_FORMAT_PLAIN;
	getasnumber=false;
	noelapsed=false;
	nextresultset=false;
}

sqlrshenv::~sqlrshenv() {
	clearbinds(&inputbinds);
	clearbinds(&outputbinds);
	clearbinds(&inputoutputbinds);
	delete[] cacheto;
}

void sqlrshenv::clearbinds(dictionary<char *, sqlrshbindvalue *> *binds) {

	for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
					*node=binds->getList()->getFirst();
		node; node=node->getNext()) {

		delete[] node->getValue()->getKey();
		sqlrshbindvalue	*bv=node->getValue()->getValue();
		if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
			delete[] bv->stringval;
		}
		delete bv;
	}
	binds->clear();
	inbindpool.clear();
}

enum querytype_t {
	SHOW_DATABASES_QUERY=0,
	SHOW_TABLES_QUERY,
	SHOW_COLUMNS_QUERY,
	SHOW_PRIMARY_KEYS_QUERY,
	DESCRIBE_QUERY
};

class	sqlrsh {
	public:
			sqlrsh();
			~sqlrsh();
		bool	execute(int argc, const char **argv);
	private:
		void	startupMessage(sqlrshenv *env,
					const char *host,
					uint16_t port,
					const char *user);
		void	userRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env);
		bool	runScript(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					sqlrshenv *env, 
					const char *filename,
					bool displayerror);
		bool	runCommands(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					sqlrshenv *env, 
					const char *commands,
					bool *exitprogram);
		bool	getCommandFromFileOrString(file *fl,
					const char *string,
					const char **stringpos,
					stringbuffer *cmdbuffer,
					sqlrshenv *env);
		bool	runCommand(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env,
					const char *command,
					bool *exitprogram);
		int	commandType(const char *command);
		bool	internalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					sqlrshenv *env,
					const char *command);
		bool	externalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					sqlrshenv *env, 
					const char *command);
		void	executeQuery(sqlrcursor *sqlrcur,
					sqlrshenv *env);
		char	*getWild(const char *command);
		char	*getTable(const char *command, bool in);
		char	*getProcedure(const char *command);
		char	*getType(const char *command);
		void	initStats(sqlrshenv *env);
		void	displayError(sqlrshenv *env,
					const char *message,
					const char *error,
					int64_t errornumber);
		void	displayHeader(sqlrcursor *sqlrcur,
						sqlrshenv *env);
		void	displayResultSet(sqlrcursor *sqlrcur,
						sqlrshenv *env);
		void	displayStats(sqlrcursor *sqlrcur,
						sqlrshenv *env);
		bool	ping(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	identify(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	dbversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	dbhostname(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	dbipaddress(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		void	clientversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	serverversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	lastinsertid(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	inputbind(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		bool	inputbindblob(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		bool	outputbind(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		bool	inputoutputbind(sqlrcursor *sqlrcur,
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
		bool	cache(sqlrshenv *env, sqlrcursor *sqlrcur,
							const char *command);
		bool	openCache(sqlrshenv *env, sqlrcursor *sqlrcur,
							const char *command);
		void	displayHelp(sqlrshenv *env);
		void	interactWithUser(sqlrconnection *sqlrcon,
						sqlrcursor *sqlrcur,
						sqlrshenv *env);

		sqlrcmdline	*cmdline;
		sqlrpaths	*sqlrpth;

		datetime	start;

		prompt		pr;
};

sqlrsh::sqlrsh() {
	cmdline=NULL;
	sqlrpth=NULL;
}

sqlrsh::~sqlrsh() {
	delete cmdline;
	delete sqlrpth;
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

bool sqlrsh::runScript(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
			sqlrshenv *env, const char *filename,
			bool displayerror) {

	bool	retval=true;

	char	*trimmedfilename=charstring::duplicate(filename);
	charstring::bothTrim(trimmedfilename);

	// open the file
	file	scriptfile;
	if (scriptfile.open(trimmedfilename,O_RDONLY)) {

		// optimize
		filesystem	fs;
		if (fs.open(trimmedfilename)) {
			scriptfile.setReadBufferSize(
				fs.getOptimumTransferBlockSize());
		}

		for (;;) {
		
			// get a command
			stringbuffer	command;
			if (!getCommandFromFileOrString(
					&scriptfile,NULL,NULL,&command,env)) {
				retval=false;
				break;
			}

			// run the command
			if (!runCommand(sqlrcon,sqlrcur,env,
						command.getString(),
						NULL)) {
				retval=false;
				break;
			}
		}

		// close the file
		scriptfile.close();

	} else {

		// error message
		if (displayerror) {
			stderror.printf("Couldn't open file: %s\n\n",
							trimmedfilename);
		}
		retval=false;
	}

	delete[] trimmedfilename;

	return retval;
}

bool sqlrsh::runCommands(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur, 
				sqlrshenv *env,
				const char *commands,
				bool *exitprogram) {

	const char	*nextcommand=commands;
	for (;;) {
		stringbuffer	command;
		if (!getCommandFromFileOrString(NULL,
						nextcommand,
						&nextcommand,
						&command,
						env)) {
			break;
		}
		if (!runCommand(sqlrcon,sqlrcur,env,
					command.getString(),
					exitprogram)) {
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

bool sqlrsh::runCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, 
					sqlrshenv *env,
					const char *command,
					bool *exitprogram) {

	int	cmdtype=commandType(command);
	if (exitprogram) {
		*exitprogram=false;
	}

	// init stats
	initStats(env);

	if (cmdtype>0) {
		// if the command an internal command, run it as one
		return internalCommand(sqlrcon,sqlrcur,env,command);
	} else if (cmdtype==0) {
		// if the command is not an internal command, 
		// execute it as a query and display the result set
		return externalCommand(sqlrcon,sqlrcur,env,command);
	}

	// exit
	if (exitprogram) {
		*exitprogram=true;
	}
	return true;
}

int sqlrsh::commandType(const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	if (!charstring::compareIgnoringCase(ptr,"headers",7) ||
		!charstring::compareIgnoringCase(ptr,"divider",7) ||
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
		!charstring::compareIgnoringCase(ptr,"currentschema") ||
		!charstring::compareIgnoringCase(ptr,"run",3) ||
		!charstring::compareIgnoringCase(ptr,"@",1) ||
		!charstring::compareIgnoringCase(ptr,"delimiter",9) ||
		!charstring::compareIgnoringCase(ptr,"delimeter",9) ||
		!charstring::compareIgnoringCase(ptr,"inputbind ",10) ||
		!charstring::compareIgnoringCase(ptr,"inputbindblob ",14) ||
		!charstring::compareIgnoringCase(ptr,"outputbind ",11) ||
		!charstring::compareIgnoringCase(ptr,"inputoutputbind ",16) ||
		!charstring::compareIgnoringCase(ptr,"printinputbind",14) ||
		!charstring::compareIgnoringCase(ptr,"printoutputbind",15) ||
		!charstring::compareIgnoringCase(
					ptr,"printinputoutputbind",20) ||
		!charstring::compareIgnoringCase(ptr,"printbinds") ||
		!charstring::compareIgnoringCase(ptr,"clearinputbind",14) ||
		!charstring::compareIgnoringCase(ptr,"clearoutputbind",15) ||
		!charstring::compareIgnoringCase(
					ptr,"clearinputoutputbind",20) ||
		!charstring::compareIgnoringCase(ptr,"clearbinds") ||
		!charstring::compareIgnoringCase(ptr,"lastinsertid") ||
		!charstring::compareIgnoringCase(ptr,"setclientinfo ",14) ||
		!charstring::compareIgnoringCase(ptr,"getclientinfo") ||
		!charstring::compareIgnoringCase(ptr,
					"setresultsetbuffersize ",23) ||
		!charstring::compareIgnoringCase(ptr,
					"getresultsetbuffersize") ||
		!charstring::compareIgnoringCase(ptr,"lazyfetch ",10) ||
		!charstring::compareIgnoringCase(ptr,"endsession") ||
		!charstring::compareIgnoringCase(ptr,"querytree") ||
		!charstring::compareIgnoringCase(ptr,"translatedquery") ||
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

bool sqlrsh::internalCommand(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
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
	} else if (!charstring::compareIgnoringCase(ptr,"divider",7)) {
		ptr=ptr+7;
		cmdtype=11;
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
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"ping")) {	
		return ping(sqlrcon,env);
	} else if (!charstring::compareIgnoringCase(ptr,"use ",4)) {	
		if (!sqlrcon->selectDatabase(ptr+4)) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			return false;
		}
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"currentdb")) {	
		const char	*currentdb=sqlrcon->getCurrentDatabase();
		if (currentdb) {
			stdoutput.printf("%s\n",currentdb);
		} else if (sqlrcon->errorMessage()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			return false;
		} else {
			stdoutput.printf("\n");
		}
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"currentschema")) {	
		const char	*currentschema=sqlrcon->getCurrentSchema();
		if (currentschema) {
			stdoutput.printf("%s\n",currentschema);
		} else if (sqlrcon->errorMessage()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			return false;
		} else {
			stdoutput.printf("\n");
		}
		return true;
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
		return identify(sqlrcon,env);
	} else if (!charstring::compareIgnoringCase(ptr,"dbversion")) {	
		return dbversion(sqlrcon,env);
	} else if (!charstring::compareIgnoringCase(ptr,"dbhostname")) {	
		return dbhostname(sqlrcon,env);
	} else if (!charstring::compareIgnoringCase(ptr,"dbipaddress")) {	
		return dbipaddress(sqlrcon,env);
	} else if (!charstring::compareIgnoringCase(ptr,"clientversion")) {	
		clientversion(sqlrcon,env);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"serverversion")) {	
		return serverversion(sqlrcon,env);
	} else if (!charstring::compareIgnoringCase(ptr,"inputbind ",10)) {	
		return inputbind(sqlrcur,env,command);
	} else if (!charstring::compareIgnoringCase(ptr,"inputbindblob ",14)) {	
		return inputbindblob(sqlrcur,env,command);
	} else if (!charstring::compareIgnoringCase(ptr,"outputbind ",11)) {	
		return outputbind(sqlrcur,env,command);
	} else if (!charstring::compareIgnoringCase(
						ptr,"inputoutputbind ",16)) {	
		return inputoutputbind(sqlrcur,env,command);
	} else if (!charstring::compareIgnoringCase(ptr,"printbinds")) {	
		printbinds("Input",&env->inputbinds);
		stdoutput.printf("\n");
		printbinds("Output",&env->outputbinds);
		stdoutput.printf("\n");
		printbinds("Input/Output",&env->inputoutputbinds);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"clearinputbind",14)) {	
		env->clearbinds(&env->inputbinds);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"clearoutputbind",15)) {
		env->clearbinds(&env->outputbinds);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,
						"clearinputoutputbind",20)) {
		env->clearbinds(&env->inputoutputbinds);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"clearbinds")) {	
		env->clearbinds(&env->inputbinds);
		env->clearbinds(&env->outputbinds);
		env->clearbinds(&env->inputoutputbinds);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"lastinsertid")) {	
		if (!lastinsertid(sqlrcon,env)) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			return false;
		}
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"setclientinfo ",14)) {	
		setclientinfo(sqlrcon,command);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"getclientinfo")) {	
		getclientinfo(sqlrcon);
		return true;
	} else if (!charstring::compareIgnoringCase(
					ptr,"setresultsetbuffersize ",23)) {	
		ptr=ptr+23;
		env->rsbs=charstring::toInteger(ptr);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"lazyfetch ",10)) {
		ptr=ptr+10;
		cmdtype=12;
	} else if (!charstring::compareIgnoringCase(
					ptr,"getresultsetbuffersize")) {	
		stdoutput.printf("%lld\n",(long long)env->rsbs);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"endsession")) {	
		sqlrcon->endSession();
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"querytree")) {	
		xmldom	xmld;
		if (xmld.parseString(sqlrcur->getQueryTree())) {
			xmld.getRootNode()->write(&stdoutput,true);
		}
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"translatedquery")) {	
		stdoutput.printf("%s\n",sqlrcur->getTranslatedQuery());
		return true;
	} else if (!charstring::compareIgnoringCase(
					ptr,"response timeout",16)) {
		responseTimeout(sqlrcon,command);
		return true;
	} else if (!charstring::compareIgnoringCase(ptr,"cache ",6)) {
		return cache(env,sqlrcur,command);
	} else if (!charstring::compareIgnoringCase(ptr,"opencache ",10)) {
		return openCache(env,sqlrcur,command);
	} else {
		return false;
	}

	// skip white space
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// handle scripts
	if (cmdtype==6) {
		return runScript(sqlrcon,sqlrcur,env,ptr,true);
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
		return true;
	}

	// handle nullsasnulls
	if (cmdtype==9) {
		if (!charstring::compareIgnoringCase(ptr,"on",2)) {
			sqlrcur->getNullsAsNulls();
		} else if (!charstring::compareIgnoringCase(ptr,"off",3)) {
			sqlrcur->getNullsAsEmptyStrings();
		}
		return true;
	}

	// handle format
	if (cmdtype==10) {
		if (!charstring::compareIgnoringCase(ptr,"csv",3)) {
			env->format=SQLRSH_FORMAT_CSV;
		} else {
			env->format=SQLRSH_FORMAT_PLAIN;
		}
		return true;
	}

	// on or off?
	bool	toggle=false;
	if (!charstring::compareIgnoringCase(ptr,"on",2)) {
		toggle=true;
	}

	// set parameter
	switch (cmdtype) {
		case 2:
			env->headers=toggle;
			break;
		case 11:
			env->divider=toggle;
			break;
		case 3:
			env->stats=toggle;
			break;
		case 5:
			env->final=toggle;
			break;
		case 7:
			env->delimiter=ptr[0];
			stdoutput.printf("Delimiter set to %c\n",
							env->delimiter);
			break;
		case 8:
			if (toggle) {
				if (sqlrcon->autoCommitOn()) {
					stdoutput.printf(
						"Autocommit set on\n");
				} else {
					displayError(env,NULL,
						sqlrcon->errorMessage(),
						sqlrcon->errorNumber());
					return false;
				}
			} else {
				if (sqlrcon->autoCommitOff()) {
					stdoutput.printf(
						"Autocommit set off\n");
				} else {
					displayError(env,NULL,
						sqlrcon->errorMessage(),
						sqlrcon->errorNumber());
					return false;
				}
			}
			break;
		case 12:
			env->lazyfetch=toggle;
			break;
	}
	return true;
}

bool sqlrsh::externalCommand(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur, sqlrshenv *env, 
				const char *command) {

	bool	retval=true;

	// handle begin, commit and rollback
	if (!charstring::compareIgnoringCase(command,"begin")) {

		if (!sqlrcon->begin()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			retval=false;
		}

	} else if (!charstring::compareIgnoringCase(command,"commit")) {

		if (!sqlrcon->commit()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			retval=false;
		}

	} else if (!charstring::compareIgnoringCase(command,"rollback")) {

		if (!sqlrcon->rollback()) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
			retval=false;
		}

	} else if (!charstring::compareIgnoringCase(command,"fields ",7)) {

		char	*table=getTable(command,false);
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

		if (env->lazyfetch) {
			sqlrcur->lazyFetch();
		} else {
			sqlrcur->dontLazyFetch();
		}

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
						"show schemas",12)) {
			char	*wild=getWild(command);
			sqlrcur->getSchemaList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show tables odbc",16)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_ODBC,
					DB_OBJECT_TABLE|
					DB_OBJECT_VIEW|
					DB_OBJECT_ALIAS|
					DB_OBJECT_SYNONYM);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show only tables odbc",21)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_ODBC,
					DB_OBJECT_TABLE);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show only views odbc",20)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_ODBC,
					DB_OBJECT_VIEW);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show only aliases odbc",22)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_ODBC,
					DB_OBJECT_ALIAS);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show only synonyms odbc",23)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild,
					SQLRCLIENTLISTFORMAT_ODBC,
					DB_OBJECT_SYNONYM);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show tables",11)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show table types",16)) {
			char	*wild=getWild(command);
			sqlrcur->getTableTypeList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show columns odbc",17)) {
			char	*table=getTable(command,true);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild,
					SQLRCLIENTLISTFORMAT_ODBC);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show columns",12)) {
			char	*table=getTable(command,true);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show primary keys",17)) {
			char	*table=getTable(command,true);
			char	*wild=getWild(command);
			sqlrcur->getPrimaryKeysList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
						"show keys and indexes",21)) {
			char	*table=getTable(command,true);
			char	*wild=getWild(command);
			sqlrcur->getKeyAndIndexList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"describe ",9)) {
			char	*table=getTable(command,false);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
				"show procedure binds and columns",32)) {
			char	*procedure=getProcedure(command);
			char	*wild=getWild(command);
			sqlrcur->getProcedureBindAndColumnList(procedure,wild);
			delete[] procedure;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show type info",14)) {
			char	*type=getType(command);
			sqlrcur->getTypeInfoList(type,NULL);
			delete[] type;
		} else if (!charstring::compareIgnoringCase(command,
						"show procedures",15)) {
			char	*wild=getWild(command);
			sqlrcur->getProcedureList(wild);
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
			retval=false;

		} else if (env->nextresultset) {

			do {

				// display the header
				displayHeader(sqlrcur,env);

				// display the result set
				displayResultSet(sqlrcur,env);

				// display any errors
				if (sqlrcur->errorMessage()) {
					displayError(env,NULL,
						sqlrcur->errorMessage(),
						sqlrcur->errorNumber());
					retval=false;
				}

			} while (sqlrcur->nextResultSet());

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

	return retval;
}

void sqlrsh::executeQuery(sqlrcursor *sqlrcur, sqlrshenv *env) {

	sqlrcur->clearBinds();

	if (env->inputbinds.getList()->getLength()) {

		for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
				*node=env->inputbinds.getList()->getFirst();
				node; node=node->getNext()) {

			const char	*name=node->getValue()->getKey();
			sqlrshbindvalue	*bv=node->getValue()->getValue();
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				sqlrcur->inputBind(name,bv->stringval);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
				sqlrcur->inputBind(name,bv->integerval);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
				sqlrcur->inputBind(name,bv->doubleval.value,
							bv->doubleval.precision,
							bv->doubleval.scale);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
				sqlrcur->inputBind(name,
						bv->dateval.year,
						bv->dateval.month,
						bv->dateval.day,
						bv->dateval.hour,
						bv->dateval.minute,
						bv->dateval.second,
						bv->dateval.microsecond,
						bv->dateval.tz,
						bv->dateval.isnegative);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_BLOB) {
				sqlrcur->inputBindBlob(name,bv->stringval,
					charstring::length(bv->stringval));
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_NULL) {
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
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				// FIXME: make buffer length variable
				sqlrcur->defineOutputBindString(name,
						bv->outputstringbindlength);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
				sqlrcur->defineOutputBindInteger(name);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
				sqlrcur->defineOutputBindDouble(name);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
				sqlrcur->defineOutputBindDate(name);
			}
		}
	}

	if (env->inputoutputbinds.getList()->getLength()) {

		for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
			*node=env->inputoutputbinds.getList()->getFirst();
			node; node=node->getNext()) {

			const char	*name=node->getValue()->getKey();
			sqlrshbindvalue	*bv=node->getValue()->getValue();
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				sqlrcur->defineInputOutputBindString(name,
						bv->stringval,
						bv->outputstringbindlength);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
				sqlrcur->defineInputOutputBindInteger(name,
						bv->integerval);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
				sqlrcur->defineInputOutputBindDouble(name,
						bv->doubleval.value,
						bv->doubleval.precision,
						bv->doubleval.scale);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
				sqlrcur->defineInputOutputBindDate(name,
						bv->dateval.year,
						bv->dateval.month,
						bv->dateval.day,
						bv->dateval.hour,
						bv->dateval.minute,
						bv->dateval.second,
						bv->dateval.microsecond,
						bv->dateval.tz,
						bv->dateval.isnegative);
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
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] bv->stringval;
				bv->stringval=charstring::duplicate(
					sqlrcur->getOutputBindString(name));
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
				bv->integerval=
					sqlrcur->getOutputBindInteger(name);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
				bv->doubleval.value=
					sqlrcur->getOutputBindDouble(name);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
				sqlrcur->getOutputBindDate(name,
						&(bv->dateval.year),
						&(bv->dateval.month),
						&(bv->dateval.day),
						&(bv->dateval.hour),
						&(bv->dateval.minute),
						&(bv->dateval.second),
						&(bv->dateval.microsecond),
						&(bv->dateval.tz),
						&(bv->dateval.isnegative));
			}
		}
	}

	if (env->inputoutputbinds.getList()->getLength()) {

		for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
			*node=env->inputoutputbinds.getList()->getFirst();
			node; node=node->getNext()) {

			const char	*name=node->getValue()->getKey();
			sqlrshbindvalue	*bv=node->getValue()->getValue();
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] bv->stringval;
				bv->stringval=charstring::duplicate(
				sqlrcur->getInputOutputBindString(name));
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
				bv->integerval=
				sqlrcur->getInputOutputBindInteger(name);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
				bv->doubleval.value=
				sqlrcur->getInputOutputBindDouble(name);
			} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
				sqlrcur->getInputOutputBindDate(name,
						&(bv->dateval.year),
						&(bv->dateval.month),
						&(bv->dateval.day),
						&(bv->dateval.hour),
						&(bv->dateval.minute),
						&(bv->dateval.second),
						&(bv->dateval.microsecond),
						&(bv->dateval.tz),
						&(bv->dateval.isnegative));
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

char *sqlrsh::getTable(const char *command, bool in) {
	const char	*tableptr=NULL;
	if (in) {
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
	} else {
		tableptr=charstring::findFirst(command," ");
		if (!tableptr) {
			return NULL;
		}
		return charstring::duplicate(tableptr+1);
	}
	return NULL;
}

char *sqlrsh::getProcedure(const char *command) {
	const char	*procptr=charstring::findFirst(command," in ");
	if (!procptr) {
		return NULL;
	}
	procptr=procptr+4;
	const char	*endptr=charstring::findFirst(procptr," ");
	if (!endptr) {
		return charstring::duplicate(procptr);
	}
	return charstring::duplicate(procptr,endptr-procptr);
}

char *sqlrsh::getType(const char *command) {
	const char	*procptr=charstring::findFirst(command," for ");
	if (!procptr) {
		return NULL;
	}
	procptr=procptr+5;
	const char	*endptr=charstring::findFirst(procptr," ");
	if (!endptr) {
		return charstring::duplicate(procptr);
	}
	return charstring::duplicate(procptr,endptr-procptr);
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
		stderror.printf("%s\n",message);
	}
	stderror.printf("%lld:\n",(long long)errornumber);
	if (!charstring::isNullOrEmpty(error)) {
		stderror.printf("%s\n\n",error);
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

	// display divider
	if (env->divider) {
		for (uint32_t i=0; i<charcount; i++) {
			stdoutput.printf("=");
		}
		stdoutput.printf("\n");
	}
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
	const char	*fieldtype;
	char		numberfieldbuffer[256];

	bool		done=false;
	for (uint64_t row=0; !done; row++) {

		for (uint32_t col=0; col<colcount; col++) {

			// put a comma or extra space between fields
			if (col) {
				if (env->format==SQLRSH_FORMAT_CSV) {
					stdoutput.write(',');
				} else {
					stdoutput.write(' ');
				}
			}

			// get the field
			field=sqlrcur->getField(row,col);
			fieldlength=sqlrcur->getFieldLength(row,col);
			fieldtype=sqlrcur->getColumnType(col);

			// FIXME: move this down below the end-of-rs check?
			// The purpose of this is to verify the functionality
			// of the getFieldAsXXX() methods.
			if (field && env->getasnumber &&
				(isBitTypeChar(fieldtype) ||
					isNumberTypeChar(fieldtype))) {

				if (isFloatTypeChar(fieldtype)) {
					double	fd=sqlrcur->getFieldAsDouble(row,col);
					if (isNonScaleFloatTypeChar(fieldtype)) {
						int32_t	precision=sqlrcur->getColumnPrecision(col);
						// here precision is a number of bits, but printf %g wants digits.
						// FIXME: precision should actually be the number of digits, not bits...
						int32_t	digits=(int32_t)(ceil(precision/3.33));
						charstring::printf(&numberfieldbuffer[0],sizeof(numberfieldbuffer),"%.*g",digits,fd);
					} else {
						int	scale=sqlrcur->getColumnScale(col);
						// NOTE: we are not using the precision to format the number to a string.
						charstring::printf(&numberfieldbuffer[0],sizeof(numberfieldbuffer),"%.*f",scale,fd);
					}
				} else {
					int64_t fi = sqlrcur->getFieldAsInteger(row,col);
					charstring::printf(&numberfieldbuffer[0], sizeof(numberfieldbuffer), "%ld", fi);
				}
				field=numberfieldbuffer;
				fieldlength=charstring::length(field);
			}

			// check for end-of-result-set condition
			// (since nullsasnulls might be set, we have to do 
			// a bit more than just check for a NULL)
			if (!col && !field &&
				sqlrcur->endOfResultSet() &&
				row==sqlrcur->rowCount()) {
				done=true;
				break;
			}

			// handle nulls
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
				longest=sqlrcur->getLongest(col);
				if (env->headers) {
					namelen=charstring::length(
						sqlrcur->getColumnName(col));
					if (namelen>longest) {
						longest=namelen;
					}
				}
				for (uint32_t i=fieldlength; i<longest; i++) {
					stdoutput.write(' ');
				}
			}
		}
		stdoutput.write('\n');
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
	if (!env->noelapsed) {
		stdoutput.printf("	Elapsed Time    : ");
		stdoutput.printf("%.6f sec\n",time);
	}
	stdoutput.printf("\n");
}

bool sqlrsh::ping(sqlrconnection *sqlrcon, sqlrshenv *env) {
	bool	result=sqlrcon->ping();
	if (result) {
		stdoutput.printf("	The database is up.\n");
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
		return false;
	} else {
		stdoutput.printf("	The database is down.\n");
	}
	return true;
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

bool sqlrsh::identify(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->identify();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
		return false;
	} else {
		stdoutput.printf("\n");
	}
	return true;
}

bool sqlrsh::dbversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->dbVersion();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
		return false;
	} else {
		stdoutput.printf("\n");
	}
	return true;
}

bool sqlrsh::dbhostname(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->dbHostName();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
		return false;
	} else {
		stdoutput.printf("\n");
	}
	return true;
}

bool sqlrsh::dbipaddress(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->dbIpAddress();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
		return false;
	} else {
		stdoutput.printf("\n");
	}
	return true;
}

void sqlrsh::clientversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	stdoutput.printf("%s\n",sqlrcon->clientVersion());
}

bool sqlrsh::serverversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	const char	*value=sqlrcon->serverVersion();
	if (value) {
		stdoutput.printf("%s\n",value);
	} else if (sqlrcon->errorMessage()) {
		displayError(env,NULL,
				sqlrcon->errorMessage(),
				sqlrcon->errorNumber());
		return false;
	} else {
		stdoutput.printf("\n");
	}
	return true;
}

bool sqlrsh::inputbind(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// sanity check
	const char	*ptr=command+10;
	const char	*space=charstring::findFirst(ptr,' ');
	if (!space) {
		stderror.printf("usage: inputbind [variable] = [value]\n");
		return false;
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
		stderror.printf("usage: inputbind [variable] = [value]\n");
		stderror.printf("       inputbind [variable] is null\n");
		return false;
	}
		
	// get the value
	char	*value=charstring::duplicate(ptr);
	charstring::bothTrim(value);
	size_t	valuelen=charstring::length(value);

	// if the bind variable is already defined, clear it...
	sqlrshbindvalue	*bv=NULL;
	if (env->inputbinds.getValue(variable,&bv)) {
		if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
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
		bv->type=SQLRCLIENTBINDVARTYPE_NULL;
	} else if ((value[0]=='\'' && value[valuelen-1]=='\'') ||
			(value[0]=='"' && value[valuelen-1]=='"')) {

		bv->type=SQLRCLIENTBINDVARTYPE_STRING;

		// trim off quotes
		char	*newvalue=charstring::duplicate(value+1);
		newvalue[valuelen-2]='\0';
		delete[] value;

		// unescape the string
		bv->stringval=charstring::unescape(newvalue);
		delete[] newvalue;

	} else if (charstring::contains(value,"/") && 
			charstring::contains(value,":")) {
		int16_t	year;
		int16_t	month;
		int16_t	day;
		int16_t	hour;
		int16_t	minute;
		int16_t	second;
		int32_t	microsecond;
		bool	isnegative;
		parseDateTime(value,false,false,"/",
					&year,&month,&day,
					&hour,&minute,&second,
					&microsecond,&isnegative);
		bv->type=SQLRCLIENTBINDVARTYPE_DATE;
		bv->dateval.year=year;
		bv->dateval.month=month;
		bv->dateval.day=day;
		bv->dateval.hour=hour;
		bv->dateval.minute=minute;
		bv->dateval.second=second;
		bv->dateval.microsecond=microsecond;
		bv->dateval.tz="";
		bv->dateval.isnegative=isnegative;
		delete[] value;
	} else if (charstring::isInteger(value)) {
		bv->type=SQLRCLIENTBINDVARTYPE_INTEGER;
		bv->integerval=charstring::toInteger(value);
		delete[] value;
	} else if (charstring::isNumber(value)) {
		bv->type=SQLRCLIENTBINDVARTYPE_DOUBLE;
		bv->doubleval.value=charstring::toFloatC(value);
		bv->doubleval.precision=valuelen-((value[0]=='-')?2:1);
		bv->doubleval.scale=
			charstring::findFirst(value,'.')-value+
			((value[0]=='-')?0:1);
		delete[] value;
	} else {
		bv->type=SQLRCLIENTBINDVARTYPE_STRING;
		bv->stringval=value;
	}

	// put the bind variable in the list
	env->inputbinds.setValue(variable,bv);

	return true;
}

bool sqlrsh::inputbindblob(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// sanity check
	const char	*ptr=command+14;
	const char	*space=charstring::findFirst(ptr,' ');
	if (!space) {
		stderror.printf("usage: inputbindblob [variable] = [value]\n");
		return false;
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
		stderror.printf("usage: inputbindblob [variable] = [value]\n");
		stderror.printf("       inputbindblob [variable] is null\n");
		return false;
	}
		
	// get the value
	char	*value=charstring::duplicate(ptr);
	charstring::bothTrim(value);
	size_t	valuelen=charstring::length(value);

	// if the bind variable is already defined, clear it...
	sqlrshbindvalue	*bv=NULL;
	if (env->inputbinds.getValue(variable,&bv)) {
		if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
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
		bv->type=SQLRCLIENTBINDVARTYPE_NULL;
	} else if ((value[0]=='\'' && value[valuelen-1]=='\'') ||
			(value[0]=='"' && value[valuelen-1]=='"')) {

		bv->type=SQLRCLIENTBINDVARTYPE_BLOB;

		// trim off quotes
		char	*newvalue=charstring::duplicate(value+1);
		newvalue[valuelen-2]='\0';
		delete[] value;

		// unescape the string
		bv->stringval=charstring::unescape(newvalue);
		delete[] newvalue;

	} else {
		bv->type=SQLRCLIENTBINDVARTYPE_BLOB;
		bv->stringval=value;
	}

	// put the bind variable in the list
	env->inputbinds.setValue(variable,bv);

	return true;
}

bool sqlrsh::outputbind(sqlrcursor *sqlrcur,
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
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] bv->stringval;
			}
			delete bv;
		}

		// define the variable
		bv=new sqlrshbindvalue;

		if (!charstring::compareIgnoringCase(
						parts[2],"string") &&
						partcount==4) {
			bv->type=SQLRCLIENTBINDVARTYPE_STRING;
			bv->stringval=NULL;
			bv->outputstringbindlength=
				charstring::toInteger(parts[3]);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"integer") &&
						partcount==3) {
			bv->type=SQLRCLIENTBINDVARTYPE_INTEGER;
			bv->integerval=0;
		} else if (!charstring::compareIgnoringCase(
						parts[2],"double") &&
						partcount==5) {
			bv->type=SQLRCLIENTBINDVARTYPE_DOUBLE;
			bv->doubleval.value=0.0;
			bv->doubleval.precision=
				charstring::toInteger(parts[3]);
			bv->doubleval.scale=
				charstring::toInteger(parts[4]);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"date") &&
						partcount==3) {
			bv->type=SQLRCLIENTBINDVARTYPE_DATE;
			bv->dateval.year=0;
			bv->dateval.month=0;
			bv->dateval.day=0;
			bv->dateval.hour=0;
			bv->dateval.minute=0;
			bv->dateval.second=0;
			bv->dateval.microsecond=0;
			bv->dateval.tz="";
			bv->dateval.isnegative=false;
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
		stderror.printf("usage: outputbind "
				// FIXME: not entirely accurate
				"[variable] [type] [length] [scale]\n");
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
	}
	delete[] parts;

	return sane;
}

bool sqlrsh::inputoutputbind(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// get the value
	char		*value=NULL;
	const char	*equals=charstring::findFirst(command,'=');
	if (equals) {
		value=charstring::duplicate(equals+1);
		charstring::bothTrim(value);
		charstring::bothTrim(value,'\'');
	} else if (charstring::compare(
			command+charstring::length(command)-8," is null")) {
		// FIXME: usage...
		return false;
	}

	// split the command on ' '
	char		**parts;
	uint64_t	partcount;
	charstring::split(command," ",true,&parts,&partcount);

	// sanity check...
	bool	sane=true;
	if (partcount>=5 && !charstring::compare(parts[0],"inputoutputbind")) {

		// if the bind variable is already defined, clear it...
		sqlrshbindvalue	*bv=NULL;
		if (env->inputoutputbinds.getValue(parts[1],&bv)) {
			if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
				delete[] bv->stringval;
			}
			delete bv;
		}

		// define the variable
		bv=new sqlrshbindvalue;

		if (!charstring::compareIgnoringCase(
						parts[2],"string") &&
						partcount>=6) {
			// inputoutputbind 1 string length = 'string'
			bv->type=SQLRCLIENTBINDVARTYPE_STRING;
			bv->outputstringbindlength=
				charstring::toInteger(parts[3]);
			bv->stringval=charstring::unescape(value);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"integer") &&
						partcount==5) {
			// inputoutputbind 1 integer = value
			bv->type=SQLRCLIENTBINDVARTYPE_INTEGER;
			bv->integerval=charstring::toInteger(value);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"double") &&
						partcount==7) {
			// inputoutputbind 1 double prec scale = value
			bv->type=SQLRCLIENTBINDVARTYPE_DOUBLE;
			bv->doubleval.value=charstring::toFloatC(value);
			bv->doubleval.precision=
				charstring::toInteger(parts[3]);
			bv->doubleval.scale=
				charstring::toInteger(parts[4]);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"date") &&
						partcount>=5) {
			// inputoutputbind 1 date = '...'
			int16_t	year;
			int16_t	month;
			int16_t	day;
			int16_t	hour;
			int16_t	minute;
			int16_t	second;
			int32_t	microsecond;
			bool	isnegative;
			parseDateTime(value,false,false,"/",
						&year,&month,&day,
						&hour,&minute,&second,
						&microsecond,&isnegative);
			bv->type=SQLRCLIENTBINDVARTYPE_DATE;
			bv->dateval.year=year;
			bv->dateval.month=month;
			bv->dateval.day=day;
			bv->dateval.hour=hour;
			bv->dateval.minute=minute;
			bv->dateval.second=second;
			bv->dateval.microsecond=microsecond;
			bv->dateval.tz="";
			bv->dateval.isnegative=isnegative;
		} else {
			sane=false;
		}

		// put the bind variable in the list
		if (sane) {
			env->inputoutputbinds.setValue(parts[1],bv);
		}

	} else {
		sane=false;
	}

	// clean up
	if (sane) {
		delete[] parts[0];
	} else {
		stderror.printf("usage: inputoutputbind "
				// FIXME: not entirely accurate
				"[variable] [type] [length] [scale]\n");
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
	}
	delete[] parts;
	delete[] value;

	return sane;
}

void sqlrsh::printbinds(const char *type,
			dictionary<char *, sqlrshbindvalue *> *binds) {

	stdoutput.printf("%s bind variables:\n",type);

	for (linkedlistnode<dictionarynode<char *, sqlrshbindvalue *> *>
					*node=binds->getList()->getFirst();
		node; node=node->getNext()) {

		stdoutput.printf("    %s ",node->getValue()->getKey());
		sqlrshbindvalue	*bv=node->getValue()->getValue();
		if (bv->type==SQLRCLIENTBINDVARTYPE_STRING) {
			stdoutput.printf("(STRING) = %s\n",bv->stringval);
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_INTEGER) {
			stdoutput.printf("(INTEGER) = %lld\n",
						(long long)bv->integerval);
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_DOUBLE) {
			stdoutput.printf("(DOUBLE %d,%d) = %*.*f\n",
						bv->doubleval.precision,
						bv->doubleval.scale,
						(int)bv->doubleval.precision,
						(int)bv->doubleval.scale,
						bv->doubleval.value);
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_DATE) {
			stdoutput.printf("(DATE) = %02d/%02d/%04d "
						"%s%02d:%02d:%02d.%06d %s\n",
						bv->dateval.month,
						bv->dateval.day,
						bv->dateval.year,
						(bv->dateval.isnegative)?"-":"",
						bv->dateval.hour,
						bv->dateval.minute,
						bv->dateval.second,
						bv->dateval.microsecond,
						bv->dateval.tz);
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_BLOB) {
			stdoutput.printf("(BLOB) = ");
			stdoutput.safePrint(bv->stringval,
					charstring::length(bv->stringval));
			stdoutput.printf("\n");
		} else if (bv->type==SQLRCLIENTBINDVARTYPE_NULL) {
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

bool sqlrsh::cache(sqlrshenv *env, sqlrcursor *sqlrcur, const char *command) {

	// move to file name
	const char	*ptr=command+6;

	// skip whitespace
	while (*ptr==' ') {
		ptr++;
	}

	// bail if no file name was given
	if (!*ptr) {
		stderror.printf("	No file name given\n\n");
		return false;
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

	return true;
}

bool sqlrsh::openCache(sqlrshenv *env,
			sqlrcursor *sqlrcur, const char *command) {

	// move to file name
	command=command+10;

	// skip whitespace
	while (*command==' ') {
		command++;
	}

	// bail if no file name was given
	if (!*command) {
		stderror.printf("	No file name given\n\n");
		return false;
	}

	// if the file name starts with a slash then use it as-is, otherwise
	// prepend the default cache directory.
	stringbuffer	fn;
	fn.append(sqlrpth->getCacheDir())->append(command);

	// open the cached result set
	if (!sqlrcur->openCachedResultSet(fn.getString())) {
		stderror.printf("	Cannot open cache file\n\n");
		return false;
	}

	// display the header
	displayHeader(sqlrcur,env);

	// display the result set
	displayResultSet(sqlrcur,env);

	// display statistics
	displayStats(sqlrcur,env);

	return true;
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
	stdoutput.printf("returns the version of the client library\n");
	stdoutput.printf("	serverversion		- ");
	stdoutput.printf("returns the version of the server\n");
	stdoutput.printf("	use [database]		- ");
	stdoutput.printf("change the current database/schema\n");
	stdoutput.printf("	currentdb		- ");
	stdoutput.printf("shows the current database/schema\n");
	stdoutput.printf("	run script		- ");
	stdoutput.printf("runs commands contained in file \"script\"\n");
	stdoutput.printf("	headers on|off		- ");
	stdoutput.printf("toggles column descriptions before result set\n");
	stdoutput.printf("	divider on|off		- ");
	stdoutput.printf("toggles the divider before the result set\n");
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

	stdoutput.printf("%ssh - ",SQLR);
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
	bool		exitprogram=false;
	uint32_t	promptcount;

	// Blocking mode is apparently not the default on some systems
	// (Syllable for sure, maybe others) and this causes hilariously
	// odd behavior when reading standard input.
	stdinput.useBlockingMode();

	while (!exitprogram) {

		// prompt the user
		promptcount=0;
		
		// get the command
		bool	done=false;
		while (!done) {

			prmpt.append(promptcount);
			prmpt.append("> ");
			pr.setPrompt(prmpt.getString());
			prmpt.clear();

			char	*cmd=pr.read();

			// cmd is NULL if you hit ctrl-D
			if (!cmd) {
				return;
			}

			size_t	len=charstring::length(cmd);

			// len=0 and cmd="" if you just hit return
			if (len) {
				command.append(cmd);
				done=(cmd[len-1]==env->delimiter);
			}

			if (!done) {
				promptcount++;
				command.append('\n');
			}
		}

		char	*cmd=command.detachString();

		// run the command
		runCommands(sqlrcon,sqlrcur,env,cmd,&exitprogram);

		// clean up
		delete[] cmd;
	}
}

bool sqlrsh::execute(int argc, const char **argv) {

	cmdline=new sqlrcmdline(argc,argv);
	sqlrpth=new sqlrpaths(cmdline);
	sqlrconfigs	sqlrcfgs(sqlrpth);

	// get command-line options
	const char	*configurl=sqlrpth->getConfigUrl();
	const char	*id=cmdline->getValue("id");
	const char	*host=cmdline->getValue("host");
	uint16_t	port=charstring::toInteger(
				(cmdline->found("port"))?
				cmdline->getValue("port"):DEFAULT_PORT);
	const char	*socket=cmdline->getValue("socket");
	const char	*user=cmdline->getValue("user");
	const char	*password=cmdline->getValue("password");
	bool		usekrb=cmdline->found("krb");
	const char	*krbservice=cmdline->getValue("krbservice");
	const char	*krbmech=cmdline->getValue("krbmech");
	const char	*krbflags=cmdline->getValue("krbflags");
	bool		usetls=cmdline->found("tls");
	const char	*tlsversion=cmdline->getValue("tlsversion");
	const char	*tlscert=cmdline->getValue("tlscert");
	const char	*tlspassword=cmdline->getValue("tlspassword");
	const char	*tlsciphers=cmdline->getValue("tlsciphers");
	const char	*tlsvalidate="no";
	if (cmdline->found("tlsvalidate")) {
		tlsvalidate=cmdline->getValue("tlsvalidate");
	}
	const char	*tlsca=cmdline->getValue("tlsca");
	uint16_t	tlsdepth=charstring::toUnsignedInteger(
					cmdline->getValue("tlsdepth"));
	const char	*localeargument=cmdline->getValue("locale");
	const char	*script=cmdline->getValue("script");
	const char	*command=cmdline->getValue("command");
	
	// at least id, host, or socket is required
	if (charstring::isNullOrEmpty(id) &&
		charstring::isNullOrEmpty(host) &&
		charstring::isNullOrEmpty(socket)) {

		stderror.printf("usage:\n"
			" %ssh -host host -port port -socket socket\n"
			"        [-user user] [-password password]\n"
			"        [-krb] [-krbservice svc] [-krbmech mech] "
			"[-krbflags flags]\n"
			"        [-tls] [-tlsversion version]\n"
			"        [-tlscert certfile] [-tlspassword password]\n"
			"        [-tlsciphers cipherlist]\n"
			"        [-tlsvalidate (no|ca|ca+domain|ca+host)] "
			"[-tlsca ca] [-tlsdepth depth]\n"
			"        [-script script | -command command] [-quiet] "
			"[-format (plain|csv)] [-locale (env|name)] "
			"[-getasnumber] [-noelapsed] [-nextresultset]\n"
			"        [-resultsetbuffersize rows]\n"
			"  or\n"
			" %ssh [-config config] -id id\n"
			"        [-script script | -command command] [-quiet] "
			"[-format (plain|csv)] [-locale (env|name)] "
			"[-getasnumber] [-noelapsed] [-nextresultset]\n"
			"        [-resultsetbuffersize rows]\n",
			SQLR,SQLR);
		process::exit(1);
	}

	// if an id was specified, then get various values from the config file
	if (!charstring::isNullOrEmpty(id)) {
		sqlrconfig	*cfg=sqlrcfgs.load(configurl,id);
		if (cfg) {
			if (!cmdline->found("host")) {
				host="localhost";
			}
			if (!cmdline->found("port")) {
				port=cfg->getDefaultPort();
			}
			if (!cmdline->found("socket")) {
				socket=cfg->getDefaultSocket();
			}
			if (!cmdline->found("krb")) {
				usekrb=cfg->getDefaultKrb();
			}
			if (!cmdline->found("krbservice")) {
				krbservice=cfg->getDefaultKrbService();
			}
			if (!cmdline->found("krbmech")) {
				krbmech=cfg->getDefaultKrbMech();
			}
			if (!cmdline->found("krbflags")) {
				krbflags=cfg->getDefaultKrbFlags();
			}
			if (!cmdline->found("tls")) {
				usetls=cfg->getDefaultTls();
			}
			if (!cmdline->getValue("tlsciphers")) {
				tlsciphers=cfg->getDefaultTlsCiphers();
			}
			if (!cmdline->found("user")) {
				user=cfg->getDefaultUser();
				password=cfg->getDefaultPassword();
			}
		}
	}

	if (!charstring::isNullOrEmpty(localeargument)) {
		// This is useful for making sure that decimals still work
		// when the locale is changed to say, de_DE that has different
		// number formats.
		char	*localeresult=setlocale(LC_ALL,
				(!charstring::compare(localeargument,"env"))?
							"":localeargument);
		if (!localeresult) {
			stderror.printf("ERROR: setlocale failed\n");
			return false;
		}
	}

	// configure sql relay connection
	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	// configure kerberos/tls
	if (usekrb) {
		sqlrcon.enableKerberos(krbservice,krbmech,krbflags);
	} else if (usetls) {
		sqlrcon.enableTls(tlsversion,tlscert,tlspassword,tlsciphers,
						tlsvalidate,tlsca,tlsdepth);
	}

	// set up an sqlrshenv
	sqlrshenv	env;

	// handle quiet flag
	if (cmdline->found("quiet")) {
		env.headers=false;
		env.stats=false;
	}

	// handle the result set format
	if (!charstring::compare(cmdline->getValue("format"),"csv")) {
		env.format=SQLRSH_FORMAT_CSV;
	}

	// handle the result set buffer size
	if (cmdline->found("resultsetbuffersize")) {
		env.rsbs=charstring::toInteger(
				cmdline->getValue("resultsetbuffersize"));
	}

	// FIXME: make these commands instead of commandline args
	env.getasnumber=cmdline->found("getasnumber");
	env.noelapsed=cmdline->found("noelapsed");
	env.nextresultset=cmdline->found("nextresultset");

	// process RC files
	userRcFile(&sqlrcon,&sqlrcur,&env);


	// handle the history file
	const char	*home=environment::getValue("HOME");
	if (!charstring::isNullOrEmpty(home)) {
		char	*filename=new char[charstring::length(home)+16+1];
		charstring::copy(filename,home);
		charstring::append(filename,"/.sqlrsh_history");
		pr.setHistoryFile(filename);
		pr.setMaxHistoryLines(100);
	}

	bool	retval=true;

	if (!charstring::isNullOrEmpty(script)) {
		// if a script was specified, run it
		retval=runScript(&sqlrcon,&sqlrcur,&env,script,true);
	} else if (!charstring::isNullOrEmpty(command)) {
		// if a command was specified, run it
		retval=runCommands(&sqlrcon,&sqlrcur,&env,command,NULL);
	} else {
		// otherwise go into interactive mode
		startupMessage(&env,host,port,user);
		interactWithUser(&sqlrcon,&sqlrcur,&env);
	}

	// clean up
	pr.flushHistory();

	return retval;
}

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s command line database shell.\n"
		"\n"
		"It can be used interactively, or non-interactively to run queries directly from the command line, or scripts containing queries.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		"\n"
		CONNECTIONOPTIONS
		"\n"
		"Command options:\n"
		"	-script filename	Run the specified script which contains	commands\n"
		"				or queries that could otherwise be run at the\n"
		"				%s prompt.\n"
		"\n"
		"	-command \"commands\"	Run the provided string which contains commands\n"
		"				or queries that could otherwise be run at the\n"
		"				%s prompt.\n"
		"\n"
		"	-quiet			Omit headers and stats in output.\n"
		"\n"
		"	-format plain|csv	Format the output as specified.\n"
		"				Defaults to plain.\n"
		"\n"
		"	-locale env|locale_name	calls setlocale(LC_ALL, locale_name).\n"
		"				env means use LC variables.\n"
		"\n"
		"	-getasnumber		calls getFieldAs(Integer|Double) as appropriate\n"
		"\n"
		"	-noelapsed		do not print elapsed time\n"
		"\n"
		"	-nextresultset		attempt to fetch multiple resultsets\n"
		"\n"
		"	-resultsetbuffersize rows\n"
		"				Fetch result sets using the specified number of\n"
		"				rows at once.\n"
		"\n"
		"Examples:\n"
		"\n"
		"Interactive session with server at svr:9000 as usr/pwd.\n"
		"\n"
		"	%s -host svr -port 9000 -user usr -password pwd\n"
		"\n"
		"Interactive session with local server on socket /tmp/svr.sock as usr/pwd.\n"
		"\n"
		"	%s -socket /tmp/svr.sock -user usr -password pwd\n"
		"\n"
		"Interactive session using connection info and credentials from an instance\n"
		"defined in the default configuration.\n"
		"	%s -id myinst\n"
		"\n"
		"Interactive session using connection info and credentials from an instance\n"
		"defined in the config file ./myconfig.conf\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst\n"
		"\n"
		"Non-interactive session, running commands from ./script.sql\n"
		"\n"
		"	%s -id myinst -script ./script.sql\n"
		"\n"
		"Non-interactive session, running query \"select * from mytable\" with csv output.\n"
		"\n"
		"	%s -id myinst -command \"select * from mytable\" -quiet -format csv\n"
		"\n",
		progname,SQL_RELAY,progname,progname,progname,progname,
		progname,progname,progname,progname,progname);
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

	int32_t	exitcode=0;
	{
		sqlrsh	s;
		exitcode=!s.execute(argc,argv);
	}
	process::exit(exitcode);
}
