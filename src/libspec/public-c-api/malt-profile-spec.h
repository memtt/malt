/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFormat.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/
 
#ifndef MALT_PROFILE_SPEC_H
#define MALT_PROFILE_SPEC_H

/**********************************************************/
//standard
#include <stdio.h>
#include <stdlib.h>

/**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************/
enum malt_profile_lang_e
{
	MALT_PROFILE_LANG_C,
	MALT_PROFILE_LANG_PYTHON,
	MALT_PROFILE_LANG_TRANS_PTR,
};
typedef enum malt_profile_lang_e malt_profile_lang_t;

/**********************************************************/
struct malt_profile_lang_address_s
{
	malt_profile_lang_t lang;
	void * address;
};
typedef struct malt_profile_lang_address_s malt_profile_lang_address_t;

/**********************************************************/
#ifdef __cplusplus
}
#endif

#endif //MALT_PROFILE_SPEC_H
