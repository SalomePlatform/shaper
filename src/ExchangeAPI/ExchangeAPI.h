// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef EXCHANGEAPI_H
#define EXCHANGEAPI_H

#if defined EXCHANGEAPI_EXPORTS
#if defined WIN32
#define EXCHANGEAPI_EXPORT __declspec( dllexport )
#else
#define EXCHANGEAPI_EXPORT
#endif
#else
#if defined WIN32
#define EXCHANGEAPI_EXPORT __declspec( dllimport )
#else
#define EXCHANGEAPI_EXPORT
#endif
#endif

#endif
