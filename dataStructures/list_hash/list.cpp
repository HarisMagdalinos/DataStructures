#include "list.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>

List::List()
 :m_numOfNodes(0),m_first(NULL),m_last(NULL),m_current(NULL)
 {}

List::~List()
{
	while(!empty())
	{
		Node* temp=m_first;
		m_first=m_first->next;
		delete temp;
		m_numOfNodes--;
	}
}

Node* List::first()
{
	return m_first;
}

bool List::empty()
{
	return !m_numOfNodes;
}

Node* List::newListNode(int key, int value){
	Node* temp=new Node;
	temp->key=key;
	temp->value=value;
	temp->next=NULL;
	return temp;
}

bool List::exists(int key, int* value){
    if(!empty())
    {
	Node* temp=m_first;
	while(temp->next!=NULL && temp->key!=key)
	{
		temp=temp->next;
	}
	if(temp->next==NULL)
	{
		return 0;
	}
	if(key==temp->key)
	{
		*value=temp->value;
		return 1;
	}
    }
    else
    {
	return 0;
    }
}


void List::addToEndList(int key, int value){

	if(empty())
	{
		m_first=newListNode(key,value);
		m_last=m_first;
		m_numOfNodes++;
	}
	else
	{
		Node* temp=newListNode(key,value);
		m_last->next=temp;
		m_last=temp;
		m_numOfNodes++;
	}
}

void List::printList(){
	if(!empty())
	{
		m_current=m_first;
		for(int i=0; i < m_numOfNodes-1; i++)
		{
			std::cout << "(" <<m_current->key << ",";
			std::cout << m_current->value <<")" <<" | ";
			m_current=m_current->next;
		}
		std::cout << "( " <<m_current->key << ",";
		std::cout << m_current->value << ")" <<std::endl;
	}
}


