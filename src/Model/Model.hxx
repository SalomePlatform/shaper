#ifndef MODEL_H
#define MODEL_H

#if defined MODEL_EXPORTS
#if defined WIN32
#define MODEL_EXPORT              __declspec( dllexport )
#else
#define MODEL_EXPORT
#endif
#else
#if defined WIN32
#define MODEL_EXPORT              __declspec( dllimport )
#else
#define MODEL_EXPORT
#endif
#endif

#endif
