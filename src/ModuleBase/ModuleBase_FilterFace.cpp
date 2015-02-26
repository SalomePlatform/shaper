// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterFace.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterFace.h"

#include <StdSelect_FaceFilter.hxx>
#include <StdSelect_TypeOfFace.hxx>

#include <Events_Error.h>

#include <QString>
#include <QMap>

typedef QMap<QString, StdSelect_TypeOfFace> FaceTypes;
static FaceTypes MyFaceTypes;

StdSelect_TypeOfFace ModuleBase_FilterFace::faceType(const std::string& theType)
{
  if (MyFaceTypes.count() == 0) {
    MyFaceTypes["plane"] = StdSelect_Plane;
    MyFaceTypes["cylinder"] = StdSelect_Cylinder;
  }
  QString aType = QString(theType.c_str()).toLower();
  if (MyFaceTypes.contains(aType))
    return MyFaceTypes[aType];
  Events_Error::send("Face type defined in XML is not implemented!");
  return StdSelect_AnyFace;
}

void ModuleBase_FilterFace::createFilter()
{
  myFilter = new StdSelect_FaceFilter(StdSelect_AnyFace);
}

void ModuleBase_FilterFace::setArguments(const std::list<std::string>& theArguments)
{
  if (theArguments.size()!= 1)
    return;

  std::string anArgument = theArguments.front();
  Handle(StdSelect_FaceFilter) aFilter = Handle(StdSelect_FaceFilter)::DownCast(getFilter());
  if (!aFilter.IsNull())
    aFilter->SetType(faceType(anArgument));
}
