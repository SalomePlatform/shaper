// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef SKETCHPLUGIN_H
#define SKETCHPLUGIN_H

#if defined SKETCHPLUGIN_EXPORTS
#if defined WIN32
#define SKETCHPLUGIN_EXPORT              __declspec( dllexport )
#else
#define SKETCHPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define SKETCHPLUGIN_EXPORT              __declspec( dllimport )
#else
#define SKETCHPLUGIN_EXPORT
#endif
#endif

#endif
