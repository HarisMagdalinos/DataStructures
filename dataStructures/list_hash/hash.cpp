/*
 * hash.c
 */

#include "hashtable.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Hash::Hash(unsigned int buckets_number){
	
	m_buckets_number=buckets_number;
	m_buckets=new List*[m_buckets_number];
	for(unsigned int i=0; i < m_buckets_number; i++)
	{
		m_buckets[i]=new List;		
	}	
}

Hash::~Hash()
{
	for(unsigned int i=0; i < m_buckets_number; i++)
	{
		delete m_buckets[i];
	}
	delete m_buckets;
}




void Hash::insertRecord(int key, int value){

    unsigned int hashKey=key%m_buckets_number;
    Node* temp;
    if((temp=getRecord(key))==NULL)
    {
		m_buckets[hashKey]->addToEndList(key,value);
    }
    else
    {
		temp->value++;
    }

}

Node* Hash::getRecord(const int key){
    unsigned int hashKey=key%m_buckets_number;	
    Node* temp=m_buckets[hashKey]->first();
    if(temp==NULL)
    {
	return temp;
    }
    while(temp->next!=NULL && temp->key!= key)
    {
	temp=temp->next;
    }
    return temp;
    
}

void Hash::print(){
	for(int i=0; i<m_buckets_number; i++)
	{
		m_buckets[i]->printList();
	}

}

