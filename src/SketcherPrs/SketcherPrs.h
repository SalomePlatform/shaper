// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SKETCHERPRS_H
#define SKETCHERPRS_H

#if defined SKETCHERPRS_EXPORTS
#if defined WIN32
#define SKETCHERPRS_EXPORT   __declspec( dllexport )
#else
#define SKETCHERPRS_EXPORT
#endif
#else
#if defined WIN32
#define SKETCHERPRS_EXPORT   __declspec( dllimport )
#else
#define SKETCHERPRS_EXPORT
#endif
#endif

#endif
