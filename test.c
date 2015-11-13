#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// lscpu
size_t size = 8;
double multiplier = 1.3;
const size_t innerLoopsAmount = 200;
size_t maxSize = 8;

struct MeasureList {
    struct MeasureList* next;
//    size_t index;
};

typedef struct MeasureList MeasureList;
MeasureList** measureArray; 

MeasureList* prepare(const size_t);
void freeMeasureList(const size_t);
void prepareCache(size_t, size_t, MeasureList*);

int main() {
    srand(time(NULL));
    struct timeval start, finish, delta;
    struct timezone tz;

    FILE* results = fopen("results", "w");
    size_t k;
//    size_t temp;

    /*
    MeasureList* head_test = prepare(size); 
    size_t i = 0;
    for (i = 0; i < size; ++i) {
        printf("%ld ", (size_t)head_test->index); 
        head_test = head_test->next;
    }
    putchar('\n');
    return 0;
    */

    while ((sizeof(MeasureList) * size / 1024 / 1024) < maxSize) {
        printf("\nSize: %d\n", (int)size);
        printf("Sizeof MeasureList: %ld\n", sizeof(MeasureList));
        printf("Size of all elements: %ld B\n", sizeof(MeasureList) * size);
        printf("                      %ld kB\n", sizeof(MeasureList) * size / 1024);
        printf("                      %ld MB\n", sizeof(MeasureList) * size / 1024 / 1024);

        MeasureList* head = prepare(size); 
        MeasureList* randomIndexes = head;
        
        k = innerLoopsAmount;
        prepareCache(200, size, randomIndexes);
        printf("Start iterating\n");
        double resultTime = 0; 
        while (--k > 0) {
            gettimeofday(&start, &tz);
            randomIndexes = randomIndexes->next;
            while (randomIndexes != head) {
                //        if (randomIndexes == NULL)
                //            printf("ERROR\n");
                //        printf("%d ", (int)randomIndexes->index);
                randomIndexes = randomIndexes->next;
            }
            gettimeofday(&finish, &tz);
            delta.tv_usec = finish.tv_usec - start.tv_usec;
            delta.tv_sec = finish.tv_sec - start.tv_sec;
            resultTime += ((double)(delta.tv_sec * 1000000 + delta.tv_usec)) / size;
        }

        printf("%f mc\n", resultTime);
        fprintf(results, "%ld %f\n", sizeof(MeasureList) * size, resultTime / innerLoopsAmount); 
        fflush(results);

        freeMeasureList(size);
        size = (int) (size * multiplier);
    }
    fclose(results);
}

void inline prepareCache(size_t innerLoopsAmount, size_t size, MeasureList* randomIndexes) {
    size_t i, k;
    for (k = 0; k < innerLoopsAmount; ++k) {
        for (i = 0; i < size; ++i) {
            randomIndexes = randomIndexes->next;
        }
    }
}

void freeMeasureList(const size_t size) {
    size_t i;
    for (i = 0; i < size; ++i) {
        free(measureArray[i]);
    }
    free(measureArray);
    measureArray = NULL;
}

MeasureList* prepare(const size_t size) {
    measureArray = (MeasureList**) malloc(sizeof(MeasureList*) * size);
    size_t i;
//    measureArray[0]->index = 0;
    for (i = 0; i < size; ++i) {
        measureArray[i] = (MeasureList*) malloc(sizeof(MeasureList));
//        measureArray[i]->index = i;
        measureArray[i]->next = NULL;
    }

    
    MeasureList* head = NULL;
    MeasureList *current, *temp;
    size_t randInt;

    /*
    size_t j = 0;
    current = measureArray[0];
    for (j = 0; j < size; ++j) {
        printf("%ld ", (long)current->index);
        current = current->next;
    }
    putchar('\n');
    */

    head = current = measureArray[0];
    for (i = 1; i < size; ++i){
        randInt = rand() % size;
        temp = measureArray[randInt];
        while (temp == current || temp->next != NULL) {
            randInt = (++randInt) % size;
            temp = measureArray[randInt];
        }
        current->next = temp;
        current = current->next;
    }
    current->next = head;
    return head;
}
