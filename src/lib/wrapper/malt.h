/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrapper/malt.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2016 - 2024
***********************************************************/

#ifndef MALT_CONTROLER_H
#define MALT_CONTROLER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************/
void maltEnable(void);
void maltDisable(void);
int maltInitStatus(void);

#ifdef __cplusplus
}
#endif

#endif //MALT_CONTROLER_H
