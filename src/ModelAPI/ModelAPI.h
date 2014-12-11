// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MODELAPI_H
#define MODELAPI_H

#if defined MODELAPI_EXPORTS
#if defined WIN32
#define MODELAPI_EXPORT              __declspec( dllexport )
#else
#define MODELAPI_EXPORT
#endif
#else
#if defined WIN32
#define MODELAPI_EXPORT              __declspec( dllimport )
#else
#define MODELAPI_EXPORT
#endif
#endif

#endif
