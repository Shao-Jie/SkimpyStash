//
// test.cpp
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
#include <time.h>
#include "skimpystash.h"
using namespace std;
#define LOOP (11000000)
int main()
{
	class DataBase *db = new DataBase("test");
	char key[KSIZE];
	char value[VSIZE];
	clock_t start,finish;
	double duration;
	RC rc;
	db->AddData("aaaa","bbb");
	db->AddData("yyyyy","ccc");
	db->DeleteData("aaaa");
	db->AddData("aaaa","bbb");
	db->AddData("yyyyy","ccc");
	db->DeleteData("aaaa");
	db->AddData("aaaa","bbb");
	db->AddData("yyyyy","ccc");
	db->DeleteData("aaaa");
	db->AddData("aaaa","bbb");
	db->AddData("yyyyy","ccc");
	db->DeleteData("aaaa");
	if (db->FindData("aaaa",value) ==OK){
		__DEBUG("the key value pair , the key is %s,value is %s","aaaa",value);
	}else{
		__DEBUG("not find this value! The key is %s","aaaa");
	}

	if (db->FindData("yyyyy",value) ==OK){
		__DEBUG("the key value pair , the key is %s,value is %s","yyyyy",value);
	}else{
		__DEBUG("not find this value! The key is %s","aaaa");
	}	

	if (db->FindData("xxx",value) ==OK){
		__DEBUG("the key value pair , the key is %s,value is %s","xxx",value);
	}else{
		__DEBUG("not find this value! The key is %s","xxx");
	}	
	start = clock();
	int i = 0;
	for (i=1;i<LOOP;i++)
	{
		snprintf(key,KSIZE,"key:%d",i);
		snprintf(value,VSIZE,"value:%d",i);
		db->AddData(key,value);
	}
	finish = clock();
	duration = (double)(finish - start)/CLOCKS_PER_SEC;
	__DEBUG("write %f seconds",duration);
//	db->FindData(key,value);
//	for(i=LOOP;i>0;i--)

	start = clock();
	for (i=1;i<LOOP;i++)
	{
		snprintf(key,KSIZE,"key:%d",i);
		if(db->FindData(key,value)!=OK){
			__DEBUG("not find this value! The key is %s",key);
		}else{
//			__DEBUG("the key value pair , the key is %s,value is %s",key,value);
		}
	}	

	finish = clock();
	duration = (double)(finish - start)/CLOCKS_PER_SEC;
	__DEBUG("read %f seconds",duration);
	db->CloseDB();
	return 0;
}
