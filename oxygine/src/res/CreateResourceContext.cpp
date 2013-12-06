#include "CreateResourceContext.h"
#include "core/NativeTexture.h"
#include "MemoryTexture.h"

namespace oxygine
{	
	RestoreResourcesContext RestoreResourcesContext::instance;
	void RestoreResourcesContext::createTexture(spMemoryTexture src, spNativeTexture dest)
	{
		dest->init(src->lock(), false);
	}

	bool RestoreResourcesContext::isNeedProceed(spNativeTexture t)
	{
		return true;
	}

	SingleThreadResourcesContext SingleThreadResourcesContext::instance;

	void SingleThreadResourcesContext::createTexture(spMemoryTexture src, spNativeTexture dest)
	{
		dest->init(src->lock(), false);
	}

	bool SingleThreadResourcesContext::isNeedProceed(spNativeTexture t)
	{
		return t->getHandle() == 0;
	}
}