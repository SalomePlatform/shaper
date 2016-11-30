// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GDMLAPI.h
// Created:     29 Nov 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GDMLAPI_H
#define GDMLAPI_H

#if defined GDMLAPI_EXPORTS
#if defined WIN32
#define GDMLAPI_EXPORT __declspec( dllexport )
#else
#define GDMLAPI_EXPORT
#endif
#else
#if defined WIN32
#define GDMLAPI_EXPORT __declspec( dllimport )
#else
#define GDMLAPI_EXPORT
#endif
#endif

#endif // GDMLAPI_H
