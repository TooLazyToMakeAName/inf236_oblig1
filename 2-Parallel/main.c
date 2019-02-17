#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

struct Automa {
    int* initState;
    int stateLengths;
};

int* getFunctionFromFile(char* filename) {
    int* fx = malloc(sizeof(int) * ((2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2) + 1));
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

int computeFx(int* fx, int* bits) {
    int index = 0;
    for (int i = 8; i >= 0; --i) {
        index = index | bits[i] << i;
    }
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
    int* initState = malloc(sizeof(int) * itt * itt);
    for (int j = 0; j < itt; ++j) {
        lineLen = getline(&line, &lineBufflen, fp);
        for (int i = 0; i < itt; ++i) {
            int x = line[i] - '0';
            initState[(j * itt) + i] = x;
        }
    }
    free(line);
    //printf("\n%d\n", lineLen);
    struct Automa automa;
    automa.initState = initState;
    automa.stateLengths = itt;
    return automa;
}

void print(int* array, int size) {
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < size; ++i) {
            printf("%d", array[(size * j) + i]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argvc, char** argv) {
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
    sscanf(argv[3], "%d", &t);
    if (rank == 0) {
        automa = pasreInitState(binaryString);
        fx = getFunctionFromFile(fxFile);
        fx[2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2] = automa.stateLengths;
    } else {
        fx = malloc(sizeof(int) * ((2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2) + 1));
    }

    MPI_Bcast(fx, 1 + (2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2), MPI_INT, 0, MPI_COMM_WORLD);

    int sideLength = fx[2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2];
    int sideHigth = sideLength / comm_sz;
    int seqBuffExtendedLength =
            sizeof(int) * (sideLength) * (sideHigth + 2);        //adds buffer space for the two odd elemetes
    int* seqBuffExtended = malloc(
            seqBuffExtendedLength);                         //remember that everyting is now 1 index (ehh)
    int* placeHolder = malloc(seqBuffExtendedLength);
    int* seqBuff = &seqBuffExtended[sideLength];
    int* bitSequence = malloc(sizeof(int) * 9);
    MPI_Scatter(automa.initState, sideHigth * sideLength, MPI_INT, seqBuff, sideHigth * sideLength, MPI_INT, 0,
                MPI_COMM_WORLD);

    for (int i = 0; i < t; ++i) {
        MPI_Isend(&seqBuff[0], sideLength, MPI_INT, mod(rank - 1, comm_sz), 0, MPI_COMM_WORLD,
                  &sendRequesStartElement);
        MPI_Isend(&seqBuff[(sideHigth - 1) * sideLength], sideLength, MPI_INT, mod(rank + 1, comm_sz), 1,
                  MPI_COMM_WORLD, &sendRequestEndElement);
        MPI_Recv(&seqBuffExtended[0], sideLength, MPI_INT, mod(rank - 1, comm_sz), 1, MPI_COMM_WORLD,
                 &receiveStatusStartElement);
        MPI_Recv(&seqBuffExtended[(sideLength * (sideHigth + 1))], sideLength, MPI_INT, mod(rank + 1, comm_sz),
                 0, MPI_COMM_WORLD, &receiveStatusEndElement);


        for (int y = 1; y < sideHigth + 1; ++y) {
            for (int x = 0; x < sideLength; ++x) {
                bitSequence[0] = seqBuffExtended[(mod(x - 1, sideLength)) + ((y - 1) * sideLength)];
                bitSequence[1] = seqBuffExtended[(mod(x, sideLength)) + ((y - 1) * sideLength)];
                bitSequence[2] = seqBuffExtended[(mod(x + 1, sideLength)) + ((y - 1) * sideLength)];
                bitSequence[3] = seqBuffExtended[mod((x - 1), sideLength) + ((y) * sideLength)];
                bitSequence[4] = seqBuffExtended[mod((x), sideLength) + ((y) * sideLength)];
                bitSequence[5] = seqBuffExtended[mod((x + 1), sideLength) + ((y) * sideLength)];
                bitSequence[6] = seqBuffExtended[(mod(x - 1, sideLength)) + ((y + 1) * sideLength)];
                bitSequence[7] = seqBuffExtended[(mod(x, sideLength)) + ((y + 1) * sideLength)];
                bitSequence[8] = seqBuffExtended[(mod(x + 1, sideLength)) + ((y + 1) * sideLength)];

                placeHolder[x+(y*sideLength)] = computeFx(fx,bitSequence);
            }
        }

        int* temp = placeHolder;
        placeHolder = seqBuffExtended;
        seqBuffExtended = temp;
        seqBuff = &seqBuffExtended[sideLength];
        //}

        MPI_Gather(seqBuff, sideLength*sideHigth, MPI_INT, automa.initState, sideHigth*sideLength, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            print(automa.initState, automa.stateLengths);
        }

    }

    MPI_Finalize();
    free(seqBuffExtended);
    free(fx);

    return 0;
}
