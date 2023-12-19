#include <iostream>
using namespace std;

template<template<class ...> class ...Args>
class bar : public Args<int, double>... {
public:
  bar(const Args<int, double>& ...args) : Args<int, double>(args) ... {}
};

template<class ...Args>
class baz1 {};

template<class ...Args>
class baz2 {};

int main() {
  baz1<int, double> a1;
  baz2<int, double> a2;
  bar<baz1, baz2> b(a1, a2);
}