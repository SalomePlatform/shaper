#ifndef GeomModule_Defs_H
#define GeomModule_Defs_H


#if defined WIN32
#  if defined GeomModule_EXPORTS
#    define GM_EXPORT __declspec( dllexport )
#  else
#    define GM_EXPORT __declspec( dllimport )
#  endif
#else
#  define GM_EXPORT
#endif


#endif