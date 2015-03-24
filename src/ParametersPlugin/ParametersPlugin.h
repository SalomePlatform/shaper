// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PARAMETERSPLUGIN_H
#define PARAMETERSPLUGIN_H

#if defined PARAMETERSPLUGIN_EXPORTS
#if defined WIN32
#define PARAMETERSPLUGIN_EXPORT              __declspec( dllexport )
#else
#define PARAMETERSPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define PARAMETERSPLUGIN_EXPORT              __declspec( dllimport )
#else
#define PARAMETERSPLUGIN_EXPORT
#endif
#endif

#endif

