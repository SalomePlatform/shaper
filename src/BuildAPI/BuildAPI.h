// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef BUILDAPI_H
#define BUILDAPI_H

#if defined BUILDAPI_EXPORTS
#if defined WIN32
#define BUILDAPI_EXPORT __declspec( dllexport )
#else
#define BUILDAPI_EXPORT
#endif
#else
#if defined WIN32
#define BUILDAPI_EXPORT __declspec( dllimport )
#else
#define BUILDAPI_EXPORT
#endif
#endif

#endif
