#include "XGUI_Tools.h"

#include <TopoDS_Shape.hxx>
#include <ModelAPI_Feature.h>

#include <QDir>

#include <iostream>
#include <sstream>

//******************************************************************
QString dir(const QString& path, bool isAbs)
{
  QDir aDir = QFileInfo(path).dir();
  QString dirPath = isAbs ? aDir.absolutePath() : aDir.path();
  if (dirPath == QString("."))
    dirPath = QString();
  return dirPath;
}

//******************************************************************
QString file(const QString& path, bool withExt)
{
  QString fPath = path;
  while(!fPath.isEmpty() && (fPath[fPath.length() - 1] == '\\' || fPath[fPath.length() - 1] == '/'))
    fPath.remove(fPath.length() - 1, 1);

  if (withExt)
    return QFileInfo(fPath).fileName();
  else
    return QFileInfo(fPath).completeBaseName();
}

//******************************************************************
QString extension(const QString& path, bool full)
{
  return full ? QFileInfo(path).completeSuffix() : QFileInfo(path).suffix();
}

//******************************************************************
QString addSlash(const QString& path)
{
  QString res = path;
  if (!res.isEmpty() && res.at(res.length() - 1) != QChar('/')
      && res.at(res.length() - 1) != QChar('\\'))
    res += QDir::separator();
  return res;
}

//******************************************************************
QRect makeRect(const int x1, const int y1, const int x2, const int y2)
{
  return QRect(qMin(x1, x2), qMin(y1, y2), qAbs(x2 - x1), qAbs(y2 - y1));
}

//******************************************************************
std::string featureInfo(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  std::ostringstream aStream; 
  if (theFeature)
    aStream << theFeature.get();
  return QString(aStream.str().c_str()).toStdString();
}

