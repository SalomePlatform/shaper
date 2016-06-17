// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PARAMETERSAPI_H
#define PARAMETERSAPI_H

#if defined PARAMETERSAPI_EXPORTS
#if defined WIN32
#define PARAMETERSAPI_EXPORT __declspec( dllexport )
#else
#define PARAMETERSAPI_EXPORT
#endif
#else
#if defined WIN32
#define PARAMETERSAPI_EXPORT __declspec( dllimport )
#else
#define PARAMETERSAPI_EXPORT
#endif
#endif

#endif
