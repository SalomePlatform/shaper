// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Validators.h
// Created: Aug 01, 2014
// Author:  Sergey BELASH

#ifndef EXCHANGEPLUGIN_H
#define EXCHANGEPLUGIN_H

#if defined EXCHANGEPLUGIN_EXPORTS
# if defined WIN32
#  define EXCHANGEPLUGIN_EXPORT              __declspec( dllexport )
# else
#  define EXCHANGEPLUGIN_EXPORT
# endif
#else
# if defined WIN32
#  define EXCHANGEPLUGIN_EXPORT              __declspec( dllimport )
# else
#  define EXCHANGEPLUGIN_EXPORT
# endif
#endif

#endif
