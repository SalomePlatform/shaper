// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GDMLPlugin.h
// Created:     23 Nov 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GDMLPLUGIN_H
#define GDMLPLUGIN_H

#if defined GDMLPLUGIN_EXPORTS
#if defined WIN32
#define GDMLPLUGIN_EXPORT              __declspec( dllexport )
#else
#define GDMLPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define GDMLPLUGIN_EXPORT              __declspec( dllimport )
#else
#define GDMLPLUGIN_EXPORT
#endif
#endif

#endif // GDMLPLUGIN_H
