#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// lscpu


struct List {
    struct List* next;
    size_t index;
};

struct MeasureList {
    struct MeasureList* next;
    size_t index;
};

typedef struct List List;
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
    size_t size = 8;
    const size_t innerLoopsAmount = 1000;
    size_t k;
    size_t maxSize = 4;
//    size_t temp;

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
        gettimeofday(&start, &tz);
        while (--k > 0) {
            randomIndexes = randomIndexes->next;
            while (randomIndexes != head) {
                //        if (randomIndexes == NULL)
                //            printf("ERROR\n");
                //        printf("%d ", (int)randomIndexes->index);
                randomIndexes = randomIndexes->next;
            }
        }
        gettimeofday(&finish, &tz);

        delta.tv_usec = finish.tv_usec - start.tv_usec;
        delta.tv_sec = finish.tv_sec - start.tv_sec;
        double resultTime = ((double)(delta.tv_sec * 1000000 + delta.tv_usec)) / (innerLoopsAmount * size);
        printf("%f mc\n", resultTime);
        fprintf(results, "%ld %f\n", sizeof(MeasureList) * size, resultTime); 
        fflush(results);

        freeMeasureList(size);
        size = (int) (size * 1.3);
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

List* initListElement(List* parent, size_t value) {
    List* newListEl = (List*) malloc(sizeof(List));
    newListEl->next = NULL;
    newListEl->index = value;

    if (parent != NULL) {
        parent->next = newListEl;
    }
    return newListEl;
}

List* getListAfterNSteps(List* current, size_t steps, List **prev) {
    while (steps-- > 0) {
        *prev = current;
        current = current->next;
    }
    return current;
}

void deleteListEl(List* listEl, List* prev) {
    if (prev != NULL)
        prev->next = listEl->next;
}

MeasureList* prepare(const size_t size) {
    measureArray = (MeasureList**) malloc(sizeof(MeasureList*) * size);
    size_t i;
    for (i = 0; i < size; ++i) {
        measureArray[i] = (MeasureList*) malloc(sizeof(MeasureList));
        measureArray[i]->index = i;
    }
    
    List* head = initListElement(NULL, 0);
    List* current = head;
    for (i = 1; i < size; ++i) {
        current = initListElement(current, i);
    }
    current->next = head;
    current = head;

    List* temp;
    size_t randInt;
    MeasureList* resultHead = NULL;
    MeasureList* last = resultHead; 
    List* prev;
    for (i = 0; i < size; ++i){
        randInt = rand() % (size - i);
        temp = current = getListAfterNSteps(current, randInt, &prev);
        deleteListEl(current, prev);
        if (last != NULL) {
            last->next = measureArray[current->index];
        } else {
            resultHead = measureArray[current->index];
        }
        last = measureArray[current->index];
        current = current->next;
        free(temp);
    }
    last->next = resultHead;
    return resultHead;
}
