#include "Serialize.h"
#include "Sprite.h"
#include "TextActor.h"
#include "ColorRectSprite.h"
#include "ClipRectActor.h"
#include "ProgressBar.h"
namespace oxygine
{
    //#define S(s) #s
    //#define SS(s) ##s
    #define CREATE(T) if (!strcmp(#T, type)) return new T

	spActor creator::create(const char *type) const
	{
		CREATE(Sprite);
		CREATE(TextActor);		
		CREATE(Actor);
		CREATE(ProgressBar);
		CREATE(ColorRectSprite);
		CREATE(ClipRectActor);

		return 0;
	}

	spActor deserializedata::deser(pugi::xml_node node, const creator* factory)
	{
		deserializedata d;
		d.node = node;
		d.factory = factory;
		const char *name = node.name();
		spActor actor = factory->create(name);
		actor->deserialize(&d);
		return actor;
	}
}
