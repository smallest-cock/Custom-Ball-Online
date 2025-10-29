#pragma once
#define VERSION_MAJOR 2
#define VERSION_MINOR 2
#define VERSION_PATCH 9
#define VERSION_BUILD 0

#define VERSION_SUFFIX "" // can be "" if no suffix

#define stringify(a) stringify_(a)
#define stringify_(a) #a

#define VERSION_STR stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) VERSION_SUFFIX
