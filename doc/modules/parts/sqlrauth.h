class SQLRSERVER_DLLSPEC sqlrcredentials {
	public:
			sqlrcredentials();
		virtual	~sqlrcredentials();
		virtual const char	*getType()=0;
};

class SQLRSERVER_DLLSPEC sqlruserpasswordcredentials : public sqlrcredentials {
	public:
			sqlruserpasswordcredentials();
		virtual	~sqlruserpasswordcredentials();
		const char	*getType();

		void	setUser(const char *user);
		void	setPassword(const char *password);

		const char	*getUser();
		const char	*getPassword();
	private:
		const char	*user;
		const char	*password;
};

class SQLRSERVER_DLLSPEC sqlrauth {
	public:
			sqlrauth(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe,
					bool debug);
		virtual	~sqlrauth();
		virtual	const char	*auth(sqlrserverconnection *sqlrcon,
							sqlrcredentials *cred);
	protected:
		xmldomnode		*parameters;
		sqlrpwdencs		*sqlrpe;
		bool			debug;
};
