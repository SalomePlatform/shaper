#ifndef GEOMAPI_H
#define GEOMAPI_H

#if defined GEOMAPI_EXPORTS
#if defined WIN32
#define GEOMAPI_EXPORT              __declspec( dllexport )
#else
#define GEOMAPI_EXPORT
#endif
#else
#if defined WIN32
#define GEOMAPI_EXPORT              __declspec( dllimport )
#else
#define GEOMAPI_EXPORT
#endif
#endif

#endif
