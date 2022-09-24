#include "stdlib.h"

#define T int

int compare(T left, T right){
    return left - right;
}

void swap(T* a, T* b){
    T t = *a;
    *a = *b;
    *b = t;
}

T* q_sort_part(T* begin, T* end){
    T* pivot = end - 1;
    T* left_ptr = begin - 1;

    for (T* right_ptr = begin; right_ptr < end; ++right_ptr){
        if (compare(*right_ptr, *pivot) > 0) continue;
        ++left_ptr;
        swap(left_ptr, right_ptr);
    }

    return left_ptr + 1;
}

void q_sort(T* begin, T* end){
    if (begin >= end) return;
    T* part_ptr = q_sort_part(begin, end);
    q_sort(begin, part_ptr-1);
    q_sort(part_ptr+1,end);
}