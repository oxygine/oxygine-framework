#ifndef _ZLIBIOAPI_MEM_H
#define _ZLIBIOAPI_MEM_H
#include "unzip.h"

extern "C" 
{
	typedef struct 
	{
		char* data;
		uLong size;
	} zmemdata;

	void fill_memory_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def));
}
#endif