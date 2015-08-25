// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef GEOMALGOIMPL_H
#define GEOMALGOIMPL_H

#if defined GEOMALGOIMPL_EXPORTS
#if defined WIN32
#define GEOMALGOIMPL_EXPORT              __declspec( dllexport )
#else
#define GEOMALGOIMPL_EXPORT
#endif
#else
#if defined WIN32
#define GEOMALGOIMPL_EXPORT              __declspec( dllimport )
#else
#define GEOMALGOIMPL_EXPORT
#endif
#endif

#endif
