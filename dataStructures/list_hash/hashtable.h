#ifndef SD2016_DOUBLE_HASHTABLE_H
#define SD2016_DOUBLE_HASHTABLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"


class Hash
{
 public:
	Hash(unsigned int bucketsNum);
	~Hash();
	void insertRecord(int key,int value);
	Node* getRecord(int key);
	void print();

 private:
	List** m_buckets;
	unsigned int m_buckets_number;
};




#ifdef __cplusplus
}
#endif
#endif //SD2016_DOUBLE_HASHTABLE_H
