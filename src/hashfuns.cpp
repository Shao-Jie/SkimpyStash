//
// hashfuns.cpp
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

HashFuns::HashFuns(void)
{
	this->hashNum = 7; // defined default
}

HashFuns::HashFuns(int num)
{
	if(num <= 0){ // wrong num
		this->hashNum = num;
	}else{
		this->hashNum = num;
	}
}

HashFuns::~HashFuns()
{
}

RC HashFuns::GenerateHashValue(char *str,unsigned int *Ret)
{
	if(this->hashNum<=0){
		__DEBUG("in hash funs GenerateHashValue,hashnum less than zero");
		return HASH_NUM_ZERO;
	}
	int i;
	for(i = 0; i < this->hashNum; i++){
		Ret[i] = this->HashValue(str,i); // get every hashvalue
	}
	return OK;
}

unsigned int HashFuns::HashValue(char *str,int index)
{
	switch(index)
	{
		case 0:
			return this->RSHash(str);
		case 1:
			return this->JSHash(str);
		case 2:
			return this->PJWHash(str);
		case 3:
			return this->ELFHash(str);
		case 4:
			return this->BKDRHash(str);
		case 5:
			return this->SDBMHash(str);
		case 6:
			return this->DJBHash(str);
		case 7:
			return this->DEKHash(str);
		case 8:
			return this->BPHash(str);
		case 9:
			return this->FNVHash(str);
		case 10:
			return this->APHash(str);
		default:
			return ERR;
	}
}

unsigned int HashFuns::RSHash(char *str) const
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	while(*str){
		hash = hash * a + (*str ++);
		a = a * b;
	}
	return hash;
}

unsigned int HashFuns::JSHash(char *str) const
{
	 unsigned int hash = 1315423911;
	 while(*str){
			hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	 }
	 return hash;
}

unsigned int HashFuns::PJWHash(char *str) const
{
	const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);    
  const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);    
  const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);    
  const unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);    
  unsigned int hash              = 0;    
  unsigned int test              = 0;
	while(*str){
		hash = (hash << OneEighth) + (*str ++);    
	  if((test = hash & HighBits)  != 0){
			hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return hash;
}

unsigned int HashFuns::ELFHash(char *str) const
{
	unsigned int hash = 0;    
	unsigned int x    = 0;
	while(*str){
		hash = (hash << 4) + (*str++);
		if((x = hash & 0xF0000000L) != 0){    
			hash ^= (x >> 24);    
		}    
	  hash &= ~x;
	}
	return hash;
}

unsigned int HashFuns::BKDRHash(char *str) const
{
	unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */    
  unsigned int hash = 0;
  while(*str){
		hash = (hash * seed) + (*str++);
	}
	return hash;
}

unsigned int HashFuns::SDBMHash(char *str) const
{
	unsigned int hash = 0;
	while(*str){
		hash = (*str ++) + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

unsigned int HashFuns::DJBHash(char *str) const
{
	unsigned int hash = 5381;
	while(*str){
		hash = ((hash << 5) + hash) + (*str++);
	}
	return hash;
}

unsigned int HashFuns::DEKHash(char *str) const
{
	unsigned int hash = strlen(str);
//	cout<< " in DEKHash for test let me see the strlen is "<< hash <<endl;
	while(*str){
		hash = ((hash << 5) ^ (hash >> 27)) ^ (*str ++);
	}
	return hash;
}
unsigned int HashFuns::BPHash(char *str) const
{
	unsigned int hash = strlen(str);
	while(*str){
		hash = hash << 7 ^ (*str ++);
	}
	return hash;
}
unsigned int HashFuns::FNVHash(char *str) const
{
	const unsigned int fnv_prime = 0x811C9DC5;    
	unsigned int hash      = 0;
	while(*str){
		hash *= fnv_prime;    
    hash ^= (*str ++);
	}
	return hash;
}
unsigned int HashFuns::APHash(char *str) const
{
	unsigned int hash = 0xAAAAAAAA;
	int len = strlen(str);
	unsigned int i = 0;
	for(i = 0; i < len; str++, i++){
		hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ (*str) * (hash >> 3)) :    
		                         (~((hash << 11) + (*str) ^ (hash >> 5)));
	}
 return hash;
}
