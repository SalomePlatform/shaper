#ifndef SalomeModule_H
#define SalomeModule_H

#if defined SalomeModule_EXPORTS
#if defined WIN32
#define SalomeModule_EXPORT     __declspec( dllexport )
#else
#define SalomeModule_EXPORT
#endif
#else
#if defined WIN32
#define SalomeModule_EXPORT     __declspec( dllimport )
#else
#define SalomeModule_EXPORT
#endif
#endif

#endif



