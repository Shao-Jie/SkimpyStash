//
// skimpystash.h
//
#ifndef SKIMPYSTASH_H
#define SKIMPYSTASH_H


// Globally-useful defines
//

#if defined(__linux__)
	#define LLINT long long int
	#define DB_CREAT_FLAGS (O_RDWR | O_TRUNC | O_CREAT | O_LARGEFILE)
#endif

#define HASH_M 249997

#define KSIZE 30

#define VSIZE 50

#define MAXNAME 24

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

//
// define data struct
//

struct slice{
	char Key[KSIZE];
	char Value[VSIZE];
	LLINT offset;
	int op;
};

class HashTable{
public:
	HashTable(int _fd);
	~HashTable();
	RC Insert(struct slice*kv);
	RC Find (char*key,char *value);
private:
	int Hash(char *key)const;
	int fd;
	int numBuckets;
	LLINT *offsetMap;
};

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


class DataBase{
public: 
  DataBase(const char *fileName);
  ~DataBase();
	RC AddData(const char *key,const char*value);
	RC DeleteData(char *key);
	RC FindData(char *key,char *value);
	RC CloseDB();
private:
	int fd;
	Log *log;
	HashTable *hashtable;
};
#endif // end define SKIMPYSTASH_H
