//
// skimpystash.h
// Author: Jie Shao
// Email : jsshaojie@gmail.com
//

#ifndef SKIMPYSTASH_H
#define SKIMPYSTASH_H

//#include <sys/types.h>
//#include <unistd.h>

// Globally-useful defines
//

#if defined(__linux__)
	#define LLINT long long int
	#define DB_CREAT_FLAGS (O_RDWR | O_TRUNC | O_CREAT | O_LARGEFILE)
#endif

#define DEBUG

#ifdef DEBUG
#define __DEBUG(x...) do {                                    \
	fprintf(stderr,"[%d] %s(%d) ",(int)getgid(), __FUNCTION__,__LINE__); \
	fprintf(stderr,##x);             \
	fprintf(stderr,"\n");              \
}while(0)
#else
	#define __DEBUG(x...)
#endif

#define HASH_M 249997

#define KSIZE 30

#define VSIZE 50

#define MAXNAME 24

// define operation type add or delete
enum OpType{
	ADD,
	DELETE
};

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 0
#endif

#ifndef NULL
#define NULL 0
#endif

//
// define return code
//

typedef int RC;
// need add others
#define OK                    0

#define ERR                  (-1)
#define ERR_HASH             (-2) 
#define ERR_WRITE            (-3)
#define ERR_READ             (-4)

#define WARN                   1
#define WARN_FILE_EXISTS       2
#define DATA_DELETE            3
#define DATA_FIND              4
#define DATA_NOFIND            5
#define HASH_NUM_ZERO          6

//
// define data struct
//

struct slice{
	char Key[KSIZE];
	char Value[VSIZE];
	LLINT offset;
	int op;
};

// define hash funs used by bloom filter
class HashFuns{
public:
	HashFuns(void);
	HashFuns(int num);
	~HashFuns();
	RC GenerateHashValue(char *str,unsigned int *Ret);// call hashvalue,
	unsigned int HashValue(char*str,int index); // call private function,get wanted hash result.
	
private:
	unsigned int RSHash(char *str) const;
	unsigned int JSHash(char *str) const;
	unsigned int PJWHash(char *str) const;
	unsigned int ELFHash(char *str) const;
	unsigned int BKDRHash(char *str) const;
	unsigned int SDBMHash(char *str) const;
	unsigned int DJBHash(char *str) const;
	unsigned int DEKHash(char *str) const;
	unsigned int BPHash(char *str) const;
	unsigned int FNVHash(char *str) const;
	unsigned int APHash(char *str) const;
	int hashNum; // hash function nam, the max is 11
};

class BloomFilter{
public:
	BloomFilter();
	~BloomFilter();
	RC InsertBloomFilter(char *str);
	bool IsContain(char *str);
private:
	RC InsertBitMap(int index,unsigned int pos); // set the bitmap
	bool FindBitMap(int index,unsigned int pos); // get result from bitmap
	class HashFuns *hashFuns;
	int numBuckets; // how many slot
	int hashFunsNum; // the name of hash function in bloom filter
	unsigned int *hashFunsRet; // hashFuns result, hash value
	char **bitMap; // the bitmap stored
};

// define main struct hashtable
class HashTable{
public:
	HashTable(int _fd);
	~HashTable();
	RC Insert(struct slice*kv); // insert this slice
	RC Find (char*key,char *value); // find this value using key.
private:
	int Hash(char *key)const; // The hash function used/
	int fd;
	int numBuckets; // slot
	LLINT *offsetMap; // defined as offsetMap[numBuckets]_offset,
	class BloomFilter *bloom; // using bloomfilter to speedup read operator
};


// need add function
class Log{
//friend: class HashTable;
public:
	Log(const char *name);
	~Log();
	int Getfd();
private:
	int fd;
	char DBName[MAXNAME];
	RC _file_exists();
};

// called by user
class DataBase{
public: 
  DataBase(const char *fileName); // creat this database
  ~DataBase();
	RC AddData(const char *key,const char*value); // add data,simple use key,value
	RC DeleteData(char *key); // delete data,index by key
	RC FindData(char *key,char *value); // find data, index by key
	RC CloseDB(); // not used.
private:
	int fd;
	Log *log;
	class HashTable *hashtable;
};
#endif // end define SKIMPYSTASH_H
