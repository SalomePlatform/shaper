// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PARTSETAPI_H
#define PARTSETAPI_H

#if defined PARTSETAPI_EXPORTS
#if defined WIN32
#define PARTSETAPI_EXPORT __declspec( dllexport )
#else
#define PARTSETAPI_EXPORT
#endif
#else
#if defined WIN32
#define PARTSETAPI_EXPORT __declspec( dllimport )
#else
#define PARTSETAPI_EXPORT
#endif
#endif

#endif
