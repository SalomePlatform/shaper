// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef COLLECTIONAPI_H
#define COLLECTIONAPI_H

#if defined COLLECTIONAPI_EXPORTS
#if defined WIN32
#define COLLECTIONAPI_EXPORT __declspec( dllexport )
#else
#define COLLECTIONAPI_EXPORT
#endif
#else
#if defined WIN32
#define COLLECTIONAPI_EXPORT __declspec( dllimport )
#else
#define COLLECTIONAPI_EXPORT
#endif
#endif

#endif
