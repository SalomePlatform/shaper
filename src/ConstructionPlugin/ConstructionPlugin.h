#ifndef CONSTRUCTIONPLUGIN_H
#define CONSTRUCTIONPLUGIN_H

#if defined CONSTRUCTIONPLUGIN_EXPORTS
#if defined WIN32
#define CONSTRUCTIONPLUGIN_EXPORT              __declspec( dllexport )
#else
#define CONSTRUCTIONPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define CONSTRUCTIONPLUGIN_EXPORT              __declspec( dllimport )
#else
#define CONSTRUCTIONPLUGIN_EXPORT
#endif
#endif

#endif
