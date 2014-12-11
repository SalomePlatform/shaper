// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef EVENTS_H
#define EVENTS_H

#if defined EVENTS_EXPORTS
#if defined WIN32
#define EVENTS_EXPORT              __declspec( dllexport )
#else
#define EVENTS_EXPORT
#endif
#else
#if defined WIN32
#define EVENTS_EXPORT              __declspec( dllimport )
#else
#define EVENTS_EXPORT
#endif
#endif

#endif
