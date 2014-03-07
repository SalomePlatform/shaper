
#ifndef XGUI_Tools_H
#define XGUI_Tools_H

#include <QString>

/*!
  \brief Convert the given parameter to the platform-specific library name.

  The function appends platform-specific prefix (lib) and suffix (.dll/.so)
  to the library file name.
  For example, if \a str = "mylib", "libmylib.so" is returned for Linux and
  mylib.dll for Windows.

  \param str short library name
  \return full library name
*/
QString library( const QString& str );

/*!
  \brief Return directory part of the file path.

  If the file path does not include directory part (the file is in the
  current directory), null string is returned.

  \param path file path
  \param abs if true (default) \a path parameter is treated as absolute file path
  \return directory part of the file path
*/
QString dir( const QString& path, bool isAbs = true);


/*!
  \brief Return file name part of the file path.

  \param path file path
  \param withExt if true (default) complete file name (with all
         extension except the last) is returned, otherwise only base name
         is returned
  \return file name part of the file path
*/
QString file( const QString& path, bool withExt = true );

/*!
  \brief Return extension part of the file path.

  \param path file path
  \param full if true complete extension (all extensions, dot separated)
         is returned, otherwise (default) only last extension is returned
  \return extension part of the file path
*/
QString extension( const QString& path, bool full = false );


/*!
  \brief Add a slash (platform-specific) to the end of \a path
         if it is not already there.
  \param path directory path
  \return modified path (with slash added to the end)
*/
QString addSlash( const QString& path );

#endif