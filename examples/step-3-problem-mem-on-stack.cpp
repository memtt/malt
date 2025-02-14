/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/config.h.in
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
void depth_3(void)
{
    char var_in_stack[1024*1024*3];
    memset(var_in_stack, 0, sizeof(var_in_stack));
}

/**********************************************************/
void depth_2(int depth_next)
{
    char var_in_stack[4096];
    memset(var_in_stack, 0, sizeof(var_in_stack));

    //call next
    if (depth_next == 0)
        depth_3();
    else
        depth_2(depth_next-1);
}

/**********************************************************/
void depth_1(void)
{
    char var_in_stack[4096];
    memset(var_in_stack, 0, sizeof(var_in_stack));
    depth_2(15);
}

/**********************************************************/
void doing_things_with_memory(void)
{
    depth_1();
}

/**********************************************************/
int main(void)
{
    doing_things_with_memory();
    return EXIT_SUCCESS;
}
