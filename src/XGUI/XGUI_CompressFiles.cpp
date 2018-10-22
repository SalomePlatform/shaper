// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "XGUI_CompressFiles.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>




bool XGUI_CompressFiles::compress(const QString& theFile, std::list<std::string> theFiles)
{
  QFile aOutFile(theFile);
  if (aOutFile.open(QIODevice::WriteOnly)) {
    QDataStream aOutStream(&aOutFile);

    std::list<std::string>::const_iterator aIt;
    for (aIt = theFiles.cbegin(); aIt != theFiles.cend(); ++aIt) {
      QString aPathName((*aIt).c_str());
      QFile aSrcFile(aPathName);
      QFileInfo aInfo(aSrcFile);
      quint64 aSize = aInfo.size();
      if (aSrcFile.open(QIODevice::ReadOnly)) {
        QString aName =
          aPathName.right(aPathName.length() - aPathName.lastIndexOf(QDir::separator()) - 1);

        aOutStream << aName << aSize << qCompress(aSrcFile.readAll());
        aSrcFile.close();
      }
      else {
        aOutFile.close();
        return false;
      }
    }
    aOutFile.close();
  }
  else
    return false;

  return true;
}


bool XGUI_CompressFiles::uncompress(const QString& theFile, const QString& theDir)
{
  QFile aInFile(theFile);
  QDir aDir(theDir);
  if (!aDir.exists())
    aDir.mkpath(theDir);
  if (!aDir.isEmpty()) {
    if (!aDir.removeRecursively())
      return false;
    aDir.mkdir(theDir);
  }

  if (aInFile.open(QIODevice::ReadOnly)) {
    QDataStream aInStream(&aInFile);
    do {
      QString aFileName;
      quint64 aSize;
      aInStream >> aFileName;
      aInStream >> aSize;

      QFile aOutFile(theDir + QDir::separator() + aFileName);
      if (aOutFile.open(QIODevice::WriteOnly)) {
        QByteArray aContent;
        aContent.resize(aSize);
        aInStream >> aContent;
        aOutFile.write(qUncompress(aContent));
        aOutFile.close();
      }
    } while (!aInStream.atEnd());
    aInFile.close();
  }
  return true;
}