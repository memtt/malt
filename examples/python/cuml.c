#include <stdio.h>
#include <stdlib.h>
// cmult.c
float cmult(int int_param, float float_param) {
    float return_value = int_param * float_param;
    printf("    In cmult : int: %d float %.1f returning  %.1f\n", int_param,
            float_param, return_value);
    size_t i;
    for (i = 0 ; i < 100000; i++)
	    free(malloc(10));
    return return_value;
}
