// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef FEATURESAPI_H
#define FEATURESAPI_H

#if defined FEATURESAPI_EXPORTS
#if defined WIN32
#define FEATURESAPI_EXPORT __declspec( dllexport )
#else
#define FEATURESAPI_EXPORT
#endif
#else
#if defined WIN32
#define FEATURESAPI_EXPORT __declspec( dllimport )
#else
#define FEATURESAPI_EXPORT
#endif
#endif

#endif
