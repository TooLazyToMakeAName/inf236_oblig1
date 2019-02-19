#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <time.h>	
//#include <sys/types.h>

struct Automa {
    int* initState;
    int stateLengths;
};

int* getFunctionFromFile(char* filename) {
    int* fx = malloc(sizeof(int) * (9));
    char* subStirng;
    char* line = NULL;
    size_t lineBufflen = 0;
    size_t lineLen = 0;
    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File Not Found 404");
        exit(-1);
    }
    int count = 0;
    while ((lineLen = getline(&line, &lineBufflen, fp)) != -1) {
        strtok(line, " ");
        subStirng = strtok(NULL, " ");
        int y = *subStirng - '0';
        fx[count] = y;
        count++;
    }
    free(line);
    return fx;
}

int computeFx(int* fx, int x, int y, int z) {
    x = x << 2;
    y = y << 1;
    int index = x | y | z;
    return fx[index];
}

int mod(int num, int base) {
    while (num < 0) {
        num += base;
    }
    return num % base;
}

struct Automa pasreInitState(char* filename) {
    char* line = NULL;
    size_t lineBufflen = 0;
    size_t lineLen = 0;
    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File Not Found 404");
        exit(-1);
    }
    lineLen = getline(&line, &lineBufflen, fp);
    int itt = 0;
    sscanf(line, "%d", &itt);
    lineLen = getline(&line, &lineBufflen, fp);
    int* initState = malloc(sizeof(int) * itt);
    for (int i = 0; i < itt; ++i) {
        int x = line[i] - '0';
        initState[i] = x;
    }

    free(line);
    //printf("\n%d\n", lineLen);
    struct Automa automa;
    automa.initState = initState;
    automa.stateLengths = itt;
    return automa;
}

void print(int* array, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d", array[i]);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    int rank, comm_sz;
    int* fx;
    struct Automa automa;
    char* fxFile = argv[1];
    char* binaryString = argv[2];
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Request sendRequesStartElement;
    MPI_Request sendRequestEndElement;
    MPI_Status receiveStatusStartElement;
    MPI_Status receiveStatusEndElement;
    unsigned int t;
    clock_t begin;
    sscanf(argv[3], "%d", &t);
    if (rank == 0) {
	begin = clock();
        automa = pasreInitState(binaryString);
        fx = getFunctionFromFile(fxFile);
        fx[8] = automa.stateLengths;
    } else {
        fx = malloc(sizeof(int) * 9);
    }

    MPI_Bcast(fx, 9, MPI_INT, 0, MPI_COMM_WORLD);


    int seqSize = fx[8] / (comm_sz);
    int seqBuffExtendedLength = sizeof(int) * (seqSize + 2);        //adds buffer space for the two odd elemetes
    int* seqBuffExtended = malloc(seqBuffExtendedLength);     
    int* placeHolder = malloc(seqBuffExtendedLength);
    int* seqBuff = &seqBuffExtended[1];
    MPI_Scatter(automa.initState, seqSize, MPI_INT, seqBuff, seqSize, MPI_INT, 0, MPI_COMM_WORLD);
    int p = t/100;
    int bar = 0;
    for (int i = 0; i < t; ++i) {
	if(rank==0){
	    if((i-bar)> p){
		printf("=");
		fflush(stdout);
		bar = i;
		}
	}
        MPI_Isend(&seqBuff[0], 1, MPI_INT, mod(rank - 1, comm_sz), t+1 , MPI_COMM_WORLD, &sendRequesStartElement);
        MPI_Isend(&seqBuff[seqSize - 1], 1, MPI_INT, mod(rank + 1, comm_sz), t, MPI_COMM_WORLD, &sendRequestEndElement);
        MPI_Recv(&seqBuffExtended[0], 1, MPI_INT, mod(rank - 1, comm_sz), t, MPI_COMM_WORLD, &receiveStatusStartElement);
        MPI_Recv(&seqBuffExtended[seqSize + 1], 1, MPI_INT, mod(rank + 1, comm_sz), t+1 , MPI_COMM_WORLD, &receiveStatusEndElement);

        for (int j = 1; j < seqSize + 1; ++j) {
            placeHolder[j] = computeFx(fx, seqBuffExtended[j - 1], seqBuffExtended[j], seqBuffExtended[j + 1]);
        }

        int* temp = placeHolder;
        placeHolder = seqBuffExtended;
        seqBuffExtended = temp;
        seqBuff = &seqBuffExtended[1];


    }
    MPI_Gather(seqBuff, seqSize, MPI_INT, automa.initState, seqSize, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("\n%.6f\n", time_spent);
        free(automa.initState);
    }

    //*/
    MPI_Finalize();
    free(fx);

    return 0;
}
