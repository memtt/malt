/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/state/malt.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2016
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
