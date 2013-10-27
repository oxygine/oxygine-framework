#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

extern Point virtualSize;//display virtual resolution

extern Resources gameResources;
extern TextStyle basicStyle; 

void initResources();
void freeResources();

std::string getString(const char *group, const char *id);