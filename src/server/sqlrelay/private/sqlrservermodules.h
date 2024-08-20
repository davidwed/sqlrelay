// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

class SQLRSERVER_DLLSPEC sqlrmoduleplugin {
	public:
		sqlrservermodule	*m;
		dynamiclib		*dl;
		const char		*module;
};

class SQLRSERVER_DLLSPEC sqlrservermodules : public sqlrserverbase {
	public:
		sqlrservermodules(sqlrservercontroller *cont);
		virtual	~sqlrservermodules();

		bool	isModuleDisabled(domnode *parameters);

		virtual bool	load(domnode *parameters);
		virtual void	unload();
		virtual void	endTransaction(bool commit);
		virtual void	endSession();

	protected:
		virtual void	loadModule(domnode *parameters);
		const char	*getModuleName(domnode *parameters);

		singlylinkedlist< sqlrmoduleplugin * >	blist;
		singlylinkedlist< sqlrmoduleplugin * >	alist;

		sqlrservercontroller	*cont;
};

class SQLRSERVER_DLLSPEC sqlrprotocols : public sqlrservermodules {
	public:
		sqlrprotocols(sqlrservercontroller *cont);
		~sqlrprotocols();

		bool	load(domnode *listeners);
		sqlrprotocol	*getProtocol(uint16_t port);

	private:
		void	loadModule(domnode *parameters, uint16_t index);

		sqlrprotocolsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrauths : public sqlrservermodules {
	public:
		sqlrauths(sqlrservercontroller *cont);
		~sqlrauths();

		bool	load(domnode *parameters);
		const char	*auth(sqlrcredentials *cred);

	private:
		void	loadModule(domnode *parameters);

		sqlrauthsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrloggers : public sqlrservermodules {
	public:
		sqlrloggers(sqlrpaths *sqlrpth);
		~sqlrloggers();

		void	init(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon);
		void	run(sqlrlistener *sqlrl,
				sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrloglevel_t level,
				sqlrevent_t event,
				const char *info);

	private:
		void	loadModule(domnode *parameters);

		sqlrloggersprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrnotifications : public sqlrservermodules {
	public:
		sqlrnotifications(sqlrpaths *sqlrpth);
		~sqlrnotifications();

		void	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrevent_t event,
					const char *info);

	private:
		void	loadModule(domnode *parameters);

		sqlrnotificationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrschedules : public sqlrservermodules {
	public:
		sqlrschedules(sqlrservercontroller *cont);
		~sqlrschedules();

		bool	allowed(sqlrserverconnection *sqlrcon,
						const char *user);

	private:
		void	loadModule(domnode *parameters);

		sqlrschedulesprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrdirectives : public sqlrservermodules {
	public:
		sqlrdirectives(sqlrservercontroller *cont);
		~sqlrdirectives();

		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);

	private:
		void	loadModule(domnode *parameters);

		sqlrdirectivesprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrquerytranslations : public sqlrservermodules {
	public:
		sqlrquerytranslations(sqlrservercontroller *cont);
		~sqlrquerytranslations();

		bool	load(domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						uint32_t querysize,
						stringbuffer *translatedquery);

		const char	*getError();
		bool	getUseOriginalOnError();

	private:
		void	loadModule(domnode *parameters);

		sqlrdatabaseobject *createDatabaseObject(
						const char *database,
						const char *schema,
						const char *object,
						const char *dependency);

		void	setReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				sqlrdatabaseobject *oldobject,
				const char *newobject);
		bool	getReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject,
				const char **newobject);
		bool	removeReplacement(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject);

		sqlrquerytranslationsprivate	*pvt;
};

typedef sqlrquerytranslations sqlrtranslations;

class SQLRSERVER_DLLSPEC sqlrfilters : public sqlrservermodules {
	public:
		sqlrfilters(sqlrservercontroller *cont);
		~sqlrfilters();

		bool	load(domnode *parameters);
		bool	runBeforeFilters(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						const char **err,
						int64_t *errn);
		bool	runAfterFilters(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						sqlrparser *sqlrp,
						const char *query,
						const char **err,
						int64_t *errn);

	private:
		void	loadModule(domnode *parameters,
				singlylinkedlist< sqlrmoduleplugin * > *list);
		bool	run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrparser *sqlrp,
				const char *query,
				const char **err,
				int64_t *errn,
				singlylinkedlist< sqlrmoduleplugin * > *list);

		sqlrfiltersprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrbindvariabletranslations :
						public sqlrservermodules {
	public:
		sqlrbindvariabletranslations(sqlrservercontroller *cont);
		~sqlrbindvariabletranslations();

		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur);
		const char	*getError();

	private:
		void	loadModule(domnode *parameters);

		sqlrbindvariabletranslationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrresultsettranslations : public sqlrservermodules {
	public:
		sqlrresultsettranslations(sqlrservercontroller *cont);
		~sqlrresultsettranslations();

		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *fieldname,
						uint32_t fieldindex,
						const char **field,
						uint64_t *fieldsize);

		const char	*getError();

	private:
		void	loadModule(domnode *parameters);

		sqlrresultsettranslationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowtranslations :
						public sqlrservermodules {
	public:
		sqlrresultsetrowtranslations(sqlrservercontroller *cont);
		~sqlrresultsetrowtranslations();

		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char ***fields,
						uint64_t **fieldsizes);

		const char	*getError();

	private:
		void	loadModule(domnode *parameters);

		sqlrresultsetrowtranslationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrresultsetrowblocktranslations :
						public sqlrservermodules {
	public:
		sqlrresultsetrowblocktranslations(sqlrservercontroller *cont);
		~sqlrresultsetrowblocktranslations();

		bool	load(domnode *parameters);

		uint64_t	getRowBlockCount();

		bool	setRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char * const *fields,
					uint64_t *fieldsizes,
					bool *lobs,
					bool *nulls);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames);
		bool	getRow(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***fields,
					uint64_t **fieldsizes,
					bool **lobs,
					bool **nulls);

