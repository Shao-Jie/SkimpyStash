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
	struct DataBase *db = new DataBase("test");
	char value[VSIZE];
	RC rc;
	db->AddData("aaa","bbb");
	db->AddData("aaa","ccc");
//	db->DeleteData("aaa");
	if (db->FindData("aaa",value) ==OK){
		cout<<"result " <<value<<endl;
	}else{
		cout <<"not find this value!"<<endl;
	}	
//	db->AddData("aaa","bbb");

}
