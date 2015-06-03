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

//HashTable::HashTable(int _fd,int _numBuckets = HASH_M)
HashTable::HashTable(int _fd)
{
	this->fd = _fd;
	this->numBuckets = HASH_M;
	int i = 0;
//	offsetMap = (LLINT*)malloc(sizeof(LLINT) * this->numBuckets);
	pageNumMap= new int[this->numBuckets];
	for(i = 0;i < this->numBuckets;i++){
		pageNumMap[i] = -1; // init offsetMap
	}
	this->bloom = new BloomFilter();
	this->header = (struct fileHdr *)malloc(sizeof(fileHdr));
	this->header->pageNum = 0;
	this->pageSize = sizeof(struct slice);
}

HashTable::~HashTable()
{
	delete pageNumMap;
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
	int bucket = Hash(kv->key);
	if (bucket <0){
		__DEBUG("In hashtable Insert,hash err!");
		return ERR_HASH;
	}
	if(kv->op == ADD){ // only need insert bloom filter when operaton is ADD,(DELETE not necessary)
		bloom->InsertBloomFilter(kv->key);
	}
	kv->pageNum = pageNumMap[bucket];
//	int offset = _getoffset(fd); // get the last position before write
	lseek(fd,header->pageNum*pageSize,SEEK_SET);  // seek to this position = pageNum * pageSize
	pageNumMap[bucket] = header->pageNum; // pageNumMap only need record the last position
	header->pageNum ++ ;
	if(write(fd,(char*)kv,pageSize) != pageSize)
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
	int pageNum = pageNumMap[bucket];
	while(true){ // just read from databse like linked list,using pageNum recorded.
	  if(pageNum <0){ // the pageNum is -1,indicate we not find it in database
			return DATA_NOFIND;
		}else{
			lseek(fd,pageNum*pageSize,SEEK_SET);
			if (read(fd,newkv,pageSize) != pageSize)
			{
				__DEBUG("In hashtable Find,read err!");
				return ERR_READ;
			}
			if (strcmp(newkv->key,str)==0){
				if (newkv->op == DELETE){ // this data has been delete.
					return DATA_DELETE;
				}else{ // we find this data
					memcpy(value,newkv->value,VSIZE);  // set the value and return it(use index).
					return OK;
				}
			}
			pageNum = newkv->pageNum;
		}
	}
	return ERR;
}
