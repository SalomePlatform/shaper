// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef GEOMDATAAPI_H
#define GEOMDATAAPI_H

#if defined GEOMDATAAPI_EXPORTS
#if defined WIN32
#define GEOMDATAAPI_EXPORT              __declspec( dllexport )
#else
#define GEOMDATAAPI_EXPORT
#endif
#else
#if defined WIN32
#define GEOMDATAAPI_EXPORT              __declspec( dllimport )
#else
#define GEOMDATAAPI_EXPORT
#endif
#endif

#endif
