// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PARTSETPLUGIN_H
#define PARTSETPLUGIN_H

#if defined PARTSETPLUGIN_EXPORTS
#if defined WIN32
#define PARTSETPLUGIN_EXPORT              __declspec( dllexport )
#else
#define PARTSETPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define PARTSETPLUGIN_EXPORT              __declspec( dllimport )
#else
#define PARTSETPLUGIN_EXPORT
#endif
#endif

#endif
