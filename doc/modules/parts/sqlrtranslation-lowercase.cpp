#include <sqlrelay/sqlrserver.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/character.h>

class SQLRSERVER_DLLSPEC lowercase : public sqlrtranslation {
	public:
			lowercase(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);
};

lowercase::lowercase(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool lowercase::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery) {
	for (const char *c=query; *c; c++) {
		translatedquery->append(character::toLower(*c));
	}
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrtranslation *new_sqlrtranslation_lowercase(
							sqlrtranslations *sqlts,
							xmldomnode *parameters,
							bool debug) {
		return new lowercase(sqlts,parameters,debug);
	}
}
