// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef SAMPLEPANELPLUGIN_H
#define SAMPLEPANELPLUGIN_H

#if defined SAMPLEPANELPLUGIN_EXPORTS
#if defined WIN32
#define SAMPLEPANELPLUGIN_EXPORT              __declspec( dllexport )
#else
#define SAMPLEPANELPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define SAMPLEPANELPLUGIN_EXPORT              __declspec( dllimport )
#else
#define SAMPLEPANELPLUGIN_EXPORT
#endif
#endif

#endif
