
#if !defined ( PYINTERP_H )
#define PYINTERP_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WIN32
# if defined PYINTERP_EXPORTS || defined PyInterp_EXPORTS
#  define PYINTERP_EXPORT __declspec(dllexport)
# else
#  define PYINTERP_EXPORT __declspec(dllimport)
# endif
#else   // WIN32
# define PYINTERP_EXPORT
#endif  // WIN32

// ========================================================
// little trick - if we do not have debug python libraries
#ifdef _DEBUG
 #ifndef HAVE_DEBUG_PYTHON
  #undef _DEBUG
 #endif
#endif

#include <Python.h>

#ifdef _DEBUG
 #ifndef HAVE_DEBUG_PYTHON
  #define _DEBUG
 #endif
#endif

// ========================================================
// avoid warning messages
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // PYINTERP_H
