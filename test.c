#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// lscpu


struct List {
    struct List* next;
    struct List* prev;
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

int main() {
    srand(time(NULL));
    struct timeval start, finish, delta;
    struct timezone tz;

    FILE* results = fopen("results", "w");
    size_t size = 8;
    size_t innerLoopsAmount = 90;
    size_t maxSize = 4;
    size_t i, k;
    size_t temp;

    while ((sizeof(MeasureList) * size / 1024 / 1024) < maxSize) {
        printf("\nSize: %d\n", (int)size);
        printf("Sizeof MeasureList: %ld\n", sizeof(MeasureList));
        printf("Size of all elements: %ld B\n", sizeof(MeasureList) * size);
        printf("                      %ld kB\n", sizeof(MeasureList) * size / 1024);
        printf("                      %ld MB\n", sizeof(MeasureList) * size / 1024 / 1024);

        MeasureList* randomIndexes = prepare(size); 

        printf("Start iterating\n");
        gettimeofday(&start, &tz);
        for (k = 0; k < innerLoopsAmount; ++k) {
            for (i = 0; i < size; ++i) {
                //        if (randomIndexes == NULL)
                //            printf("ERROR\n");
                //        printf("%d ", (int)randomIndexes->index);
                randomIndexes = randomIndexes->next;
                temp = randomIndexes->index;
            }
        }
        gettimeofday(&finish, &tz);

        delta.tv_usec = finish.tv_usec - start.tv_usec;
        delta.tv_sec = finish.tv_sec - start.tv_sec;
        double resultTime = ((double)(delta.tv_sec * 1000000 + delta.tv_usec)) / (innerLoopsAmount * size);
        printf("%f mc\n", resultTime);
        fprintf(results, "%ld %f\n", sizeof(MeasureList) * size, resultTime); 

        freeMeasureList(size);
        size = (int) (size * 1.3);
    }
    fclose(results);
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
    newListEl->prev = parent;

    if (parent != NULL) {
        parent->next = newListEl;
    }
    return newListEl;
}

List* getListAfterNSteps(List* current, size_t steps) {
    while (steps-- > 0) {
        current = current->next;
    }
    return current;
}

void deleteListEl(List* listEl) {
    if (listEl->prev != NULL)
        listEl->prev->next = listEl->next;
    if (listEl->next != NULL)
        listEl->next->prev = listEl->prev;
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
    head->prev = current;
    current->next = head;
    current = head;

    List* temp;
    size_t randInt;
    MeasureList* resultHead = NULL;
    MeasureList* last = resultHead; 
    for (i = 0; i < size; ++i){
        randInt = rand() % (size - i);
        temp = current = getListAfterNSteps(current, randInt);
        deleteListEl(current);
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
