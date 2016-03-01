#include "emsc.h"
namespace oxygine
{
	void emscSyncFS(bool read)
	{
#ifdef EMSCRIPTEN
		EM_ASM_ARGS({
			FS.mkdir('/data');
		FS.mount(IDBFS,{}, '/data');

		FS.syncfs(true, function(err) {
			// handle callback
		});
		}, 0);
#else

#endif
	}
}