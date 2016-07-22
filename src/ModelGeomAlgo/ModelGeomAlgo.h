// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MODELGEOMALGO_H
#define MODELGEOMALGO_H

#if defined MODELGEOMALGO_EXPORTS
#if defined WIN32
#define MODELGEOMALGO_EXPORT              __declspec( dllexport )
#else
#define MODELGEOMALGO_EXPORT
#endif
#else
#if defined WIN32
#define MODELGEOMALGO_EXPORT              __declspec( dllimport )
#else
#define MODELGEOMALGO_EXPORT
#endif
#endif

#endif
