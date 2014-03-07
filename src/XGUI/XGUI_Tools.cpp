
#include "XGUI_Tools.h"

#include <QDir>


//******************************************************************
QString library( const QString& str )
{
  QString path = dir( str, false );
  QString name = file( str, false );
  QString ext  = extension( str );

#ifndef WIN32
  if ( !name.startsWith( "lib" ) )
    name = QString( "lib" ) + name;
#endif

#ifdef WIN32
  QString libExt( "dll" );
#else
  QString libExt( "so" );
#endif

  if ( ext.toLower() != QString( "so" ) && ext.toLower() != QString( "dll" ) )
  {
    if ( !name.isEmpty() && !ext.isEmpty() )
      name += QString( "." );
    name += ext;
  }

  ext = libExt;

  QString fileName = addSlash( path ) + name + QString( "." ) + ext;

  return fileName;
}

//******************************************************************
QString dir( const QString& path, bool isAbs )
{
  QDir aDir = QFileInfo( path ).dir();
  QString dirPath = isAbs ? aDir.absolutePath() : aDir.path();
  if ( dirPath == QString( "." ) )
    dirPath = QString();
  return dirPath;
}


//******************************************************************
QString file( const QString& path, bool withExt )
{
  QString fPath = path;
  while ( !fPath.isEmpty() && ( fPath[fPath.length() - 1] =='\\' || fPath[fPath.length() - 1] == '/' ) )
    fPath.remove( fPath.length() - 1, 1 );

  if ( withExt )
    return QFileInfo( fPath ).fileName();
  else
    return QFileInfo( fPath ).completeBaseName();
}

//******************************************************************
QString extension( const QString& path, bool full )
{
  return full ? QFileInfo( path ).completeSuffix() : QFileInfo( path ).suffix();
}

//******************************************************************
QString addSlash( const QString& path )
{
  QString res = path;
  if ( !res.isEmpty() && res.at( res.length() - 1 ) != QChar( '/' ) &&
       res.at( res.length() - 1 ) != QChar( '\\' ) )
  res += QDir::separator();
  return res;
}
