#include <stdio.h>

extern int Mymax(int,int);
extern int add(int,int);
extern void myPrint();

int main(){
    int a = 10,b = 15;
    printf("max: %d\nsum: %d\n",Mymax(a,b),add(a,b));
    myPrint();
    return 0;
}