#pragma once
#include "oxygine_include.h"
#include "jnihelper.h"
#include "core/oxygine.h"
#include <string>

namespace oxygine
{
	jobject			jniGetMainActivity();
	jclass			jniGetMainActivityClass();
	void			jniSetMainActivityClass(jclass);

	/**returns UTC time in MS*/
	int64			jniGetTimeUTCMS();

	/**returns locale. ISO 639-1 */
	std::string		jniGetLanguage();
	bool			jniIsNetworkAvailable();

	bool			jniExit();
	void			jniMoveTaskToBack();
	void			jniBrowse(const char *url);
}