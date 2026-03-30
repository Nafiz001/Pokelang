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
 int add ( int a , int b ) {
    return a + b ;
}

 int main( ) {
    int i = 0 ;
    int total = 0 ;

    for ( i = 1 ; i <= 5 ; i = i + 1 ) {
        total = add ( total , i ) ;
    }

    if ( total == 15 ) {
        printf ( "Sum from 1 to 5 is correct" ) ;
    } else {
        printf ( "Sum calculation failed" ) ;
    }

    return 0 ;
}

