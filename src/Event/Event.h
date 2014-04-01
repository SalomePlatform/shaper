#ifndef EVENT_H
#define EVENT_H

#if defined EVENT_EXPORTS
#if defined WIN32
#define EVENT_EXPORT              __declspec( dllexport )
#else
#define EVENT_EXPORT
#endif
#else
#if defined WIN32
#define EVENT_EXPORT              __declspec( dllimport )
#else
#define EVENT_EXPORT
#endif
#endif

#endif
