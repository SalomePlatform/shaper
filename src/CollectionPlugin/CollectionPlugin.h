// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef COLLECTIONPLUGIN_H
#define COLLECTIONPLUGIN_H

#if defined COLLECTIONPLUGIN_EXPORTS
#if defined WIN32
#define COLLECTIONPLUGIN_EXPORT              __declspec( dllexport )
#else
#define COLLECTIONPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define COLLECTIONPLUGIN_EXPORT              __declspec( dllimport )
#else
#define COLLECTIONPLUGIN_EXPORT
#endif
#endif

#endif
