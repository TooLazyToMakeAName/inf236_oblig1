#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Automa {
    int *initState;
    int stateLengths;
};

int *getFunctionFromFile(char *filename) {
    int *fx = malloc(sizeof(int) * (8));
    char *subStirng;
    char *line = NULL;
    size_t lineBufflen = 0;
    size_t lineLen = 0;
    FILE *fp;
    fp = fopen("test.txt", "r");
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

int computeFx(int *fx, int x, int y, int z) {
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

struct Automa pasreInitState(char *filename) {
    char* line = NULL;
    size_t lineBufflen = 0;
    size_t lineLen = 0;
    FILE *fp;
    fp = fopen("test2.txt", "r");
    if (fp == NULL) {
        printf("File Not Found 404");
        exit(-1);
    }
    lineLen = getline(&line, &lineBufflen, fp);
    int itt = 0;
    sscanf(line, "%d", &itt);
    lineLen = getline(&line, &lineBufflen, fp);
    int *initState = malloc(sizeof(int) * itt);
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

int main(int argc, char **argv) {
    struct Automa automa = pasreInitState(argv[1]);
    int* stateArray = malloc(sizeof(int)*automa.stateLengths);
    int *fx = getFunctionFromFile(argv[2]);
    int t;
    sscanf(argv[3], "%d", &t);
    for (int i = 0; i < t; ++i) {
        for (int j = 0; j < automa.stateLengths; ++j) {
            int x = mod(j - 1, automa.stateLengths);
            int y = mod(j, automa.stateLengths);
            int z = mod(j + 1, automa.stateLengths);
            int val = computeFx(fx, automa.initState[x], automa.initState[y], automa.initState[z]);
            stateArray[j] = val;
            printf("%d", val);
        }
        int* temp = automa.initState;
        automa.initState = stateArray;
        stateArray = temp;
        printf("\n");
    }
    free(fx);
    free(automa.initState);
    free(stateArray);
    return 0;
}
