// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef SKETCHSHAPEPLUGIN_H
#define SKETCHSHAPEPLUGIN_H

#if defined SKETCHSHAPEPLUGIN_EXPORTS
#if defined WIN32
#define SKETCHSHAPEPLUGIN_EXPORT              __declspec( dllexport )
#else
#define SKETCHSHAPEPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define SKETCHSHAPEPLUGIN_EXPORT              __declspec( dllimport )
#else
#define SKETCHSHAPEPLUGIN_EXPORT
#endif
#endif

#endif
