// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ModuleBase_IconFactory.h"

#include "Config_XMLReader.h"

#include <QDir>

ModuleBase_IconFactory* MYIconFactory = 0;


ModuleBase_IconFactory::ModuleBase_IconFactory()
{
  setFactory(this);
}

void ModuleBase_IconFactory::setFactory(ModuleBase_IconFactory* theFactory)
{
  if (MYIconFactory)
    delete MYIconFactory;
  MYIconFactory = theFactory;
}

ModuleBase_IconFactory* ModuleBase_IconFactory::get()
{
  if (!MYIconFactory) {
    MYIconFactory = new ModuleBase_IconFactory();
  }
  return MYIconFactory;
}

QIcon ModuleBase_IconFactory::getIcon(ObjectPtr theIcon)
{
  return QIcon();
}

QIcon ModuleBase_IconFactory::loadIcon(const QString& theValue)
{
  return QIcon(loadPixmap(theValue));
}

QPixmap ModuleBase_IconFactory::loadPixmap(const QString& theValue)
{
  QPixmap aPixmap(theValue);

  if (aPixmap.isNull()) {
    std::string aPath = Config_XMLReader::findConfigFile(theValue.toStdString());
    if (!aPath.empty())
      aPixmap = QPixmap(QString::fromStdString(aPath));
  }
  return aPixmap;
}

QImage ModuleBase_IconFactory::loadImage(const QString& theValue)
{
  QImage anImage(theValue);

  if (anImage.isNull()) {
    std::string aPath = Config_XMLReader::findConfigFile(theValue.toStdString());
    if (!aPath.empty())
      anImage = QImage(QString::fromStdString(aPath));
  }
  return anImage;
}
