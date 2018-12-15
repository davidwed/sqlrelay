// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#ifdef __CYGWIN__
	#include <windows.h>
#endif

#include <sqlrelay/sqlrclient.h>
#include <com_firstworks_sqlrelay_SQLRCursor.h>
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

static sqlrcursor *getSqlrCursor(JNIEnv *env, jobject self) {
	return reinterpret_cast<sqlrcursor *>(
			env->GetLongField(self,
				env->GetFieldID(env->GetObjectClass(self),
				"cursor","J")));
}

static char *curGetStringUTFChars(JNIEnv *env, jstring string,
						jboolean *modifier) {
	if (string) {
		return (char *)env->GetStringUTFChars(string,modifier);
	}
	return (char *)NULL;
}

static void curReleaseStringUTFChars(JNIEnv *env, jstring string,
						char *chararray) {
	if (string && chararray) {
		env->ReleaseStringUTFChars(string,chararray);
	}
}

static jstring curNewStringUTF(JNIEnv *env, const char *string) {
	return (string)?env->NewStringUTF(string):NULL;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    alloc
 * Signature: (LSQLRConnection;)V
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_alloc
  (JNIEnv *env, jobject self, jlong con) {
	sqlrconnection	*conn=reinterpret_cast<sqlrconnection *>(con);
	sqlrcursor	*cur=new sqlrcursor(conn,true);
	return reinterpret_cast<jlong>(cur);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    delete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_delete
  (JNIEnv *env, jobject self) {
	delete getSqlrCursor(env,self);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    setResultSetBufferSize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_setResultSetBufferSize
  (JNIEnv *env, jobject self, jlong rows) {
	getSqlrCursor(env,self)->setResultSetBufferSize((uint64_t)rows);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getResultSetBufferSize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getResultSetBufferSize
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrCursor(env,self)->getResultSetBufferSize();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    dontGetColumnInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_dontGetColumnInfo
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->dontGetColumnInfo();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnInfo
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->getColumnInfo();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    mixedCaseColumnNames
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_mixedCaseColumnNames
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->mixedCaseColumnNames();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    upperCaseColumnNames
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_upperCaseColumnNames
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->upperCaseColumnNames();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    lowerCaseColumnNames
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_lowerCaseColumnNames
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->lowerCaseColumnNames();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    cacheToFile
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_cacheToFile
  (JNIEnv *env, jobject self, jstring filename) {
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	getSqlrCursor(env,self)->cacheToFile(filenamestring);
	curReleaseStringUTFChars(env,filename,filenamestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    setCacheTtl
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_setCacheTtl
  (JNIEnv *env, jobject self, jint ttl) {
	getSqlrCursor(env,self)->setCacheTtl((uint32_t)ttl);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getCacheFileName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getCacheFileName
  (JNIEnv *env, jobject self) {
	return curNewStringUTF(env,getSqlrCursor(env,self)->getCacheFileName());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    cacheOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_cacheOff
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->cacheOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getDatabaseList
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getDatabaseList
  (JNIEnv *env, jobject self, jstring wild) {
	char	*wildstring=curGetStringUTFChars(env,wild,0);
	jboolean	retval=getSqlrCursor(env,self)->
					getDatabaseList(wildstring);
	curReleaseStringUTFChars(env,wild,wildstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getTableList
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getTableList
  (JNIEnv *env, jobject self, jstring wild) {
	char	*wildstring=curGetStringUTFChars(env,wild,0);
	jboolean	retval=getSqlrCursor(env,self)->
					getTableList(wildstring);
	curReleaseStringUTFChars(env,wild,wildstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnList
 * Signature: (Ljava/lang/String;Ljava/lang/String)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnList
  (JNIEnv *env, jobject self, jstring table, jstring wild) {
	char	*tablestring=curGetStringUTFChars(env,table,0);
	char	*wildstring=curGetStringUTFChars(env,wild,0);
	jboolean	retval=getSqlrCursor(env,self)->
					getColumnList(tablestring,wildstring);
	curReleaseStringUTFChars(env,table,tablestring);
	curReleaseStringUTFChars(env,wild,wildstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    sendQuery
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_sendQuery__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring query) {
	char	*querystring=curGetStringUTFChars(env,query,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				sendQuery(querystring))?
				JNI_TRUE:JNI_FALSE;
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
	char	*querystring=curGetStringUTFChars(env,query,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				sendQuery(querystring,(uint32_t)length))?
				JNI_TRUE:JNI_FALSE;
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
	char	*pathstring=curGetStringUTFChars(env,path,0);
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				sendFileQuery(pathstring,filenamestring))?
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
	char	*querystring=curGetStringUTFChars(env,query,0);
	getSqlrCursor(env,self)->prepareQuery(querystring);
	curReleaseStringUTFChars(env,query,querystring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    prepareQuery
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_prepareQuery__Ljava_lang_String_2I
  (JNIEnv *env, jobject self, jstring query, jint length) {
	char	*querystring=curGetStringUTFChars(env,query,0);
	getSqlrCursor(env,self)->prepareQuery(querystring,(uint32_t)length);
	curReleaseStringUTFChars(env,query,querystring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    prepareFileQuery
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_prepareFileQuery
  (JNIEnv *env, jobject self, jstring path, jstring filename) {
	char	*pathstring=curGetStringUTFChars(env,path,0);
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				prepareFileQuery(pathstring,filenamestring))?
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
	getSqlrCursor(env,self)->clearBinds();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitution
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitution__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring variable, jstring value) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	getSqlrCursor(env,self)->substitution(variablestring,valuestring);
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
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->substitution(variablestring,(int64_t)value);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitution
 * Signature: (Ljava/lang/String;DII)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitution__Ljava_lang_String_2DII
  (JNIEnv *env, jobject self, jstring variable, jdouble value, jint precision, jint scale) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->substitution(variablestring,(double)value,
				(uint32_t)precision,
				(uint32_t)scale);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    countBindVariables
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_countBindVariables
  (JNIEnv *env, jobject self) {
	return (jshort)getSqlrCursor(env,self)->countBindVariables();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBind
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBind__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring variable, jstring value) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	getSqlrCursor(env,self)->inputBind(variablestring,valuestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	curReleaseStringUTFChars(env,value,valuestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBind
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBind__Ljava_lang_String_2Ljava_lang_String_2I
  (JNIEnv *env, jobject self, jstring variable, jstring value, jint length) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	getSqlrCursor(env,self)->inputBind(variablestring,
					valuestring,(uint32_t)length);
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
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->inputBind(variablestring,(int64_t)value);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBind
 * Signature: (Ljava/lang/String;DII)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBind__Ljava_lang_String_2DII
  (JNIEnv *env, jobject self, jstring variable, jdouble value, jint precision, jint scale) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->inputBind(variablestring,(double)value,
				(uint32_t)precision,
				(uint32_t)scale);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    inputBindBlob
 * Signature: (Ljava/lang/String;[BJ)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_inputBindBlob
  (JNIEnv *env, jobject self, jstring variable, jbyteArray value, jlong size) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	if (size) {
		jbyte	*valuebytes=new jbyte[size];
		env->GetByteArrayRegion(value,0,size,valuebytes);
		getSqlrCursor(env,self)->inputBindBlob(variablestring,
						(char *)valuebytes,
						(uint32_t)size);
		delete[] valuebytes;
	} else {
		getSqlrCursor(env,self)->inputBindBlob(variablestring,NULL,0);
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
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	char	*valuestring=curGetStringUTFChars(env,value,0);
	getSqlrCursor(env,self)->inputBindClob(variablestring,valuestring,(uint32_t)size);
	curReleaseStringUTFChars(env,variable,variablestring);
	curReleaseStringUTFChars(env,value,valuestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindString
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindString
  (JNIEnv *env, jobject self, jstring variable, jint bufferlength) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->defineOutputBindString(variablestring,(uint32_t)bufferlength);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindInteger
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindInteger
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->defineOutputBindInteger(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindDouble
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindDouble
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->defineOutputBindDouble(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindBlob
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindBlob
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->defineOutputBindBlob(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindClob
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindClob
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->defineOutputBindClob(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    defineOutputBindCursor
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_defineOutputBindCursor
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	getSqlrCursor(env,self)->defineOutputBindCursor(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    substitutions
 * Signature: ([Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_substitutions___3Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *env, jobject self, jobjectArray variables, jobjectArray values) {
	jsize		variableslen=env->GetArrayLength(variables);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		jstring	value=
			(jstring)env->GetObjectArrayElement(values,i);
		char	*variablestring=curGetStringUTFChars(env,variable,0);
		char	*valuestring=curGetStringUTFChars(env,value,0);
		getSqlrCursor(env,self)->substitution(variablestring,valuestring);
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
	jsize		variableslen=env->GetArrayLength(variables);
	jlong		*valuesarray=env->GetLongArrayElements(values,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		getSqlrCursor(env,self)->substitution(variablestring,valuesarray[i]);
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
	jsize		variableslen=env->GetArrayLength(variables);
	jdouble		*valuesarray=env->GetDoubleArrayElements(values,0);
	jint		*precisionsarray=env->GetIntArrayElements(precisions,0);
	jint		*scalesarray=env->GetIntArrayElements(scales,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		getSqlrCursor(env,self)->substitution(variablestring,valuesarray[i],
					(uint32_t)precisionsarray[i],
					(uint32_t)scalesarray[i]);
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
		getSqlrCursor(env,self)->inputBind(variablestring,valuestring);
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
	jsize		variableslen=env->GetArrayLength(variables);
	jlong		*valuesarray=env->GetLongArrayElements(values,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		getSqlrCursor(env,self)->inputBind(variablestring,(int64_t)valuesarray[i]);
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
	jsize		variableslen=env->GetArrayLength(variables);
	jdouble		*valuesarray=env->GetDoubleArrayElements(values,0);
	jint		*precisionsarray=env->GetIntArrayElements(precisions,0);
	jint		*scalesarray=env->GetIntArrayElements(scales,0);
	for (int i=0; i<variableslen; i++) {
		jstring	variable=
			(jstring)env->GetObjectArrayElement(variables,i);
		char	*variablestring=
			curGetStringUTFChars(env,variable,0);
		getSqlrCursor(env,self)->inputBind(variablestring,
					valuesarray[i],
					(uint32_t)precisionsarray[i],
					(uint32_t)scalesarray[i]);
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
	getSqlrCursor(env,self)->validateBinds();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    validBind
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_validBind
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jboolean	retval=
		(getSqlrCursor(env,self)->validBind(variablestring))?JNI_TRUE:JNI_FALSE;
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    executeQuery
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_executeQuery
  (JNIEnv *env, jobject self) {
	return (getSqlrCursor(env,self)->executeQuery())?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    fetchFromBindCursor
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_fetchFromBindCursor
  (JNIEnv *env, jobject self) {
	return (getSqlrCursor(env,self)->fetchFromBindCursor())?
						JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindString
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindString
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jstring	retval=curNewStringUTF(env,getSqlrCursor(env,self)->
					getOutputBindString(variablestring));
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindBlob
 * Signature: (Ljava/lang/String;)LB
 */
JNIEXPORT jbyteArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindBlob
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	long	length=getSqlrCursor(env,self)->
				getOutputBindLength(variablestring);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
			(jbyte *)getSqlrCursor(env,self)->
				getOutputBindBlob(variablestring));
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindClob
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindClob
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jstring	retval=curNewStringUTF(env,
				getSqlrCursor(env,self)->
					getOutputBindClob(variablestring));
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindInteger
 * Signature: (Ljava/lang/String;)J;
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindInteger
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jlong	retval=(jlong)getSqlrCursor(env,self)->
					getOutputBindInteger(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindDouble
 * Signature: (Ljava/lang/String;)J;
 */
JNIEXPORT jdouble JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindDouble
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jdouble	retval=(jdouble)getSqlrCursor(env,self)->
					getOutputBindDouble(variablestring);
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
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	long	length=getSqlrCursor(env,self)->getOutputBindLength(variablestring);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
			(jbyte *)getSqlrCursor(env,self)->
					getOutputBindString(variablestring));
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
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	jlong	retval=(jlong)getSqlrCursor(env,self)->
					getOutputBindLength(variablestring);
	curReleaseStringUTFChars(env,variable,variablestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getOutputBindCursor
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getOutputBindCursorInternal
  (JNIEnv *env, jobject self, jstring variable) {
	char	*variablestring=curGetStringUTFChars(env,variable,0);
	sqlrcursor	*bindcur=getSqlrCursor(env,self)->
				getOutputBindCursor(variablestring,true);
	curReleaseStringUTFChars(env,variable,variablestring);
	return reinterpret_cast<jlong>(bindcur);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    openCachedResultSet
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_openCachedResultSet
  (JNIEnv *env, jobject self, jstring filename) {
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				openCachedResultSet(filenamestring))?
				JNI_TRUE:JNI_FALSE;
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
	return (jint)getSqlrCursor(env,self)->colCount();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    rowCount
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_rowCount
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrCursor(env,self)->rowCount();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    totalRows
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_totalRows
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrCursor(env,self)->totalRows();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    affectedRows
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_affectedRows
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrCursor(env,self)->affectedRows();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    firstRowIndex
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_firstRowIndex
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrCursor(env,self)->firstRowIndex();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    endOfResultSet
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_endOfResultSet
  (JNIEnv *env, jobject self) {
	return (getSqlrCursor(env,self)->endOfResultSet())?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    errorMessage
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_errorMessage
  (JNIEnv *env, jobject self) {
	return curNewStringUTF(env,getSqlrCursor(env,self)->errorMessage());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    errorNumber
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_errorNumber
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrCursor(env,self)->errorNumber();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getNullsAsEmptyStrings
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getNullsAsEmptyStrings
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->getNullsAsEmptyStrings();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getNullsAsNulls
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getNullsAsNulls
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->getNullsAsNulls();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getField
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getField__JI
  (JNIEnv *env, jobject self, jlong row, jint col) {
	return curNewStringUTF(env,getSqlrCursor(env,self)->
					getField((uint64_t)row,(uint32_t)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getField
 * Signature: (JLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getField__JLjava_lang_String_2
  (JNIEnv *env, jobject self, jlong row, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jstring	retval=curNewStringUTF(env,getSqlrCursor(env,self)->
					getField((uint64_t)row,colstring));
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsInteger
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsInteger__JI
  (JNIEnv *env, jobject self, jlong row, jint col) {
	return (jlong)getSqlrCursor(env,self)->
			getFieldAsInteger((uint64_t)row,(uint32_t)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsInteger
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsInteger__JLjava_lang_String_2
  (JNIEnv *env, jobject self, jlong row, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)getSqlrCursor(env,self)->
			getFieldAsInteger((uint64_t)row,colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsDouble
 * Signature: (JI)D
 */
JNIEXPORT jdouble JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsDouble__JI
  (JNIEnv *env, jobject self, jlong row, jint col) {
	return (jdouble)getSqlrCursor(env,self)->
			getFieldAsDouble((uint64_t)row,(uint32_t)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsDouble
 * Signature: (JLjava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsDouble__ILjava_lang_String_2
  (JNIEnv *env, jobject self, jlong row, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jdouble	retval=(jlong)getSqlrCursor(env,self)->
			getFieldAsDouble((uint64_t)row,colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsByteArray
 * Signature: (JI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsByteArray__JI
  (JNIEnv *env, jobject self, jlong row, jint col) {
	long	length=getSqlrCursor(env,self)->
			getFieldLength((uint64_t)row,(uint32_t)col);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
				(jbyte *)getSqlrCursor(env,self)->
						getField((uint64_t)row,
							(uint32_t)col));
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldAsByteArray
 * Signature: (JLjava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldAsByteArray__JLjava_lang_String_2
  (JNIEnv *env, jobject self, jlong row, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	long	length=getSqlrCursor(env,self)->
			getFieldLength((uint64_t)row,colstring);
	jbyteArray	retval=env->NewByteArray(length);
	env->SetByteArrayRegion(retval,0,length,
				(jbyte *)getSqlrCursor(env,self)->
						getField((uint64_t)row,
								colstring));
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldLength
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldLength__JI
  (JNIEnv *env, jobject self, jlong row, jint col) {
	return (jlong)(getSqlrCursor(env,self)->
			getFieldLength((uint64_t)row,(uint32_t)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getRow
 * Signature: (J)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getRow
  (JNIEnv *env, jobject self, jlong row) {
	uint32_t	colcount=getSqlrCursor(env,self)->colCount();
	jobjectArray	retarray=
#ifdef CAST_NEW_OBJECT_ARRAY
		// NewObjectArray returns jarray, not jobjectArray, and must be
		// cast (at least on some systems)
		(jobjectArray)
#endif
			env->NewObjectArray(colcount,
					env->FindClass("java/lang/String"),
					curNewStringUTF(env,""));
	const char * const *field=getSqlrCursor(env,self)->
					getRow((uint64_t)row);
	for (uint32_t i=0; i<colcount; i++) {
		env->SetObjectArrayElement(retarray,i,
					curNewStringUTF(env,field[i]));
	}
	return retarray;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getRowLengths
 * Signature: (J)[J
 */
JNIEXPORT jlongArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getRowLengths
  (JNIEnv *env, jobject self, jlong row) {
	uint32_t	colcount=getSqlrCursor(env,self)->colCount();
	uint32_t	*rowlengths=getSqlrCursor(env,self)->
					getRowLengths((uint64_t)row);
	if (!rowlengths) {
		return 0;
	}
	jlong	*jrowlengths=new jlong[colcount];
	for (uint32_t i=0; i<colcount; i++) {
		jrowlengths[i]=(jlong)rowlengths[i];
	}
	jlongArray	retarray=env->NewLongArray(colcount);
	env->SetLongArrayRegion(retarray,0,colcount,jrowlengths);
	delete[] jrowlengths;
	return retarray;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnNames
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnNames
  (JNIEnv *env, jobject self) {
	int	colcount=getSqlrCursor(env,self)->colCount();
	jobjectArray	retarray=
#ifdef CAST_NEW_OBJECT_ARRAY
		// NewObjectArray returns jarray, not jobjectArray, and must be
		// cast (at least on some systems)
		(jobjectArray)
#endif
			env->NewObjectArray(colcount,
					env->FindClass("java/lang/String"),
					curNewStringUTF(env,""));
	const char * const *colnames=getSqlrCursor(env,self)->getColumnNames();
	if (!colnames) {
		return 0;
	}
	for (int i=0; i<colcount; i++) {
		env->SetObjectArrayElement(retarray,i,
				curNewStringUTF(env,colnames[i]));
	}
	return retarray;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getFieldLength
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getFieldLength__JLjava_lang_String_2
  (JNIEnv *env, jobject self, jlong row, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)getSqlrCursor(env,self)->
				getFieldLength((uint64_t)row,colstring);
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
	return curNewStringUTF(env,getSqlrCursor(env,self)->
					getColumnName((uint32_t)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnType
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnType__I
  (JNIEnv *env, jobject self, jint col) {
	return curNewStringUTF(env,getSqlrCursor(env,self)->
					getColumnType((uint32_t)col));
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnType
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnType__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jstring	retval=curNewStringUTF(env,getSqlrCursor(env,self)->
						getColumnType(colstring));
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
	return (jlong)getSqlrCursor(env,self)->
			getColumnPrecision((uint32_t)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnPrecision
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnPrecision__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)getSqlrCursor(env,self)->
				getColumnPrecision(colstring);
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
	return (jlong)getSqlrCursor(env,self)->getColumnScale((uint32_t)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnScale
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnScale__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jlong	retval=(jlong)getSqlrCursor(env,self)->
				getColumnScale(colstring);
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
	return (getSqlrCursor(env,self)->
			getColumnIsNullable((uint32_t)col))?
			JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsNullable
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsNullable__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
					getColumnIsNullable(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsPrimaryKey((uint32_t)col))?
			JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsPrimaryKey
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsPrimaryKey__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				getColumnIsPrimaryKey(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsUnique((uint32_t)col))?
			JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsUnique
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsUnique__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				getColumnIsUnique(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsUnique((uint32_t)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsPartOfKey
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsPartOfKey__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				getColumnIsPartOfKey(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsUnique((uint32_t)col))?
			JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsUnsigned
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsUnsigned__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
					getColumnIsUnsigned(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsUnique((uint32_t)col))?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsZeroFilled
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsZeroFilled__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
					getColumnIsZeroFilled(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsUnique((uint32_t)col))?
			JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsBinary
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsBinary__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
					getColumnIsBinary(colstring))?
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
	return (getSqlrCursor(env,self)->
			getColumnIsUnique((uint32_t)col))?
			JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnIsAutoIncrement
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnIsAutoIncrement__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jboolean	retval=(getSqlrCursor(env,self)->
				getColumnIsAutoIncrement(colstring))?
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
	return (jint)getSqlrCursor(env,self)->getColumnLength((uint32_t)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getColumnLength
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getColumnLength__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jint	retval=(jint)getSqlrCursor(env,self)->
				getColumnLength(colstring);
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
	return (jint)getSqlrCursor(env,self)->getLongest((uint32_t)col);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getLongest
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getLongest__Ljava_lang_String_2
  (JNIEnv *env, jobject self, jstring col) {
	char	*colstring=curGetStringUTFChars(env,col,0);
	jint	retval=(jint)getSqlrCursor(env,self)->getLongest(colstring);
	curReleaseStringUTFChars(env,col,colstring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    getResultSetId
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_getResultSetId
  (JNIEnv *env, jobject self) {
	return (jshort)getSqlrCursor(env,self)->getResultSetId();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    suspendResultSet
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_suspendResultSet
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->suspendResultSet();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    resumeResultSet
 * Signature: (S)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_resumeResultSet
  (JNIEnv *env, jobject self, jshort id) {
	return (jboolean)getSqlrCursor(env,self)->resumeResultSet((uint16_t)id);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    resumeCachedResultSet
 * Signature: (SLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_resumeCachedResultSet
  (JNIEnv *env, jobject self, jshort id, jstring filename) {
	char	*filenamestring=curGetStringUTFChars(env,filename,0);
	jboolean	retval=(jboolean)getSqlrCursor(env,self)->
			resumeCachedResultSet((uint16_t)id,filenamestring);
	curReleaseStringUTFChars(env,filename,filenamestring);
	return retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRCursor
 * Method:    closeResultSet
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRCursor_closeResultSet
  (JNIEnv *env, jobject self) {
	getSqlrCursor(env,self)->closeResultSet();
}

#ifdef __cplusplus
}
#endif
