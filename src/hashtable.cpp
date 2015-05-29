//
// hashtable.cpp
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
LLINT _getoffset(int fd)
{
	struct stat sb;
	if(fstat(fd,&sb) == -1)
		return 0;
	return(LLINT)sb.st_size;
}


//HashTable::HashTable(int _fd,int _numBuckets = HASH_M)
HashTable::HashTable(int _fd)
{
	this->fd = _fd;
	this->numBuckets = HASH_M;
	int i = 0;
	offsetMap = (LLINT*)malloc(sizeof(LLINT) * this->numBuckets);
	for(i=0;i<this->numBuckets;i++){
		offsetMap[i] = -1; // init offsetMap
	}
	this->bloom = new BloomFilter();
}

HashTable::~HashTable()
{
	delete offsetMap;
}

int HashTable::Hash(char *str) const
{
	unsigned int seed = 131;
	unsigned int hash = 0;
	while(*str){
		hash = hash * seed + (*str++);
	}
	return (hash%numBuckets);
}

RC HashTable::Insert(struct slice *kv)
{
	//  int bucket = 0;
	int bucket = Hash(kv->Key);
	if (bucket <0){
		__DEBUG("In hashtable Insert,hash err!");
		return ERR_HASH;
	}
	if(kv->op == ADD){ // only need insert bloom filter when operaton is ADD,(DELETE not necessary)
		bloom->InsertBloomFilter(kv->Key);
	}
	kv->offset = offsetMap[bucket];
	int offset = _getoffset(fd); // get the last position before write
	lseek(fd,offset,SEEK_SET);  // seek to this position
	offsetMap[bucket] = offset; // offsetmap only need record the last position
	if(write(fd,(char*)kv,sizeof(struct slice)) != sizeof(struct slice))
  {
		__DEBUG("In hashtabe Insert,write err!");
	  return ERR_WRITE;
	}
	return OK;
}

RC HashTable::Find(char *str,char *value)
{
	//  int bucket = 0;
  int bucket = Hash(str);
	if (bucket < 0){
		__DEBUG("In hashtable Find,hash err!");
		return ERR_HASH;
	}

	if (bloom->IsContain(str)==false){ // if not in bloomfilter,
		return DATA_NOFIND;             // we donot need find in db.
	}
									

	struct slice *newkv;
	newkv = (struct slice *)malloc(sizeof(struct slice));
	int offset = offsetMap[bucket];
	while(true){ // just read from databse like linked list,using offset recorded.
	  if(offset <0){ // the offset is -1,indicate we not find it in database
			return DATA_NOFIND;
		}else{
			lseek(fd,offset,SEEK_SET);
			if (read(fd,newkv,sizeof(struct slice)) != sizeof(struct slice))
			{
				__DEBUG("In hashtable Find,read err!");
				return ERR_READ;
			}
			if (strcmp(newkv->Key,str)==0){
				if (newkv->op == DELETE){ // this data has been delete.
					return DATA_DELETE;
				}else{ // we find this data
					memcpy(value,newkv->Value,VSIZE);  // set the value and return it(use index).
					return OK;
				}
			}
			offset = newkv->offset;
		}
	}
	return ERR;
}
