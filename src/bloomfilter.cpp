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

BloomFilter::BloomFilter()
{
	this->numBuckets = HASH_M;
	this->hashFunsNum = 10;
	this->hashFuns = new HashFuns(this->hashFunsNum);
	this->hashFunsRet = new unsigned int[this->hashFunsNum];
//	this->bitmap = (char **)malloc(this->hashFunsNum * sizeof(char *));
	this->bitMap = new char *[this->hashFunsNum];
	int i = 0;
	for(i=0;i < this->hashFunsNum;i++){
		this->bitMap[i] = new char[this->numBuckets/8 + 1];
	}
}

// release resource
BloomFilter::~BloomFilter()
{
	delete []bitMap;
	delete hashFuns;
}


// insert this key into bloom filter

RC BloomFilter::InsertBloomFilter(char *str)
{
#ifndef USEBLOOMFILTER
	return OK;
#endif
	this->hashFuns->GenerateHashValue(str,this->hashFunsRet); // call HashFuns generateHashValue
	for(int i = 0;i< this->hashFunsNum;i++){
		unsigned int pos = (this->hashFunsRet[i]%this->numBuckets); // get every hash pos
		InsertBitMap(i,pos);
	}
	return OK;
}

RC BloomFilter::InsertBitMap(int index,unsigned int pos)
{
	this->bitMap[index][pos/8] |= (0x01<<(pos%8)); // bit operator,set this bit 1
	return OK;
}

bool BloomFilter::IsContain(char*str)
{
#ifndef USEBLOOMFILTER
	return true;
#endif
	this->hashFuns->GenerateHashValue(str,this->hashFunsRet); // generateHashValue use this key
	for(int i = 0;i< this->hashFunsNum;i++){
		unsigned int pos = (this->hashFunsRet[i]%this->numBuckets);
		if(FindBitMap(i,pos)==false){
//			__DEBUG("no this key %s,retunrn when i is %d",str,i);
			return false;
		}
	}
	return true;
}

bool BloomFilter::FindBitMap(int index,unsigned int pos)
{
	if((this->bitMap[index][pos/8] & (0x01<<(pos%8))) == 0x00) // find in bitmap,if not exited this result is 0,others not 0
		return false;
	else
		return true;
}
