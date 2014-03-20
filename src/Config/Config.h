#ifndef CONFIG_H
#define CONFIG_H

#if defined CONFIG_EXPORTS
#if defined WIN32
#define CONFIG_EXPORT              __declspec( dllexport )
#else
#define CONFIG_EXPORT
#endif
#else
#if defined WIN32
#define CONFIG_EXPORT              __declspec( dllimport )
#else
#define CONFIG_EXPORT
#endif
#endif

#endif //CONFIG_H
