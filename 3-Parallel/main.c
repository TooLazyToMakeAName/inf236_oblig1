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
    int rank, comm_sz;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Op op;
    int* senderBuff;
    int  params[4] = {0,0,0,0};
    struct vars var;
    if(rank == 0) {
        var = getFunctionFromFile(argv[1]);
        params[0] = var.lenBoolString;
        params[1] = var.lenBranch;
        params[2] = setSize;
        params[3] = var.numCorrectAws;
        MPI_Bcast(params, 4, MPI_INT, 0, MPI_COMM_WORLD);
        senderBuff = var.triplets;
    }else{
        MPI_Bcast(params, 4, MPI_INT, 0, MPI_COMM_WORLD);
        var.lenBoolString = params[0];
        var.lenBranch = params[1];
        setSize = params[2];
        var.numCorrectAws = params[3];
        operator = malloc(sizeof(int)*setSize*setSize);
    }

    getBinaryString(&var, argv[2]);
    var.lenBranch = var.lenBranch/comm_sz;
    var.triplets = malloc(sizeof(int)*var.lenBranch*3);
    MPI_Bcast(operator,setSize*setSize,  MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(senderBuff,var.lenBranch*3, MPI_INT, var.triplets ,var.lenBranch*3, MPI_INT, 0, MPI_COMM_WORLD);
    int value = 0;
    int p = var.lenBranch/100;
    int bar = 0;
    for (int i = 0; i < var.lenBranch; ++i) {	
	if(rank==0){
	    if((i-bar)> p){
		printf("=");
		fflush(stdout);
		bar = i;
		}
	}
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
    int aws;
    MPI_Op_create((MPI_User_function*)doMapping, 1, &op);
    MPI_Reduce(&value, &aws, 1, MPI_INT, op, 0, MPI_COMM_WORLD);
    MPI_Op_free(&op);
    if (rank==0){
    int state = 0;
    for (int i = 0; i < var.numCorrectAws; ++i) {
	if(aws == var.goodState[i]){
		state = 1;
    	}
    }
    printf("%d", state);
    }
    //*/
    MPI_Finalize();
    return 0;
}
