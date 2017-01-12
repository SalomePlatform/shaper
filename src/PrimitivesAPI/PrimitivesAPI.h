// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

#ifndef PRIMITIVESAPI_H
#define PRIMITIVESAPI_H

#if defined PRIMITIVESAPI_EXPORTS
#if defined WIN32
#define PRIMITIVESAPI_EXPORT __declspec( dllexport )
#else
#define PRIMITIVESAPI_EXPORT
#endif
#else
#if defined WIN32
#define PRIMITIVESAPI_EXPORT __declspec( dllimport )
#else
#define PRIMITIVESAPI_EXPORT
#endif
#endif

#endif //PRIMITIVESAPI_H
