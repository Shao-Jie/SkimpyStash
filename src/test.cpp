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
#include "skimpystash.h"
using namespace std;
int main()
{
	class DataBase *db = new DataBase("test");
	char value[VSIZE];
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
	db->CloseDB();
	return 0;
}
