// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterShapeType.cpp
// Created:     12 Jan 2015
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterShapeType.h"

#include <StdSelect_ShapeTypeFilter.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <Events_Error.h>

#include <QString>
#include <QMap>

typedef QMap<QString, TopAbs_ShapeEnum> FaceTypes;
static FaceTypes MyShapeTypes;

TopAbs_ShapeEnum ModuleBase_FilterShapeType::shapeType(const std::string& theType)
{
  if (MyShapeTypes.count() == 0) {
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
  }
  QString aType = QString(theType.c_str()).toLower();
  if (MyShapeTypes.contains(aType))
    return MyShapeTypes[aType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}

ModuleBase_FilterShapeType::ModuleBase_FilterShapeType()
: ModuleBase_Filter(), myShapeType(TopAbs_SHAPE)
{
}

void ModuleBase_FilterShapeType::createFilter()
{
  myFilter = new StdSelect_ShapeTypeFilter(myShapeType);
}

void ModuleBase_FilterShapeType::setArguments(const std::list<std::string>& theArguments)
{
  if (theArguments.size()!= 1)
    return;
  myShapeType = shapeType(theArguments.front());
}
