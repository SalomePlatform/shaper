// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef GEOMVALIDATORS_H
#define GEOMVALIDATORS_H

#if defined GEOMVALIDATORS_EXPORTS
#if defined WIN32
#define GEOMVALIDATORS_EXPORT              __declspec( dllexport )
#else
#define GEOMVALIDATORS_EXPORT
#endif
#else
#if defined WIN32
#define GEOMVALIDATORS_EXPORT              __declspec( dllimport )
#else
#define GEOMVALIDATORS_EXPORT
#endif
#endif

#endif
