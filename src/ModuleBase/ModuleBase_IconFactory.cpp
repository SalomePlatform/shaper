// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IconFactory.cpp
// Created:     28 Jul 2015
// Author:      Vitaly SMETANNIKOV

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
    std::string aPluginPath = Config_XMLReader::pluginConfigFile();
    QString aPath = QString::fromStdString(aPluginPath) + QDir::separator() + theValue;
    if (QFile::exists(aPath))
      aPixmap = QPixmap(aPath);
  }
  return aPixmap;
}

QImage ModuleBase_IconFactory::loadImage(const QString& theValue)
{
  QImage anImage(theValue);

  if (anImage.isNull()) {
    std::string aPluginPath = Config_XMLReader::pluginConfigFile();
    QString aPath = QString::fromStdString(aPluginPath) + QDir::separator() + theValue;
    if (QFile::exists(aPath))
      anImage = QImage(aPath);
  }
  return anImage;
}
