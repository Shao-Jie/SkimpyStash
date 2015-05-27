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
	for(i-0;i<this->numBuckets;i++){
		offsetMap[i] = -1; // init offsetMap
	}
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
		cout << "In hashtable Insert,hash err!"<<endl;
		return ERR_HASH;
	}
	kv->offset = offsetMap[bucket];
	int offset = _getoffset(fd);
	lseek(fd,offset,SEEK_SET);
	offsetMap[bucket] = offset;
	if(write(fd,(char*)kv,sizeof(struct slice)) != sizeof(struct slice))
  {
  	cout<<"In hashtabe Insert,write err!"<<endl;
	  return ERR_WRITE;
	}
	return OK;
}

RC HashTable::Find(char *str,char *value)
{
	//  int bucket = 0;
  int bucket = Hash(str);
	if (bucket < 0){
		cout << "In hashtable Find,hash err!"<<endl;
		return ERR_HASH;
	}
	struct slice *newkv;
	newkv = (struct slice *)malloc(sizeof(struct slice));
	int offset = offsetMap[bucket];
	while(true){
	  if(offset <0){
			return DATA_NOFIND;
		}else{
			lseek(fd,offset,SEEK_SET);
			if (read(fd,newkv,sizeof(struct slice)) != sizeof(struct slice))
			{
		  	cout<<"In hashtabe Find,read err!"<<endl;
				return ERR_READ;
			}
			if (strcmp(newkv->Key,str)==0){
				if (newkv->op == DELETE){
					return DATA_DELETE;
				}else{
					memcpy(value,newkv->Value,VSIZE);
					return OK;
				}
			}
			offset = newkv->offset;
		}
	}
	return ERR;
}
