// buddyallocator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "type.h"
#include "bda.h"
#include "zvector.h"
#include <vector>
using namespace std;

#define MAXM 50000
char freeregion[MAXM];
char buddylists[MAXM];


#define TOPLEVEL 16




void main2()
{
    buddyallocator mb(freeregion,buddylists,16);

    PVOID addr = mb.alloc(1);
    mb.showlevel();
    PVOID addr1 = mb.alloc(100);
    PVOID addr2 = mb.alloc(100);
    PVOID addr3 = mb.alloc(100);
    mb.showlevel();
   
    mb.free(addr1, 100);

    mb.free(addr2, 100);
    mb.free(addr3, 100);
    mb.free(addr, 1);
    mb.showlevel();
}

#pragma pack(push)
typedef union a {
    uint8_t val;
    struct {
        uint8_t bit0 : 1;
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
    } x;
    
} b;
#pragma pack(pop)

#pragma pack(push)
typedef union _gdtentry {
    uint64_t descriptor;
    struct {
        uint64_t limit : 16;
        uint64_t base : 24;
        uint64_t accessbit : 8;
        uint64_t limit2 : 4;
        uint64_t flags : 4;
        uint64_t base2 : 8;
    } x;

} gdtentry;
#pragma pack(pop)

gdtentry example[10];

#pragma pack(push)
typedef struct _gdtr {
    uint16_t len;
    uint32_t address;
}gdtr;
#pragma pack(pop)

gdtr m;
int gdtentrytotal = 0;

void add(uint64_t baseaddress, uint64_t lmt, uint64_t flags, uint64_t access) {
    example[gdtentrytotal].x.base = (baseaddress & 0xffffff);
    example[gdtentrytotal].x.limit = (lmt & 0xffff);
    example[gdtentrytotal].x.base2 = ((baseaddress >> 24) & 0xff);
    example[gdtentrytotal].x.limit2 = ((lmt >> 16) & 0xf);
    example[gdtentrytotal].x.flags = flags;
    example[gdtentrytotal].x.accessbit = access;
    ++gdtentrytotal;
    m.address = (uint32_t)&example;
    m.len = ((char*)&example[gdtentrytotal]) - ((char*)example + 1);
}

void show(int idx) {
    uint32_t t1 = example[idx].x.base;
    t1 = t1 | (example[idx].x.base2 << 24);
    uint32_t t2 = example[idx].x.limit;
    t2 = t2 | (example[idx].x.limit2 << 16);
    cout << t1 << " " << t2 << endl;
    printf("0x%X 0x%X\n", t1, t2);
    printf("0x%.16llX\n", example[idx].descriptor); ;
}

int main4()
{
    buddyallocator mb(freeregion, buddylists, 16);
    zvector<int> u(&mb);
    for (int i = 0; i < 100; ++i)
        u.push_back(i);
    for (int i = 0; i < 99; ++i)
        u.pop_back();
    //add((uint32_t)freeregion, (uint32_t)freeregion);
    
    return 0;
}


#define GDT_PRESENT (1<<7)
#define GDT_PRIV(X) ((X&0x3)<<5)
#define GDT_TYPE (1<<4)
#define GDT_EXECUTE (1<<3)

#define GDT_READ 0
#define GDT_WRITE (1<<1)
#define GDT_ACCESS (1<<0)

#define GDT_SIZE (1<<2)
#define GDT_GR (1<<3)


int
main(void)
{
    add(0, 0, 0, 0);
    add(0, 0x000FFFFF, GDT_SIZE | GDT_GR, GDT_PRESENT | GDT_TYPE | GDT_EXECUTE | GDT_WRITE);
    add(0, 0x000FFFFF, GDT_SIZE | GDT_GR, GDT_PRESENT | GDT_TYPE  | GDT_WRITE);

    /*add(0, 0x000FFFFF, GDT_SIZE | GDT_GR, GDT_PRESENT | GDT_TYPE | GDT_EXECUTE | GDT_WRITE | GDT_PRIV(3));
    add(0, 0x000FFFFF, GDT_SIZE | GDT_GR, GDT_PRESENT | GDT_TYPE | GDT_WRITE | GDT_PRIV(3));*/

    show(0);
    show(1);
    show(2);
 
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
