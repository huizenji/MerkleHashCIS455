#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// helper function
char* computeHashTree(char **array, int count);

// find and hash files using md5
char* merkle_hash_tree() {
    // run script to find directory and hash all files
    system("bash hashscript");

    FILE *fp;
    int file_count = 0;
    char c;
    fp = fopen("initialhash", "r");

    // check if file exists
    if (fp == NULL){
        printf("Computing initial hash has failed");
        return 0;
    }

    // calculate number of files (array size)
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            file_count = file_count + 1;
    fclose(fp);

    // copy values to array, handle odd numbers
    int odd = 0; // boolean
    if (file_count % 2 != 0){
        char * initialArray[file_count + 1];
        odd = 1;
    }
    char * initialArray[file_count];

    // loop through initialhash to store hashes in array
    fp = fopen("initialhash", "r");
    for (int i = 0; i < file_count; i++){
        initialArray[i] = malloc(32 * sizeof(char));
        fscanf(fp, "%s", initialArray[i]);
    }
    fclose(fp);

    // if odd number, then duplicate last hash
    if (odd){
        initialArray[file_count] = initialArray[file_count - 1];
        file_count++;
    }

    // compute binary tree hashing
    return computeHashTree(initialArray, file_count);
}

// recursive function that computes up to the final hash
char* computeHashTree(char **array, int count){

    // end condition (all files couple-hashed together)
    if (count == 1){
        return array[0];
    }

    // handle odd number of files
    int odd = 0; // boolean
    int arraySize = count / 2;
    if (count % 2 != 0) {
        odd = 1;
        arraySize++;
    }

    // concat couple hashes together, store in temp array for next recursion
    char * tempArray[arraySize];
    int index = 0;
    for (int i = 0; i < count - 1; i = i + 2){
        char concat[64];
        strcpy(concat, array[i]);
        strcat(concat, array[i+1]);

        // hash it with md5
        FILE *fp, *fp2;
        fp = fopen("tempHash", "w");
        fprintf(fp, "%64s", concat);
        fclose(fp);
        fp = NULL;

        system("md5sum tempHash | cut -f1 | tee tempHash2");
        fp2 = fopen("tempHash2", "r");
        tempArray[index] = malloc(32 * sizeof(char));
        fscanf(fp2, "%32s", tempArray[index]);
        fclose(fp2);
        fp2 = NULL;
        index++;
    }

    if (odd){
        tempArray[arraySize - 1] = array[count - 1];
    }
    array = NULL;

    return computeHashTree(tempArray, arraySize);
}

// give us the final hash
int main() {
    printf("\nTop Hash: %s\n", merkle_hash_tree());
    return 0;
}