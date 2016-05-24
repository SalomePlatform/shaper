// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef CONSTRUCTIONAPI_H
#define CONSTRUCTIONAPI_H

#if defined CONSTRUCTIONAPI_EXPORTS
#if defined WIN32
#define CONSTRUCTIONAPI_EXPORT __declspec( dllexport )
#else
#define CONSTRUCTIONAPI_EXPORT
#endif
#else
#if defined WIN32
#define CONSTRUCTIONAPI_EXPORT __declspec( dllimport )
#else
#define CONSTRUCTIONAPI_EXPORT
#endif
#endif

#endif
