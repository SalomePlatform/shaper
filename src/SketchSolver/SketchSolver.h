// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SKETCHSOLVER_H
#define SKETCHSOLVER_H

#if defined SKETCHSOLVER_EXPORTS
#if defined WIN32
#define SKETCHSOLVER_EXPORT              __declspec( dllexport )
#else
#define SKETCHSOLVER_EXPORT
#endif
#else
#if defined WIN32
#define SKETCHSOLVER_EXPORT              __declspec( dllimport )
#else
#define SKETCHSOLVER_EXPORT
#endif
#endif

#endif
