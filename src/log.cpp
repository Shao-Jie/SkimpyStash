//
// log.cpp
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
Log::Log(const char *fileName) // create db.
{
	char db_name[MAXNAME];
	memset(db_name,0,MAXNAME);
	snprintf(db_name,MAXNAME,"%s.db",fileName);
	if(_file_exists() != OK){ 
		unlink(db_name);
	}
	memcpy(DBName,db_name,MAXNAME);
	fd = open(db_name,DB_CREAT_FLAGS,0644);
}

Log::~Log()
{
	close(fd);
}

int Log::Getfd()
{
	return this->fd;
}


RC Log::_file_exists()
{
	int fd = open(DBName,O_RDWR);
	if(fd > -1){ // if file exists, need recove it!
		close(fd);
		__DEBUG("file exists! need do something to recove!");
		return WARN_FILE_EXISTS;
	}
	return OK;
}
