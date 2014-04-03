
#if !defined ( PYCONSOLE_H )
#define PYCONSOLE_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WIN32
#  if defined PYCONSOLE_EXPORTS || defined PyConsole_EXPORTS
#    define PYCONSOLE_EXPORT __declspec(dllexport)
#  else
#    define PYCONSOLE_EXPORT __declspec(dllimport)
#  endif
#else   // WIN32
#  define PYCONSOLE_EXPORT
#endif  // WIN32

// ========================================================
// avoid warning messages
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // PYCONSOLE_H
