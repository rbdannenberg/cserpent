//
// Created by anthony on 6/24/24.
//
#pragma once

template <typename T>
inline void do_not_optimize(const T& value) {
    asm volatile("" : : "r,m"(value) : "memory");
}