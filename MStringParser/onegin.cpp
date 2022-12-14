//
// Created by vladimir on 26.08.22.
//

#include "onegin.h"

long int getFileSize(const char *fileAddress) {
    assert(fileAddress != nullptr);

    struct stat fileStat = {};
    stat(fileAddress, &fileStat);

    return fileStat.st_size;
}

char *readTextToBuffer(FILE *file, long int fileSize) {
    assert(fileSize >= 0);

    char *buffer = (char *) calloc((size_t) fileSize + 1, sizeof(char)); //fileSize can't be negative!
    assert(buffer != nullptr);

    fread(buffer, sizeof(char), (size_t) fileSize + 1, file); //fileSize can't be negative!

    return buffer;
}

long int countLines(char *buffer, long int fileSize, char pivot) {
    long int stringAmount = 0;

    for (long int i = 0; i <= fileSize; i++) {
        if (buffer[i] == '\n' || buffer[i] == '\0' || buffer[i] == pivot) {
            stringAmount++;
        }
    }

    return stringAmount;
}

void chopLines(char *buffer, char **strings, long int fileSize, char pivot) {
    assert(strings != nullptr);

    long int stringsIndex = 1;
    strings[0] = &buffer[0];

    for (long int i = 0; i < fileSize; i++) {
        if (buffer[i] == '\n' || buffer[i] == '\0' || buffer[i] == pivot) {
            buffer[i] = '\0';
            strings[stringsIndex] = &buffer[i + 1];

            stringsIndex++;
        }
    }
}

Strings fromFile(const char *fileAddress) {
    FILE *file = fopen(fileAddress, "rb+");

    assert(file != nullptr);
    long int fileSize = getFileSize(fileAddress);
    char *buffer = readTextToBuffer(file, fileSize);

    long int stringAmount = countLines(buffer, fileSize);

    char **strings = (char **) calloc((size_t) stringAmount, sizeof(char *)); //stringAmount can't be negative!
    assert(strings != nullptr);

    chopLines(buffer, strings, fileSize);
    fclose(file);

    return {.array = strings, .buffer = buffer, .bufferSize = fileSize, .size = stringAmount};
}

void writeToFile(FILE *file, const char *string) {
    assert(file != nullptr);
    fprintf(file, "%s\n", string);
}

void writeToFile(const char *fileAddress, const Strings *strings, const char *openArgs) { 
    FILE *file = fopen(fileAddress, openArgs);

    assert(file != nullptr);

    for (long int i = 0; i < strings->size; i++) {
        writeToFile(file, strings->array[i]);
    }
    writeToFile(file, "\n");

    fclose(file);
}

void writeBytesToFile(char *buffer, long int size, const char *fileAddress, const char *openArgs) {
    FILE *file = fopen(fileAddress, openArgs);
    for (int i = 0; i < size; i++) {
        if (buffer[i] != '\0') {
            fprintf(file, "%c", buffer[i]);
        } else {
            fprintf(file, "%c", '\n');
        }
    }
}


void printStringArray(Strings *strings) {
    assert(strings != nullptr);
    for (long int i = 0; i < strings->size; i++) {
        printf("%s\n", strings->array[i]);
    }
}

int compareString(void *v1, void *v2) {
    assert(v1 != nullptr && v2 != nullptr);
    char *string1 = (char *) v1, *string2 = (char *) v2;
    assert(string1 != nullptr && string2 != nullptr);

    while (*string1 != '\0' || *string2 != '\0') {
        while (!isalnum(*string1) && *string1 == ' ') {
            string1++;
        }
        while (!isalnum(*string2) && *string2 == ' ') {
            string2++;
        }

        int result = *string1 - *string2;
        if (result != 0) {
            return result;
        }

        if (*string1 == '\0') {
            return 1;
        }
        if (*string2 == '\0') {
            return -1;
        }

        string1++;
        string2++;
    }

    return 0;
}

int compareBytes(int char1, int char2) {
    int res1 = 0, res2 = 0;

    for (int i = 0; i < 8; i++) {
        res1 += ((!!((char1 << i) & 0x80))  << (8 - i - 1)); // i bit of a byte
        res2 += ((!!((char2 << i) & 0x80))  << (8 - i - 1));
    }

    return res1 - res2;
}

int compareFlipped(void *v1, void *v2) {
    assert(v1 != nullptr && v2 != nullptr);
    char *string1 = (char *) v1, *string2 = (char *) v2;
    assert(string1 != nullptr && string2 != nullptr);

    int count1 = 0, count2 = 0;

    while (*string1 != '\0') {
        string1++;
        count1++;
    }
    while (*string2 != '\0') {
        string2++;
        count2++;
    }

    while (count1 >= 0 || count2 >= 0) {
        while (!isalnum(*string1) && (!!((*string1 << 0) & 0x80)) == 0) {
            count1--;
            string1--;
        }
        while (!isalnum(*string2) && (!!((*string2 << 0) & 0x80)) == 0) {
            count2--;
            string2--;
        }

        if (*string1 & 0x80) { //first bit of byte
            if (*string2 & 0x80) {
                int secondPart = compareBytes(*string1, *string2);

                string1--;
                string2--;

                count1--;
                count2--;

                int firstPart = compareBytes(*string1, *string2);
                if (firstPart > 0) {
                    return 1;
                } else if (firstPart < 0) {
                    return -1;
                } else {
                    if (secondPart != 0) {
                        return secondPart;
                    }
                }
            } else /* if !(*string2 & 0x80)*/{
                return 1;
            }
        } else {
            if (*string2 & 0x80) {
                return -1;
            } else {
                int result = compareBytes(*string1, *string2);

                if (result != 0) {
                    return result;
                }
            }
        }

        if (count1 <= 0) {
            break;
        }
        if (count2 <= 0) {
            break;
        }

        count1--;
        count2--;
        string1--;
        string2--;
    }

    return 0;
}

void quickSort(char **strings, int (*comparator)(void *v1, void *v2), long int first, long int last) {
    assert(strings != nullptr);

    if (first < last) {
        long int pivot = first;
        long int l = first, r = last - 1;

        while (l < r) {
            while (comparator(strings[l], strings[pivot]) < 0 && l < last) {
                l++;
            }
            while (comparator(strings[r], strings[pivot]) > 0) {
                r--;
            }

            if (l < r) {
                char *tmp = strings[l];
                strings[l] = strings[r];
                strings[r] = tmp;

                l++;
                r--;
            }
        }

        char *tmp = strings[pivot];
        strings[pivot] = strings[r];
        strings[r] = tmp;

        quickSort(strings, comparator, first, r - 1);
        quickSort(strings, comparator, r + 1, last);
    }
}

void sortAsc(Strings *strings) {
    assert(strings != nullptr);
    quickSort(strings->array, compareString, 0, strings->size);
}

void sortReversed(Strings *strings) {
    assert(strings != nullptr);
    quickSort(strings->array, compareFlipped, 0, strings->size);
}

// void resetComposition(Strings *strings) {
//     assert(strings != nullptr);
//     for (long int i = 0; i < strings->size; i++) {
//         strings->composition[i] = i;
//     }
// }

void freeStrings(Strings *strings) {
    free(strings->array);
    free(strings->buffer);
}