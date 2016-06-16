// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef CONNECTORAPI_H
#define CONNECTORAPI_H

#if defined CONNECTORAPI_EXPORTS
#if defined WIN32
#define CONNECTORAPI_EXPORT __declspec( dllexport )
#else
#define CONNECTORAPI_EXPORT
#endif
#else
#if defined WIN32
#define CONNECTORAPI_EXPORT __declspec( dllimport )
#else
#define CONNECTORAPI_EXPORT
#endif
#endif

#endif
