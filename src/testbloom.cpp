//
// testbloom.cpp
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

#define TESTLOOP 20

using namespace std;
int main()
{
	char key[KSIZE];
	class BloomFilter *bloom = new BloomFilter();
	int i = 0;
	for(i = 0;i < TESTLOOP;i++){
		snprintf(key,KSIZE,"key:%d",i);
		bloom->InsertBloomFilter(key);
	}
	for(i = 0; i< 2*TESTLOOP;i++){ // when i less than testloop we will get it in bloom,others not in bloom
		snprintf(key,KSIZE,"key:%d",i);
		if(bloom->IsContain(key)){
			__DEBUG("OK,this key = %s is in BloomFilter",key);
		}else{
			__DEBUG("NO,this key = %s isnot in BloomFilter",key);
		}
		
	}
		
	return 0;
}
