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
    int healthPoints = 72 ;

    if ( healthPoints > 80 ) {
        printf ( "Excellent health" ) ;
    } else if ( healthPoints >= 50 ) {
        printf ( "Moderate health" ) ;
    } else {
        printf ( "Critical health" ) ;
    }

    return 0 ;
}

