#pragma once

#include "any.h"

Any operator+ (Any lhs, int rhs);
Any operator+ (Any lhs, int64_t rhs);
Any operator+ (Any lhs, double rhs);
Any operator+ (Any lhs, Any rhs);
Any operator+ (int lhs, Any rhs);
Any operator+ (int64_t lhs, Any rhs);
Any operator+ (double lhs, Any rhs);

Any operator* (Any lhs, int rhs);
Any operator* (Any lhs, int64_t rhs);
Any operator* (Any lhs, double rhs);
Any operator* (Any lhs, Any rhs);
Any operator* (int lhs, Any rhs);
Any operator* (int64_t lhs, Any rhs);
Any operator* (double lhs, Any rhs);

Any operator- (Any lhs, int64_t rhs);
Any operator- (Any lhs, int rhs);
Any operator- (Any lhs, double rhs);
Any operator- (Any lhs, Any rhs);
Any operator- (int64_t lhs, Any rhs);
Any operator- (int lhs, Any rhs);
Any operator- (double lhs, Any rhs);

double operator/ (double lhs, Any rhs);
double operator/ (int64_t lhs, Any rhs);
double operator/ (Any lhs, Any rhs);
double operator/ (Any lhs, double rhs);
double operator/ (Any lhs, int64_t rhs);

bool operator< (int64_t lhs, Any rhs);
bool operator< (double lhs, Any rhs);
bool operator< (Any lhs, Any rhs);
bool operator< (Any lhs, int64_t rhs);
bool operator< (Any lhs, double rhs);

bool operator> (Any lhs, Any rhs);

bool operator>= (Any lhs, int rhs);
bool operator>= (Any lhs, Any rhs);

bool operator <= (Any lhs, Any rhs);

int64_t operator&= (int64_t lhs, Any rhs);
int64_t operator&= (Any lhs, int64_t rhs);
int64_t operator&= (Any lhs, Any rhs);

int64_t operator& (int64_t lhs, Any rhs);
int64_t operator& (Any lhs, int64_t rhs);
int64_t operator& (Any lhs, Any rhs);

int64_t operator<< (int64_t lhs, Any rhs);
int64_t operator<< (int lhs, Any rhs);
int64_t operator<< (Any lhs, int64_t rhs);
int64_t operator<< (Any lhs, int rhs);
int64_t operator<< (Any lhs, Any rhs);

int64_t operator| (Any lhs, Any rhs);
int64_t operator| (int64_t lhs, Any rhs);
int64_t operator| (Any lhs, int64_t rhs);

int64_t operator^ (Any lhs, Any rhs);
int64_t operator^ (int64_t lhs, Any rhs);
int64_t operator^ (Any lhs, int64_t rhs);

int64_t operator>> (int64_t lhs, Any rhs);
int64_t operator>> (Any lhs, int rhs);


std::ostream& operator<<(std::ostream& os, Any x);

// only defining operator overloads for binary_trees
/// @note only doing int comparison because I worry about floating point equality
bool operator==(Any lhs, int64_t rhs);
bool operator==(Any lhs, int rhs);
bool operator==(Any lhs, Any rhs);
bool operator!=(Any lhs, Any rhs);