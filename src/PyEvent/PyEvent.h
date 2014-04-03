
#if !defined ( PYEVENT_H )
#define PYEVENT_H

#ifdef WIN32
#  if defined PYEVENT_EXPORTS || defined PyEvent_EXPORTS
#    define PYEVENT_EXPORT __declspec(dllexport)
#  else
#    define PYEVENT_EXPORT __declspec(dllimport)
#  endif
#else               //WIN32
#  define PYEVENT_EXPORT
#endif              //WIN32

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

#endif // PYEVENT_H
