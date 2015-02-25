
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef INITIALIZATIONPLUGIN_H
#define INITIALIZATIONPLUGIN_H

#if defined INITIALIZATIONPLUGIN_EXPORTS
#if defined WIN32
#define INITIALIZATIONPLUGIN_EXPORT              __declspec( dllexport )
#else
#define INITIALIZATIONPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define INITIALIZATIONPLUGIN_EXPORT              __declspec( dllimport )
#else
#define INITIALIZATIONPLUGIN_EXPORT
#endif
#endif

#endif

