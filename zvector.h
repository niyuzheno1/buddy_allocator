#pragma once
#include "bda.h"
template <typename T>
class zvector
{
public:
	zvector(buddyallocator * a);
	void push_back(T b);
	T pop_back();
	size_t length();
	void print();
	T& operator[](int i);
private:
	T* head;
	size_t actualsize;
	size_t len;
	buddyallocator* use;
};
#include <iostream>
using namespace std;

template <typename T>
zvector<T>::zvector(buddyallocator* a) {
    this->use = a;
    this->head = (T*)this->use->alloc(16);
    this->actualsize = 16;
    this->len = 0;
}

template <typename T>
void zvector<T>::push_back(T b) {
    if (len == this->actualsize) {
        T* nhead = (T*)this->use->alloc((this->actualsize << 1));
        for (int i = 0; i < len; ++i)
        {
            nhead[i] = head[i];
        }
        this->use->free(head, this->actualsize);
        this->actualsize = (this->actualsize << 1);
        this->head = nhead;
    }
    this->head[len] = b;
    ++len;
}

template <typename T>
T zvector<T>::pop_back() {
    if (len == 0) {
        return head[0];
    }
    T p = head[len - 1];
    --len;
    if (len + 50 < (actualsize >> 1)) {
        T* nhead = (T*)this->use->alloc((this->actualsize >> 1));
        for (int i = 0; i < len; ++i)
        {
            nhead[i] = head[i];
        }
        this->use->free(head, this->actualsize);
        this->actualsize = (this->actualsize >> 1);
        this->head = nhead;
    }
    return p;
}
template <typename T>
size_t zvector<T>::length() {
    return len;
}
template <typename T>
void zvector<T>::print() {
    for (int i = 0; i < len; ++i) {
        cout << head[i] << " ";
    }
    cout << endl;
}
template <typename T>
T& zvector<T>::operator[](int i) {
    if (i > len) {
        return head[0];
    }

    return head[i];
}