#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int primechu(double n) {
    int i;
    int v = (int)n;
    if (v < 2) return 0;
    for (i = 2; i * i <= v; ++i) {
        if (v % i == 0) return 0;
    }
    return 1;
}



 int main( ) {
    
    printf ( "hello world" ) ;

    
    int i = 1 ;

    
    for ( i = 1 ; i <= 10 ; i = i + 1 ) {
        printf ( i ) ;
    }

    return 0 ;
}

