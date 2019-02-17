#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>


int* operator;
int setSize; 
struct vars{
    int lenBoolString, lenBranch, numCorrectAws, x,y, opValue;
    int* triplets;
    int* goodState;
    int* binaryState;
};

struct vars getFunctionFromFile(char* filename) {
    int lenBoolString, lenBranch, numCorrectAws, x,y, opValue;
    int* goodState;
    int* triplets;  
    int* binaryState;
    char* line = NULL;
    size_t lineBufflen = 0;
    size_t lineLen = 0;
    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File Not Found 404");
        exit(-1);
    }
    fscanf(fp, "%d", &lenBoolString);
    fscanf(fp, "%d", &lenBranch);
    fscanf(fp, "%d", &setSize);
    fscanf(fp, "%d", &numCorrectAws);
    goodState = malloc(sizeof(int)*numCorrectAws);
    operator = malloc(sizeof(int)*setSize*setSize);
    triplets = malloc(sizeof(int)*lenBranch*3);
    for(int i = 0; i<setSize*setSize; ++i){
       fscanf(fp, "%d %d %d", &y, &x, &opValue); 
       operator[(setSize*y)+x]= opValue; 
    }
    for(int i = 0; i<numCorrectAws; ++i){
        fscanf(fp, "%d", &opValue);
        goodState[i] = opValue;
    } 
    for(int i = 0; i<lenBranch; ++i){
        fscanf(fp, "%d %d %d", &opValue, &x, &y);
        triplets[3*i] = opValue;
        triplets[(3*i)+1] = x;
	triplets[(3*i)+2] = y;

    }
    struct vars var = {lenBoolString, lenBranch, numCorrectAws, x,y, opValue, triplets, goodState, NULL};
    return var;
    
}

int mod(int num, int base) {
    while (num < 0) {
        num += base;
    }
    return num % base;
}

void getBinaryString(struct vars* var, char* charString) {
    (*var).binaryState = malloc((*var).lenBoolString * sizeof(int));
    for (int i = 0; i < (*var).lenBoolString; ++i) {
        (*var).binaryState[i] = charString[i] - '0';
    }
}

//void *invec, void *inoutvec, int *len, MPI_Datatype *d);
void doMapping(int* opreandOne, int* operandTwo, int len, MPI_Datatype *d) {
    *opreandOne = operator[(setSize * (*opreandOne)) + (*operandTwo)];
}
int main(int argc, char** argv) {
    int comm_sz = 1;
    struct vars var;
        var = getFunctionFromFile(argv[1]);
    getBinaryString(&var, argv[2]);
    var.lenBranch = var.lenBranch/comm_sz;
    var.triplets = malloc(sizeof(int)*var.lenBranch*3);
    int value = 0;
    for (int i = 0; i < var.lenBranch; ++i) {	
	int tripletIndex = i*3;
	int j = var.triplets[tripletIndex];
	int flaseState = var.triplets[tripletIndex+1];	
	int trueState = var.triplets[tripletIndex+2];
	int xj = var.binaryState[j];
	if (xj == 0){
		doMapping(&value, &flaseState,1, NULL);
	}else{

		doMapping(&value, &trueState,1, NULL);
	}
    }
    int aws = value;
    int state = 0;
    for (int i = 0; i < var.numCorrectAws; ++i) {
	if(aws == var.goodState[i]){
		state = 1;
    	}
    printf("%d", state);
    }
    return 0;
}

