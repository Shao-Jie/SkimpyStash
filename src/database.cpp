//
// database.cpp
// Author: Jie Shao
// Email : jsshaojie@gmail.com
//

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "skimpystash.h"

using namespace std;

DataBase::DataBase(const char *fileName)
{
	class Log *_log;
	_log = new Log(fileName);
	log = _log;
	hashtable = new HashTable(log->Getfd());
}

DataBase::~DataBase()
{
	delete log;
	delete hashtable;
}

RC DataBase::AddData(const char *key, const char *value)
{
	struct slice *kv;
	kv = (struct slice *)malloc(sizeof(struct slice));
	memset(kv->key,0,KSIZE);
	memset(kv->value,0,VSIZE);
	snprintf(kv->key,KSIZE,key); // cannot use memcpy,if this key or value not allocate space first,memcpy will be wrong.
	snprintf(kv->value,VSIZE,value);
//	memcpy(kv->key,key,KSIZE);
//	memcpy(kv->value,value,VSIZE);
	kv->op = ADD;                // define this operation is ADD
#ifdef USEBUFFER
	return hashtable->InsertBuffer(kv);
#else
	return hashtable->Insert(kv); // insert the slice in hashtable.
#endif
}

RC DataBase::DeleteData(char *key)
{
	struct slice *kv;
	kv = (struct slice *)malloc(sizeof(struct slice));
	memset(kv->key,0,KSIZE);
	memset(kv->value,0,VSIZE);
	snprintf(kv->key,KSIZE,key);
//	memcpy(kv->key,key,KSIZE);
	kv->op = DELETE;	              // define this operation is DELETE
#ifdef USEBUFFER
	return hashtable->InsertBuffer(kv);
#else
  return hashtable->Insert(kv);
#endif
}

RC DataBase::FindData(char *key,char *value)
{
	char _key[KSIZE];
	snprintf(_key,KSIZE,key);
#ifdef USEBUFFER
	return hashtable->FindBuffer(_key,value);
#else
	return hashtable->Find(_key,value);
#endif

}
RC DataBase::CloseDB()
{
	return hashtable->Close();
}
