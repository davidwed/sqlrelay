// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#ifdef __CYGWIN__
	#define __int64 long long
#endif

#include <sqlrelay/sqlrclient.h>
#include <com_firstworks_sqlrelay_SQLRCursor.h>

#ifdef __cplusplus
extern "C" {
#endif

char	*curGetStringUTFChars(JNIEnv *env, jstring string, jboolean *modifier) {
	if (string) {
		return (char *)env->GetStringUTFChars(string,modifier);
	}
	return (char *)NULL;
}

void	curReleaseStringUTFChars(JNIEnv *env, jstring string, char *chararray) {
	if (string) {
		env->ReleaseStringUTFChars(string,chararray);
	}
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    alloc
 * Signature: (LSQLRConnection;)V
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_alloc
  (JNIEnv *env, jobject self, jint con) {
	sqlrcursor	*cur=new sqlrcursor((sqlrconnection *)con);
	cur->copyReferences();
	return (jint)cur;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    delete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_delete
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	delete cur;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    setResultSetBufferSize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_setResultSetBufferSize
  (JNIEnv *env, jobject self, jint rows) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->setResultSetBufferSize((int)rows);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getResultSetBufferSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getResultSetBufferSize
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->getResultSetBufferSize();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    dontGetColumnInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_dontGetColumnInfo
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->dontGetColumnInfo();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnInfo
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->getColumnInfo();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    mixedCaseColumnNames
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_mixedCaseColumnNames
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->mixedCaseColumnNames();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    upperCaseColumnNames
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_upperCaseColumnNames
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->upperCaseColumnNames();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    lowerCaseColumnNames
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_lowerCaseColumnNames
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->lowerCaseColumnNames();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    cacheToFile
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_cacheToFile
  (JNIEnv *env, jobject self, jstring filename) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	cur->cacheToFile(filenamestring);
	curReleaseStringUTFChars(env,filename,filenamestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    setCacheTtl
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_setCacheTtl
  (JNIEnv *env, jobject self, jint ttl) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->setCacheTtl((int)ttl);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getCacheFileName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getCacheFileName
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return env->NewStringUTF(cur->getCacheFileName());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    cacheOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_cacheOff
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->cacheOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    sendQuery
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_sendQuery__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring query) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*querystring=curGetStringUTFChars(env,query,0);
	jboolean	retval=
		(cur->sendQuery(querystring))?JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,query,querystring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    sendQuery
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_sendQuery__Ljava_lang_String_2I
  (JNIEnv *env, jobject self, jstring query, jint length) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*querystring=curGetStringUTFChars(env,query,0);
	jboolean	retval=
		(cur->sendQuery(querystring,(int)length))?JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,query,querystring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    sendFileQuery
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_sendFileQuery
  (JNIEnv *env, jobject self, jstring path, jstring filename) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*pathstring=curGetStringUTFChars(env,path,0);
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=
		(cur->sendFileQuery(pathstring,filenamestring))?
						JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,path,pathstring);
	curReleaseStringUTFChars(env,filename,filenamestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    prepareQuery
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_prepareQuery__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring query) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*querystring=curGetStringUTFChars(env,query,0);
	cur->prepareQuery(querystring);
	curReleaseStringUTFChars(env,query,querystring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    prepareQuery
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_prepareQuery__Ljava_lang_String_2I
  (JNIEnv *env, jobject self, jstring query, jint length) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*querystring=curGetStringUTFChars(env,query,0);
	cur->prepareQuery(querystring,(int)length);
	curReleaseStringUTFChars(env,query,querystring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    prepareFileQuery
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_prepareFileQuery
  (JNIEnv *env, jobject self, jstring path, jstring filename) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*pathstring=curGetStringUTFChars(env,path,0);
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=
		(cur->prepareFileQuery(pathstring,filenamestring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,path,pathstring);
	curReleaseStringUTFChars(env,filename,filenamestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    clearBinds
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_clearBinds
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->clearBinds();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitution
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitution__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring variable, jstring value) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	cur->substitution(variablestring,valuestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	curReleaseStringUTFChars(env,value,valuestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitution
 * Signature: (Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitution__Ljava_lang_String_2J
  (JNIEnv *env, jobject self, jstring variable, jlong value) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->substitution(variablestring,(long)value);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitution
 * Signature: (Ljava/lang/String;DII)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitution__Ljava_lang_String_2DII
  (JNIEnv *env, jobject self, jstring variable, jdouble value, jint precision, jint scale) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->substitution(variablestring,(double)value,
				(unsigned short)precision,
				(unsigned short)scale);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    countBindVariables
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_countBindVariables
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jshort)cur->countBindVariables();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBind
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBind__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring variable, jstring value) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	cur->inputBind(variablestring,valuestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	curReleaseStringUTFChars(env,value,valuestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBind
 * Signature: (Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBind__Ljava_lang_String_2J
  (JNIEnv *env, jobject self, jstring variable, jlong value) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->inputBind(variablestring,(unsigned long)value);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBind
 * Signature: (Ljava/lang/String;DII)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBind__Ljava_lang_String_2DII
  (JNIEnv *env, jobject self, jstring variable, jdouble value, jint precision, jint scale) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->inputBind(variablestring,(double)value,
				(unsigned short)precision,
				(unsigned short)scale);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBindBlob
 * Signature: (Ljava/lang/String;[BJ)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBindBlob
  (JNIEnv *env, jobject self, jstring variable, jbyteArray value, jlong size) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	if (size) {
		jbyte	*valuebytes=new jbyte[size];
		env->GetByteArrayRegion(value,0,size,valuebytes);
		cur->inputBindBlob(variablestring,(char *)valuebytes,
						(unsigned long)size);
		delete[] valuebytes;
	} else {
		cur->inputBindBlob(variablestring,NULL,0);
	}
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBindClob
 * Signature: (Ljava/lang/String;Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBindClob
  (JNIEnv *env, jobject self, jstring variable, jstring value, jlong size) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	cur->inputBindClob(variablestring,valuestring,(unsigned long)size);
	curReleaseStringUTFChars(env,variable,variablestring);
	curReleaseStringUTFChars(env,value,valuestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBind
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBind
  (JNIEnv *env, jobject self, jstring variable, jint bufferlength) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->defineOutputBind(variablestring,(int)bufferlength);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindBlob
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindBlob
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->defineOutputBindBlob(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindClob
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindClob
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->defineOutputBindClob(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindCursor
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindCursor
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	cur->defineOutputBindCursor(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitutions
 * Signature: ([Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitutions___3Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *env, jobject self, jobjectArray variables, jobjectArray values) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	jsize		variableslen=env->GetArrayLength(variables);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		jstring	value=
			(jstring)env->GetObjectArrayElement(values,i);
		char	*variablestring=curGetStringUTFChars(env,variable,0);
		char	*valuestring=curGetStringUTFChars(env,value,0);
		cur->substitution(variablestring,valuestring);
		curReleaseStringUTFChars(env,variable,variablestring);
		curReleaseStringUTFChars(env,value,valuestring);
	}
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitutions
 * Signature: ([Ljava/lang/String;[J)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitutions___3Ljava_lang_String_2_3J
  (JNIEnv *env, jobject self, jobjectArray variables, jlongArray values) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	jsize		variableslen=env->GetArrayLength(variables);
	jlong		*valuesarray=env->GetLongArrayElements(values,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		cur->substitution(variablestring,valuesarray[i]);
		curReleaseStringUTFChars(env,variable,variablestring);
	}
	env->ReleaseLongArrayElements(values,valuesarray,0);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitutions
 * Signature: ([Ljava/lang/String;[D[I[I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitutions___3Ljava_lang_String_2_3D_3I_3I
  (JNIEnv *env, jobject self, jobjectArray variables, jdoubleArray values, jintArray precisions, jintArray scales) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	jsize		variableslen=env->GetArrayLength(variables);
	jdouble		*valuesarray=env->GetDoubleArrayElements(values,0);
	jint		*precisionsarray=env->GetIntArrayElements(precisions,0);
	jint		*scalesarray=env->GetIntArrayElements(scales,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		cur->substitution(variablestring,valuesarray[i],
					(unsigned short)precisionsarray[i],
					(unsigned short)scalesarray[i]);
		curReleaseStringUTFChars(env,variable,variablestring);
	}
	env->ReleaseDoubleArrayElements(values,valuesarray,0);
	env->ReleaseIntArrayElements(precisions,precisionsarray,0);
	env->ReleaseIntArrayElements(scales,scalesarray,0);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBinds
 * Signature: ([Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBinds___3Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *env, jobject self, jobjectArray variables, jobjectArray values) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	jsize		variableslen=env->GetArrayLength(variables);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		jstring	value=
			(jstring)env->GetObjectArrayElement(values,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		char	*valuestring=
			curGetStringUTFChars(env,value,0);
		cur->inputBind(variablestring,valuestring);
		curReleaseStringUTFChars(env,variable,variablestring);
		curReleaseStringUTFChars(env,value,valuestring);
	}
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBinds
 * Signature: ([Ljava/lang/String;[J)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBinds___3Ljava_lang_String_2_3J
  (JNIEnv *env, jobject self, jobjectArray variables, jlongArray values) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	jsize		variableslen=env->GetArrayLength(variables);
	jlong		*valuesarray=env->GetLongArrayElements(values,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		cur->inputBind(variablestring,(unsigned long)valuesarray[i]);
		curReleaseStringUTFChars(env,variable,variablestring);
	}
	env->ReleaseLongArrayElements(values,valuesarray,0);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBinds
 * Signature: ([Ljava/lang/String;[D[I[I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBinds___3Ljava_lang_String_2_3D_3I_3I
  (JNIEnv *env, jobject self, jobjectArray variables, jdoubleArray values, jintArray precisions, jintArray scales) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	jsize		variableslen=env->GetArrayLength(variables);
	jdouble		*valuesarray=env->GetDoubleArrayElements(values,0);
	jint		*precisionsarray=env->GetIntArrayElements(precisions,0);
	jint		*scalesarray=env->GetIntArrayElements(scales,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		cur->inputBind(variablestring,valuesarray[i],
					(unsigned short)precisionsarray[i],
					(unsigned short)scalesarray[i]);
		curReleaseStringUTFChars(env,variable,variablestring);
	}
	env->ReleaseDoubleArrayElements(values,valuesarray,0);
	env->ReleaseIntArrayElements(precisions,precisionsarray,0);
	env->ReleaseIntArrayElements(scales,scalesarray,0);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    validateBinds
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_validateBinds
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->validateBinds();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    executeQuery
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_executeQuery
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->executeQuery())?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    fetchFromBindCursor
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_fetchFromBindCursor
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->fetchFromBindCursor())?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBind
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBind
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jstring	retval=env->NewStringUTF(cur->getOutputBind(variablestring));
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindAsLong
 * Signature: (Ljava/lang/String;)J;
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindAsLong
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jlong	retval=(jlong)cur->getOutputBindAsLong(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindAsDouble
 * Signature: (Ljava/lang/String;)J;
 */
JNIEXPORT jdouble JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindAsDouble
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jdouble	retval=(jdouble)cur->getOutputBindAsDouble(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindAsByteArray
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindAsByteArray
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	long	length=cur->getOutputBindLength(variablestring);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
				(jbyte *)cur->getOutputBind(variablestring));
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindLength
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindLength
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jlong	retval=(jlong)cur->getOutputBindLength(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindCursor
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindCursorInternal
  (JNIEnv *env, jobject self, jstring variable) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	sqlrcursor	*bindcur=cur->getOutputBindCursor(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	return (jint)bindcur;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    openCachedResultSet
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_openCachedResultSet
  (JNIEnv *env, jobject self, jstring filename) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=
		(cur->openCachedResultSet(filenamestring))?JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,filename,filenamestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    colCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_colCount
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->colCount();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    rowCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_rowCount
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->rowCount();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    totalRows
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_totalRows
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->totalRows();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    affectedRows
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_affectedRows
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->affectedRows();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    firstRowIndex
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_firstRowIndex
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->firstRowIndex();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    endOfResultSet
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_endOfResultSet
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->endOfResultSet())?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    errorMessage
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_errorMessage
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return env->NewStringUTF(cur->errorMessage());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getNullsAsEmptyStrings
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getNullsAsEmptyStrings
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->getNullsAsEmptyStrings();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getNullsAsNulls
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getNullsAsNulls
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->getNullsAsNulls();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getField
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getField__II
  (JNIEnv *env, jobject self, jint row, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return env->NewStringUTF(cur->getField((int)row,(int)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getField
 * Signature: (ILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getField__ILjava_lang_String_2
  (JNIEnv *env, jobject self, jint row, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jstring	retval=env->NewStringUTF(cur->getField((int)row,colstring));
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsLong
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsLong__II
  (JNIEnv *env, jobject self, jint row, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jlong)cur->getFieldAsLong((int)row,(int)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsLong
 * Signature: (ILjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsLong__ILjava_lang_String_2
  (JNIEnv *env, jobject self, jint row, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)cur->getFieldAsLong((int)row,colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsDouble
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsDouble__II
  (JNIEnv *env, jobject self, jint row, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jdouble)cur->getFieldAsDouble((int)row,(int)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsDouble
 * Signature: (ILjava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsDouble__ILjava_lang_String_2
  (JNIEnv *env, jobject self, jint row, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jdouble	retval=(jlong)cur->getFieldAsDouble((int)row,colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsByteArray
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsByteArray__II
  (JNIEnv *env, jobject self, jint row, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	long	length=cur->getFieldLength((int)row,(int)col);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
				(jbyte *)cur->getField((int)row,(int)col));
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsByteArray
 * Signature: (ILjava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsByteArray__ILjava_lang_String_2
  (JNIEnv *env, jobject self, jint row, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	long	length=cur->getFieldLength((int)row,colstring);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
				(jbyte *)cur->getField((int)row,colstring));
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldLength
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldLength__II
  (JNIEnv *env, jobject self, jint row, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jlong)(cur->getFieldLength((int)row,(int)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getRow
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getRow
  (JNIEnv *env, jobject self, jint row) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	int	colcount=cur->colCount();
	jobjectArray	retarray=env->NewObjectArray(colcount,
					env->FindClass("java/lang/String"),
					env->NewStringUTF(""));
	char	**field=cur->getRow((int)row);
	for (int i=0; i<colcount; i++) {
		env->SetObjectArrayElement(retarray,i,
					env->NewStringUTF(field[i]));
	}
	return retarray;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getRowLengths
 * Signature: (I)[J
 */
JNIEXPORT jlongArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getRowLengths
  (JNIEnv *env, jobject self, jint row) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	int	colcount=cur->colCount();
	long	*rowlengths=cur->getRowLengths((int)row);
	if (!rowlengths) {
		return 0;
	}
	jlong	*jrowlengths=new jlong[colcount];
	for (int i=0; i<colcount; i++) {
		jrowlengths[i]=(jlong)rowlengths[i];
	}
	jlongArray	retarray=env->NewLongArray(colcount);
	env->SetLongArrayRegion(retarray,0,colcount,jrowlengths);
	delete jrowlengths;
	return retarray;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnNames
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnNames
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	int	colcount=cur->colCount();
	jobjectArray	retarray=env->NewObjectArray(colcount,
					env->FindClass("java/lang/String"),
					env->NewStringUTF(""));
	char	**colnames=cur->getColumnNames();
	if (!colnames) {
		return 0;
	}
	for (int i=0; i<colcount; i++) {
		env->SetObjectArrayElement(retarray,i,
				env->NewStringUTF(colnames[i]));
	}
	return retarray;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldLength
 * Signature: (ILjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldLength__ILjava_lang_String_2
  (JNIEnv *env, jobject self, jint row, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)cur->getFieldLength((int)row,colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnName
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return env->NewStringUTF(cur->getColumnName((int)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnType
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnType__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return env->NewStringUTF(cur->getColumnType((int)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnType
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnType__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jstring	retval=env->NewStringUTF(cur->getColumnType(colstring));
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnPrecision
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnPrecision__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jlong)cur->getColumnPrecision((int)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnPrecision
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnPrecision__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)cur->getColumnPrecision(colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnScale
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnScale__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jlong)cur->getColumnScale((int)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnScale
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnScale__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)cur->getColumnScale(colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsNullable
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsNullable__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsNullable((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsNullable
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsNullable__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsNullable(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsPrimaryKey
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsPrimaryKey__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsPrimaryKey((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsPrimaryKey
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsPrimaryKey__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsPrimaryKey(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsUnique
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsUnique__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsUnique((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsUnique
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsUnique__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsUnique(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsPartOfKey
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsPartOfKey__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsUnique((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsPartOfKey
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsPartOfKey__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsPartOfKey(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsUnsigned
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsUnsigned__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsUnique((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsUnsigned
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsUnsigned__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsUnsigned(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsZeroFilled
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsZeroFilled__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsUnique((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsZeroFilled
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsZeroFilled__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsZeroFilled(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsBinary
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsBinary__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsUnique((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsBinary
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsBinary__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsBinary(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsAutoIncrement
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsAutoIncrement__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (cur->getColumnIsUnique((int)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsAutoIncrement
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsAutoIncrement__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(cur->getColumnIsAutoIncrement(colstring))?
							JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnLength
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnLength__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->getColumnLength((int)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnLength
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnLength__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jint	retval=(jint)cur->getColumnLength(colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getLongest
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getLongest__I
  (JNIEnv *env, jobject self, jint col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->getLongest((int)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getLongest
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getLongest__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*colstring=curGetStringUTFChars(env,col,0);
	jint	retval=(jint)cur->getLongest(colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getResultSetId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getResultSetId
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jint)cur->getResultSetId();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    suspendResultSet
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_suspendResultSet
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	cur->suspendResultSet();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    resumeResultSet
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_resumeResultSet
  (JNIEnv *env, jobject self, jint id) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	return (jboolean)cur->resumeResultSet((int)id);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    resumeCachedResultSet
 * Signature: (ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_resumeCachedResultSet
  (JNIEnv *env, jobject self, jint id, jstring filename) {
	jclass		cls=env->GetObjectClass(self);
	sqlrcursor	*cur=(sqlrcursor *)env->GetIntField(self,
				env->GetFieldID(cls,"cursor","I"));
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=
		(jboolean)cur->resumeCachedResultSet((int)id,filenamestring);
	curReleaseStringUTFChars(env,filename,filenamestring);
	return retval;
}

#ifdef __cplusplus
}
#endif
