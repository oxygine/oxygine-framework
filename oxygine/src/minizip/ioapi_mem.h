#ifndef _ZLIBIOAPI_MEM_H
#define _ZLIBIOAPI_MEM_H
#include "unzip.h"
extern "C" 
{
	void fill_memory_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def));
}
#endif