		const char	*getError();

	private:
		void	loadModule(domnode *parameters);

		sqlrresultsetrowblocktranslationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrresultsetheadertranslations :
						public sqlrservermodules {
	public:
		sqlrresultsetheadertranslations(sqlrservercontroller *cont);
		~sqlrresultsetheadertranslations();

		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamesizes,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamesizes,
					uint32_t **columnsizes,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablesizes);

		const char	*getError();

	private:
		void	loadModule(domnode *parameters);

		sqlrresultsetheadertranslationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrerrortranslations : public sqlrservermodules {
	public:
		sqlrerrortranslations(sqlrservercontroller *cont);
		~sqlrerrortranslations();

		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					int64_t errornumber,
					const char *error,
					uint32_t errorsize,
					int64_t *translatederrornumber,
					stringbuffer *translatederror);

		const char	*getError();

	private:
		void	loadModule(domnode *parameters);

		sqlrerrortranslationsprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrtriggers : public sqlrservermodules {
	public:
		sqlrtriggers(sqlrservercontroller *cont);
		~sqlrtriggers();

		bool	load(domnode *parameters);
		bool	runBeforeTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		bool	runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);

	private:
		void	loadModule(domnode *trigger, sqlrmoduleplugin **plugin);
		bool	runBefore(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				singlylinkedlist< sqlrmoduleplugin * > *list);
		bool	runAfter(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				singlylinkedlist< sqlrmoduleplugin * > *list);

		sqlrtriggersprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrqueries : public sqlrservermodules {
	public:
		sqlrqueries(sqlrservercontroller *cont);
		~sqlrqueries();

		sqlrquerycursor	*match(sqlrserverconnection *sqlrcon,
						const char *querystring,
						uint32_t querysize,
						uint16_t id);

	private:
		void	loadModule(domnode *parameters);

		sqlrqueriesprivate	*pvt;
};

class SQLRSERVER_DLLSPEC sqlrmoduledatas : public sqlrservermodules {
	public:
		sqlrmoduledatas(sqlrservercontroller *cont);
		~sqlrmoduledatas();

		sqlrmoduledata	*getModuleData(const char *id);

		void	closeResultSet(sqlrservercursor *sqlrcur);

	private:
		void	loadModule(domnode *parameters);

		sqlrmoduledatasprivate	*pvt;
};
