#ifndef SD2016_DOUBLE_LIST_H
#define SD2016_DOUBLE_LIST_H
#ifdef __cplusplus
extern "C" {
#endif
struct Node{
    		int key;
    		int value;
    		Node* next;
};

class List
{
 public:
		
	List();

	~List();

 	Node*  newListNode(int key, int value);
	bool exists(int key, int *value);
	void addToEndList(int key, int value);
	void printList();
	bool empty();
	Node* first();
 private:
	int m_numOfNodes;
	Node* m_first;
	Node* m_last;
	Node* m_current;
};




#ifdef __cplusplus
}
#endif

#endif //SD2016_DOUBLE_LIST_H
