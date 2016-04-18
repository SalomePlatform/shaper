// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin.h
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_H_
#define BuildPlugin_H_

#if defined BUILDPLUGIN_EXPORTS
# if defined WIN32
#  define BUILDPLUGIN_EXPORT __declspec( dllexport )
# else
#  define BUILDPLUGIN_EXPORT
# endif
#else
# if defined WIN32
#  define BUILDPLUGIN_EXPORT __declspec( dllimport )
# else
#  define BUILDPLUGIN_EXPORT
# endif
#endif

#endif
