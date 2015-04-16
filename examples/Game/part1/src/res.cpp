#include "res.h"

namespace res
{
    Resources ui;

    void load()
    {
        //load our resources
        ui.loadXML("xmls/ui.xml");
    }

    void free()
    {
        //unload
        ui.free();
    }
}