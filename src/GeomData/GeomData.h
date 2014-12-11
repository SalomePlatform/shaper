// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef GEOMDATA_H
#define GEOMDATA_H

#if defined GEOMDATA_EXPORTS
#if defined WIN32
#define GEOMDATA_EXPORT              __declspec( dllexport )
#else
#define GEOMDATA_EXPORT
#endif
#else
#if defined WIN32
#define GEOMDATA_EXPORT              __declspec( dllimport )
#else
#define GEOMDATA_EXPORT
#endif
#endif

#endif
