// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MODULEBASE_H
#define MODULEBASE_H

#if defined MODULEBASE_EXPORTS
#if defined WIN32
#define MODULEBASE_EXPORT              __declspec( dllexport )
#else
#define MODULEBASE_EXPORT
#endif
#else
#if defined WIN32
#define MODULEBASE_EXPORT              __declspec( dllimport )
#else
#define MODULEBASE_EXPORT
#endif
#endif

#endif //MODULEBASE_H