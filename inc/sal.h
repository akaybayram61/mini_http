#ifndef SAL_H_
#define SAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/* --------------------------------------------------------------------------*/
/* ----------------------------SAL_DARRAY HEADER------------------------------*/
/* --------------------------------------------------------------------------*/
#define SAL_DYNAMIC_ARRAY_BASE_CAPACITY 64
#define sal_darray_len(array) (array ? ((SalDynArrayData__*)array - 1)->size : 0)
#define sal_darray_free(array) free(((SalDynArrayData__*)array) - 1)
#define sal_darray_get_info__(ptr) (((SalDynArrayData__ *)ptr) - 1)
#define sal_darray_get_element_size__(ptr) (((SalDynArrayData__ *)ptr) - 1)->element_size
#define sal_darray_pop(array) (sal_darray_get_info__(array)->size--)

typedef enum { /* Yeni tip sistemi için */
	INT8,
	UINT8,
	INT16,
	UINT16,
	INT32,
	UINT32,
	FLOAT,
	DOUBLE
}DataType;

typedef struct{
    uint32_t capacity;
    uint32_t size;
    uint32_t element_size;
} SalDynArrayData__;

void *sal_darray_new(uint32_t element_size);
void sal_darray_append(void** array, void *value);
void sal_darray_delete(void *array, uint32_t indeks);
void sal_darray_copyall(void **array, void* value, uint32_t nelem);

/* --------------------------------------------------------------------------*/
/* ----------------------------SAL_QUEUE HEADER------------------------------*/
/* --------------------------------------------------------------------------*/
#define sal_queue_size(queue) queue->size
#define sal_queue_cap(queue) queue->capacity

typedef struct SalQueueNode{
    void *data;
    struct SalQueueNode *next;
}SalQueueNode;

typedef struct SalQueue{
    uint32_t element_size;
    uint32_t capacity;
    uint32_t size;
    SalQueueNode *first;
    SalQueueNode *last;
}SalQueue;

SalQueue* sal_queue_new(uint32_t element_size);
void sal_queue_take(SalQueue *queue, void *data);
void sal_queue_add(SalQueue *queue, void *data);
void sal_queue_free(SalQueue *queue);
void sal_queue_setcap(SalQueue *queue, uint32_t limit);

#ifdef SAL_IMPLEMENTATION
/* --------------------------------------------------------------------------*/
/* ------------------- SAL dynamic array implementation ---------------------*/
/* --------------------------------------------------------------------------*/
void *sal_darray_new(uint32_t element_size){
    SalDynArrayData__ *result;
    result = malloc(SAL_DYNAMIC_ARRAY_BASE_CAPACITY * element_size + sizeof(SalDynArrayData__));
    if(result == NULL){
        printf("Dynamic array allocation error!\n");
        return NULL;
    }
    result->capacity = SAL_DYNAMIC_ARRAY_BASE_CAPACITY;
    result->size = 0;
    result->element_size = element_size;

    return ++result;
}

static inline int sal_pow(int base, int exp) {
    int result = 1;
    while(exp) { result *= base; exp--; }
    return result;
}

static void sal_darray_resize(void **array, uint32_t nelem) {
    SalDynArrayData__ *info_ptr = sal_darray_get_info__(*array);

    uint32_t min_size = info_ptr->size + nelem;
    // resize if needed
    if(min_size >= info_ptr->capacity) {
	SalDynArrayData__ *holder_ptr;
	int i = 2;
	uint32_t new_capacity = (info_ptr->capacity * 2);
        while (min_size >= new_capacity) {
	    new_capacity = (info_ptr->capacity * sal_pow(2, i));
	    ++i;
	}
	
	holder_ptr = realloc(info_ptr,
			     (new_capacity * info_ptr->element_size) + sizeof (SalDynArrayData__));

        assert(holder_ptr != NULL);
	if(holder_ptr == NULL){
	    printf("realloc error!\n");
	    return;
	}
	
	info_ptr = holder_ptr;
	++holder_ptr;
	info_ptr->capacity = new_capacity;
	*array = holder_ptr;
    }
} 

void sal_darray_copyall(void **array, void* value, uint32_t nelem) {
    sal_darray_resize(array, nelem);
    SalDynArrayData__ *info_ptr = sal_darray_get_info__(*array);
    void *data_ptr = *array;

    memmove(data_ptr + (info_ptr->size * info_ptr->element_size), 
           value,
           info_ptr->element_size * nelem);
    info_ptr->size += nelem;
}

void sal_darray_append(void** array, void *value){

    // resize if needed
    sal_darray_resize(array, 1);
    SalDynArrayData__ *info_ptr = sal_darray_get_info__(*array);
    uint8_t *data_ptr = *array;

    memmove(data_ptr + (info_ptr->size * info_ptr->element_size), 
           value,
           info_ptr->element_size);

    ++info_ptr->size;
}

void sal_darray_delete(void *array, uint32_t indeks){
    // memcpy not suitable for this job because of restrict keyword
    uint32_t i;
    for(i = indeks; i < sal_darray_get_info__(array)->size; ++i) {
        memmove(array + (i * sal_darray_get_element_size__(array)), 
              (array + (i + 1) * sal_darray_get_element_size__(array)), 
              sal_darray_get_element_size__(array));
    }
    --sal_darray_get_info__(array)->size;
}


/* --------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------*/
/* ----------------------- SAL queue implementation -------------------------*/
/* --------------------------------------------------------------------------*/
SalQueue *sal_queue_new(uint32_t element_size){
    SalQueue *result = calloc(1, sizeof(SalQueue));
    result->element_size = element_size;
    result->capacity = UINT32_MAX;

    return result;
}

void sal_queue_add(SalQueue *queue, void *data){
    if(queue->size >= queue->capacity){
        return;
    }

    if(queue->size == 0){
        queue->first = calloc(1, sizeof(SalQueueNode));
        queue->last = queue->first;
        queue->first->data = calloc(1, queue->element_size);
        memcpy(queue->first->data, data, queue->element_size);
        queue->size++;
        return;
    }

    queue->last->next = calloc(1, sizeof(SalQueueNode));
    queue->last->next->data = calloc(1, queue->element_size);
    memcpy(queue->last->next->data, data, queue->element_size);
    queue->last = queue->last->next;
    queue->size++;
    return;
}

void sal_queue_take(SalQueue *queue, void *data){
    SalQueueNode *tmp;
    if(queue->size == 0){
        return;
    }

    memcpy(data, queue->first->data, queue->element_size);
    tmp = queue->first->next;
    free(queue->first->data);
    free(queue->first);
    queue->first = tmp;
    queue->size--;
}

void sal_queue_setcap(SalQueue *queue, uint32_t limit){
    queue->capacity = limit;
    return;
}

void sal_queue_free(SalQueue *queue){
    if(queue->size == 0){
        free(queue);
        return;
    }

    for(uint32_t i = 0; i < queue->size; ++i){
        SalQueueNode *tmp = queue->first->next;
        free(queue->first->data);
        free(queue->first);

        queue->first = tmp;
    }

    free(queue);

}
/* --------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------*/

char *sal_read_file(char *file_name){
    char *result;
    size_t file_size;

    FILE *file = fopen(file_name, "rb");
    if(file == NULL){
	fprintf(stderr, "File open error!\n");
	return NULL;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result = malloc(file_size);
    if(result == NULL){
	fprintf(stderr, "Not Enough Memory!\n");
	return NULL;
    }

    fread(result, file_size, 1, file);
    fclose(file);
    return result;
}
#endif // SAL_IMPLEMENTATION

#endif // SAL_H_
