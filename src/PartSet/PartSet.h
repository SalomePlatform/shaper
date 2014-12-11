// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PARTSET_H
#define PARTSET_H

#if defined PARTSET_EXPORTS
#if defined WIN32
#define PARTSET_EXPORT              __declspec( dllexport )
#else
#define PARTSET_EXPORT
#endif
#else
#if defined WIN32
#define PARTSET_EXPORT              __declspec( dllimport )
#else
#define PARTSET_EXPORT
#endif
#endif

#endif //PARTSET_H