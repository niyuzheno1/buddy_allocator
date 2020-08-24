#include "bda.h"
#define DEBUG
#ifdef DEBUG
#define bp() _asm int 3;
#endif
#include <iostream>

using namespace std;
void buddyallocator::inserttail(int level, linkedlist* c) {
    buddylist* tmp = (buddylist*)buddylists;
    if (tmp->a[level] == NULL) {
        tmp->a[level] = c;
        c->flink = c->blink = c;
    }
    else {
        linkedlist* d = (linkedlist*)tmp->a[level];
        c->blink = d;
        c->flink = d->flink;
        d->flink = c;
        ((linkedlist*)c->flink)->blink = c;
    }
}
void buddyallocator::deleteentry(int level, linkedlist* c) {
    buddylist* tmp = (buddylist*)buddylists;
    if (tmp->a[level] == NULL) {
        bp();
    }
    else if (tmp->a[level] == c) {
        if (c->blink == c) {
            tmp->a[level] = NULL;
        }
        else {
            tmp->a[level] = (linkedlist*)c->blink;
        }
        ((linkedlist*)c->flink)->blink = c->blink;
        ((linkedlist*)c->blink)->flink = c->flink;
    }
    else {
        ((linkedlist*)c->flink)->blink = c->blink;
        ((linkedlist*)c->blink)->flink = c->flink;
    }
}

buddyallocator::buddyallocator(PVOID fr, PVOID bd, int tl) {
    freeregion = fr;
    TOPLEVEL = tl;
    buddylists = bd;
    buddylist* h = (buddylist*)buddylists;
    h->ptr = (PVOID)(((char*)buddylists) + sizeof(buddylist) + TOPLEVEL * sizeof(PVOID) + 4);
    h->a[TOPLEVEL - 1] = (linkedlist*)h->ptr;
    linkedlist* t = (linkedlist*)h->ptr;
    t->flink = t->blink = t;
    h->ptr = t + 1;
    t->addr = freeregion;
    h->base = freeregion;
}

size_t buddyallocator::findlevel(size_t x) {
    size_t level = 0;
    for (level = 0; level < TOPLEVEL; ++level) {
        if ((1 << level) > x)
            break;
    }
    return level;
}

PVOID buddyallocator::alloc(size_t s) {
    size_t level = findlevel(s);
    if (level >= TOPLEVEL - 1)
        return NULL;

    buddylist* h = (buddylist*)buddylists;
    while (true) {
        size_t l = level;
        linkedlist* hd;
        int u = 0;
        linkedlist* i = NULL;

        while (l < 16) {
            hd = (linkedlist*)h->a[l];
            if (hd != 0)
            {
                int cnt = 0;
                for (i = (linkedlist*)hd->flink; i != hd || cnt == 0; i = (linkedlist*)i->flink) {
                    if ((i->attributes & ISALLOCATED) == 0)
                    {
                        u = 1;
                        break;
                    }
                    ++cnt;
                }
            }
            if (u == 1)
                break;
            ++l;
        }
        if (u == 1) {
            if (l == level) {

                if (i == NULL) {
                    bp();
                    return i;
                }
                i->attributes = i->attributes | ISALLOCATED;

                return i->addr;
            }
            else {
                if (i == NULL) {
                    bp();
                    return i;
                }
                char* tmp = (char*)h->ptr;
                linkedlist* leftsplit = (linkedlist*)tmp;
                leftsplit->flink = leftsplit;
                leftsplit->blink = leftsplit;
                leftsplit->addr = i->addr;
                tmp += sizeof(linkedlist);
                linkedlist* rightsplit = (linkedlist*)tmp;
                rightsplit->flink = rightsplit;
                rightsplit->blink = rightsplit;
                char* catelog = (char*)i->addr;
                rightsplit->addr = catelog + (1 << (l - 1));
                inserttail( l - 1, leftsplit);
                inserttail( l - 1, rightsplit);
                deleteentry( l, i);
                tmp += sizeof(linkedlist);
                h->ptr = tmp;
            }
        }
    }
}



void buddyallocator::showlevel() {
    buddylist* h = (buddylist*)buddylists;
    for (int l = 0; l < TOPLEVEL; ++l)
    {
        linkedlist* hd = (linkedlist*)h->a[l];
        int cnt = 0;
        if (hd != 0)
            for (linkedlist* i = (linkedlist*)hd->flink; i != hd || cnt == 0; i = (linkedlist*)i->flink) {
                ++cnt;
            }
        cout << l << " : " << cnt << " ";
    }
    cout << endl;
}

void buddyallocator::clean_fragmentation(size_t level, linkedlist* t) {
    linkedlist* i = NULL;
    buddylist* h = (buddylist*)buddylists;
#define lsize (1<<level)
    while (true) {
        int u = 0;
        int cnt = 0;
        int oddoreven = (((char*)t->addr) - ((char*)h->base)) / lsize;
        int whichpath = (oddoreven & 1);
        char* whichaddr = 0;
        if (whichpath) {
            whichaddr = ((char*)t->addr) - lsize;
        }
        else {
            whichaddr = ((char*)t->addr) + lsize;
        }
        for (i = t; i != t || cnt == 0; i = i->flink) {
            if ((i->attributes & (ISALLOCATED)) == 0) {
                if (i->addr == whichaddr) {
                    u = 1;
                    break;
                }
            }
            ++cnt;
        }
        if (u == 1) {
            if ((i->addr < t->addr && whichpath) || (i->addr > t->addr && !whichpath)) {
                deleteentry(level, i);
                deleteentry(level, t);
                t = whichpath ? i : t;
                t->flink = t->blink = t;
                inserttail(level + 1, t);
            }
        }
        else {
            break;
        }
        ++level;
    }
}

void buddyallocator::free(PVOID addr, size_t x) {
    size_t level =  findlevel(x);
    if (level >= TOPLEVEL - 1)
        return;
    buddylist* h = (buddylist*)buddylists;
    linkedlist* hd = (linkedlist*)h->a[level];
    if (hd == NULL) {
        return;
    }
    int cnt = 0;
    linkedlist* i = NULL;
    for (i = (linkedlist*)hd->flink; i != hd || cnt == 0; i = (linkedlist*)i->flink) {
        if (i->addr == addr) {
            break;
        }
        ++cnt;
    }
    if (i->addr == addr)
    {
        i->attributes = (i->attributes & (~ISALLOCATED));
        clean_fragmentation(level, i);
    }

}

buddyallocator::~buddyallocator() {

}