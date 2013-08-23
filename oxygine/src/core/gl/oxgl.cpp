#include "oxgl.h"
#include "../ox_debug.h"

void initGLExtensions()
{
#ifdef WIN32
	GLenum r = glewInit();
	OX_ASSERT(r == GLEW_OK);
#endif
}