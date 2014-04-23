#ifndef GEOMALGOAPI_H
#define GEOMALGOAPI_H

#if defined GEOMALGOAPI_EXPORTS
#if defined WIN32
#define GEOMALGOAPI_EXPORT              __declspec( dllexport )
#else
#define GEOMALGOAPI_EXPORT
#endif
#else
#if defined WIN32
#define GEOMALGOAPI_EXPORT              __declspec( dllimport )
#else
#define GEOMALGOAPI_EXPORT
#endif
#endif

#endif
