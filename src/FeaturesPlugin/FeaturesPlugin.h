// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef FEATURESPLUGIN_H
#define FEATURESPLUGIN_H

#if defined FEATURESPLUGIN_EXPORTS
#if defined WIN32
#define FEATURESPLUGIN_EXPORT              __declspec( dllexport )
#else
#define FEATURESPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define FEATURESPLUGIN_EXPORT              __declspec( dllimport )
#else
#define FEATURESPLUGIN_EXPORT
#endif
#endif

#endif
