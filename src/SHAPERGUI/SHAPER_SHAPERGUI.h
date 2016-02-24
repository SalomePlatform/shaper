// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SHAPER_SHAPERGUI_H
#define SHAPER_SHAPERGUI_H

#if defined SHAPERGUI_EXPORTS
#if defined WIN32
#define SHAPERGUI_EXPORT     __declspec( dllexport )
#else
#define SHAPERGUI_EXPORT
#endif
#else
#if defined WIN32
#define SHAPERGUI_EXPORT     __declspec( dllimport )
#else
#define SHAPERGUI_EXPORT
#endif
#endif

#endif

