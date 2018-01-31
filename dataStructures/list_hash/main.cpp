#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "list.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char*argv[]) {
	
	Hash hashA(13);
	hashA.insertRecord(1,2);
	hashA.print();
	hashA.insertRecord(1,2);
	hashA.print();
	hashA.insertRecord(1,2);
	hashA.print();
	hashA.insertRecord(1,2);
	hashA.print();
	hashA.insertRecord(1,2);
	hashA.print();
    return 0;
}
