#include "res.h"

namespace res
{
    Resources ui;

    void load()
    {
        ui.loadXML("xmls/ui.xml");
    }

    void free()
    {
        ui.free();

    }
}