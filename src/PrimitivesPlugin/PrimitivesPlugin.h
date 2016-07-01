// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

#ifndef PRIMITIVESPLUGIN_H
#define PRIMITIVESPLUGIN_H

#if defined PRIMITIVESPLUGIN_EXPORTS
#if defined WIN32
#define PRIMITIVESPLUGIN_EXPORT              __declspec( dllexport )
#else
#define PRIMITIVESPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define PRIMITIVESPLUGIN_EXPORT              __declspec( dllimport )
#else
#define PRIMITIVESPLUGIN_EXPORT
#endif
#endif

#endif
