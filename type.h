#pragma once
typedef void* PVOID;
#pragma pack(push, 1)
typedef struct _linkedlist {
    struct _linkedlist* flink;
    struct _linkedlist* blink;
    int attributes;
    PVOID addr;
}linkedlist;
#pragma pack(pop)

#define NULL 0

#define ISALLOCATED 1

#pragma pack(push, 1)
typedef struct _buddylist {
    PVOID ptr;
    PVOID ptr2;
    PVOID base;
    linkedlist* a[];
}buddylist;
#pragma pack(pop)