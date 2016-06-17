// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MODELHIGHAPI_H
#define MODELHIGHAPI_H

#if defined MODELHIGHAPI_EXPORTS
#if defined WIN32
#define MODELHIGHAPI_EXPORT __declspec( dllexport )
#else
#define MODELHIGHAPI_EXPORT
#endif
#else
#if defined WIN32
#define MODELHIGHAPI_EXPORT __declspec( dllimport )
#else
#define MODELHIGHAPI_EXPORT
#endif
#endif

#endif
