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

HashTable::HashTable(int _fd)
{
	this->fd = _fd;
	this->numBuckets = HASH_M;
	int i = 0;
	this->bloom = new BloomFilter();
	this->header = (struct fileHdr *)malloc(sizeof(fileHdr));
	this->header->pageNum = 0;
	this->pageSize = sizeof(struct slice);
	this->buffer = new char [BUFFERNUM*this->pageSize]; // the write buffer,when full,we write dist at one time,and search it first when find.
	this->bufTable = new BufPageDesc[BUFFERNUM]; // the information describe buftable
	this->bufIndex = 0;                        //
	this->hashMap = new HashDesc[this->numBuckets];
	for(i = 0; i< this->numBuckets; i++){ // init hashMap -1 is a flag.
		hashMap[i].bufferNum = -1;
		hashMap[i].pageNum = -1;
	}
	for(i = 0;i< BUFFERNUM;i++){
		bufTable[i].prev = -1; // index the previous position in this buffer.
	}
}

HashTable::~HashTable()
{
	delete []hashMap;
	delete []bufTable;
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

RC HashTable::InsertBuffer(struct slice *kv)
{
//	  int bucket = 0;
	int bucket = Hash(kv->key);
	if (bucket <0){
		__DEBUG("In hashtable Insert,hash err!");
		return ERR_HASH;
	}
	if(kv->op == ADD){ // only need insert bloom filter when operaton is ADD,(DELETE not necessary)
		bloom->InsertBloomFilter(kv->key);
	}
	kv->pageNum = hashMap[bucket].pageNum; 
	hashMap[bucket].pageNum = header->pageNum;
	header->pageNum ++;
//	__DEBUG("use insert buffer function,bufferIndex = %d,the pageNum is %d",bufIndex,header->pageNum);

	memcpy(&buffer[bufIndex*pageSize],(char *)kv,pageSize); // write data to the buffer
	bufTable[bufIndex].prev = hashMap[bucket].bufferNum;//
	hashMap[bucket].bufferNum = bufIndex;

	if (bufIndex == BUFFERNUM - 1){ // now the buffer is full.
		// start write the buffer data to file
		lseek(fd,(header->pageNum - BUFFERNUM)*pageSize,SEEK_SET);
		if(write(fd,buffer,pageSize*BUFFERNUM) != pageSize*BUFFERNUM)
	  {
			__DEBUG("In hashtabe Insertbuffer,write err!");
		  return ERR_WRITE;
		}
#ifdef USEFSYNC
		if(fsync(fd) != 0)
		{
			__DEBUG("fsync error!");
			return ERR_FSYNC;
		}
#endif

		// clear zeor,maybe need not, because,we will rewrite it.
		memset(buffer,0,pageSize*BUFFERNUM);
		int i;
		for(i = 0;i< BUFFERNUM;i++){
			bufTable[i].prev = -1;
		}
		bufIndex = 0;
		
		for(i = 0; i< this->numBuckets; i++){
			hashMap[i].bufferNum = -1;			
		}
	}else{
		bufIndex ++;
	}
	return OK;
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
	kv->pageNum = hashMap[bucket].pageNum;
	lseek(fd,header->pageNum*pageSize,SEEK_SET);  // seek to this position = pageNum * pageSize
	hashMap[bucket].pageNum = header->pageNum; // hashMap[bucket].pageNum only need record the last position
	header->pageNum ++ ;
	if(write(fd,(char*)kv,pageSize) != pageSize)
  {
		__DEBUG("In hashtabe Insert,write err!");
	  return ERR_WRITE;
	}
#ifdef USEFSYNC
		if(fsync(fd) != 0)
		{
			__DEBUG("fsync error!");
			return ERR_FSYNC;
		}
#endif
	return OK;
}

RC HashTable::FindBuffer(char *str,char *value)
{
// int bucket = 0;
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
	struct slice *_kv;
	_kv = (struct slice *)malloc(sizeof(struct slice));
	int pageNum = hashMap[bucket].pageNum;
	int bufferNum = hashMap[bucket].bufferNum;
	while(true){ // first search in buffer
		if(bufferNum < 0){ // we did not find it in buffer
			break;
		}
		newkv = (struct slice*)&buffer[bufferNum*pageSize];// get the slice in this position.
		memcpy(_kv->key,newkv->key,KSIZE); // if we operate newkv directly,may have change the value of buffer
		memcpy(_kv->value,newkv->value,VSIZE);
		_kv->op = newkv->op;
		_kv->pageNum = newkv->pageNum;
		if(strcmp(_kv->key,str) == 0){
			if(_kv->op == DELETE){
				return DATA_DELETE;
			}else{
				memcpy(value,_kv->value,VSIZE);
				return OK;
			}
		}
//		__DEBUG("the buffer page num is %d,the key is %s the value is %s,the op is %d\n",pageNum,_kv->key,_kv->value,_kv->op);
		pageNum = _kv->pageNum;
		bufferNum = bufTable[bufferNum].prev;
	}

	while(true){ // next search in disk file
		if(pageNum < 0){ // we cannot find in disk 
			return DATA_NOFIND;
		}
		if(pageNum == 0)
		{
			int x;
			scanf("%d",&x);
		}
		lseek(fd,pageNum*pageSize,SEEK_SET);
		if (read(fd,_kv,pageSize) != pageSize)
		{
			__DEBUG("In hashtable Find,read err!");
			return ERR_READ;
		}
		if (strcmp(_kv->key,str)==0){
			if (_kv->op == DELETE){ // this data has been delete.
				return DATA_DELETE;
			}else{ // we find this data
				memcpy(value,_kv->value,VSIZE);  // set the value and return it(use index).
				return OK;
			}
		}
//		__DEBUG("the page num is %d,the key is %s the value is %s,the op is %d",pageNum,newkv->key,newkv->value,newkv->op);
		pageNum = _kv->pageNum;
	}
	return ERR;
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
	int pageNum = hashMap[bucket].pageNum;
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
RC HashTable::Close()
{
	lseek(fd,(header->pageNum - bufIndex)*pageSize,SEEK_SET);
	write(fd,buffer,pageSize*bufIndex);
	if(write(fd,buffer,pageSize*bufIndex) != pageSize*bufIndex)
	{
		__DEBUG("In hashtabe Insertbuffer,write err!");
	  return ERR_WRITE;
	}
#ifdef USEFSYNC
	if(fsync(fd) != 0)
	{
		__DEBUG("fsync error!");
		return ERR_FSYNC;
	}
#endif
	close(fd);
	return OK;
}
