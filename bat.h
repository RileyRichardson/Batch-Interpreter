#define MAX_FLAGS 8

#define AFL 0
#define ECHO 1
#define LBL 2
#define FBF 3 
#define AOE 4
#define COE 5
#define CBF 6
#define FKS 7

#define RETURN -2
#define ERROR -1
#define FALSE 0 
#define TRUE 1
#define SUCCESS 2

#define PTR(a) (&a[0])

#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "src/state.cpp"
#include "src/util.cpp"
#include "src/push.cpp"
#include "src/proccess.cpp"