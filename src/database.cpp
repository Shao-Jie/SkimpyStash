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
//	cout << "key is "<<key<<",value is "<<value<<endl;
	kv = (struct slice *)malloc(sizeof(struct slice));
	memset(kv->Key,0,KSIZE);
	memset(kv->Value,0,VSIZE);
	snprintf(kv->Key,KSIZE,key);
	snprintf(kv->Value,VSIZE,value);
//	memcpy(kv->Key,key,KSIZE);
//	memcpy(kv->Value,value,VSIZE);
	kv->op = ADD;
	hashtable->Insert(kv);
	return hashtable->Insert(kv);
}

RC DataBase::DeleteData(char *key)
{
	struct slice *kv;
	kv = (struct slice *)malloc(sizeof(struct slice));
	memset(kv->Key,0,KSIZE);
	memset(kv->Value,0,VSIZE);
	snprintf(kv->Key,KSIZE,key);
//	memcpy(kv->Key,key,KSIZE);
	kv->op = DELETE;	
  return hashtable->Insert(kv);
}

RC DataBase::FindData(char *key,char *value)
{
	char _key[KSIZE];
	snprintf(_key,KSIZE,key);
	return hashtable->Find(_key,value);
}
