#ifndef NewGeom_H
#define NewGeom_H

#if defined NewGeom_EXPORTS
#if defined WIN32
#define NewGeom_EXPORT     __declspec( dllexport )
#else
#define NewGeom_EXPORT
#endif
#else
#if defined WIN32
#define NewGeom_EXPORT     __declspec( dllimport )
#else
#define NewGeom_EXPORT
#endif
#endif

#endif

