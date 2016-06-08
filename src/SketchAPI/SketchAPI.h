// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SKETCHAPI_H
#define SKETCHAPI_H

#if defined SKETCHAPI_EXPORTS
#if defined WIN32
#define SKETCHAPI_EXPORT __declspec( dllexport )
#else
#define SKETCHAPI_EXPORT
#endif
#else
#if defined WIN32
#define SKETCHAPI_EXPORT __declspec( dllimport )
#else
#define SKETCHAPI_EXPORT
#endif
#endif

#endif
