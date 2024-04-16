%module helloWorld

%{
#include <stdio.h>
extern void printHelloWorld();
%}

extern void printHelloWorld();

