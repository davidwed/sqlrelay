// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrpwdenc.h>
#include <rudiments/character.h>

using namespace rudiments;

class rot : public sqlrpwdenc {
	public:
			rot(xmldomnode *parameters);
		char	*encrypt(const char *value);
		char	*decrypt(const char *value);
	private:
		char	*rotate(const char *value, int64_t count);
};

rot::rot(xmldomnode *parameters) : sqlrpwdenc(parameters) {
}

char *rot::encrypt(const char *value) {
	return rotate(value,charstring::toInteger(
				parameters->getAttributeValue("count")));
}

char *rot::decrypt(const char *value) {
	return rotate(value,-charstring::toInteger(
				parameters->getAttributeValue("count")));
}

char *rot::rotate(const char *value, int64_t count) {

	// get the size of the value passed in and
	// allocate space for the return value
	size_t	len=charstring::length(value);
	char	*retval=new char[len+1];

	// for each character in the value...
	// (by using <= len, we'll catch the NULL terminator)
	for (size_t i=0; i<=len; i++) {

		// if it's an alphabetical or numeric character,
		// rotate it, otherwise just copy it out
		if (character::isAlphabetical(value[i])) {
			char	start='a';
			if (character::isUpperCase(value[i])) {
				start='A';
			}
			retval[i]=((value[i]-start+count)%26)+start;
		} else if (character::isDigit(value[i])) {
			retval[i]=((value[i]-'0'+count)%10)+'0';
		} else {
			retval[i]=value[i];
		}
	}
	return retval;
}

extern "C" {
	sqlrpwdenc *new_rot(xmldomnode *parameters) {
		return new rot(parameters);
	}
}
