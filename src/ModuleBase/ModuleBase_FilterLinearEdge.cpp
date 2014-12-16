// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterLinearEdge.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterLinearEdge.h"

#include <StdSelect_EdgeFilter.hxx>
#include <StdSelect_TypeOfEdge.hxx>

#include <Events_Error.h>

#include <QString>
#include <QMap>

typedef QMap<QString, StdSelect_TypeOfEdge> EdgeTypes;
static EdgeTypes MyEdgeTypes;

StdSelect_TypeOfEdge ModuleBase_FilterLinearEdge::edgeType(const std::string& theType)
{
  if (MyEdgeTypes.count() == 0) {
    MyEdgeTypes["line"] = StdSelect_Line;
    MyEdgeTypes["circle"] = StdSelect_Circle;
  }
  QString aType = QString(theType.c_str()).toLower();
  if (MyEdgeTypes.contains(aType))
    return MyEdgeTypes[aType];
  Events_Error::send("Edge type defined in XML is not implemented!");
  return StdSelect_AnyEdge;
}

void ModuleBase_FilterLinearEdge::createFilter()
{
  myFilter = new StdSelect_EdgeFilter(StdSelect_AnyEdge);
}

void ModuleBase_FilterLinearEdge::setArguments(const std::list<std::string>& theArguments)
{
  if (theArguments.size()!= 1)
    return;

  std::string anArgument = theArguments.front();
  Handle(StdSelect_EdgeFilter) aFilter = Handle(StdSelect_EdgeFilter)::DownCast(getFilter());
  if (!aFilter.IsNull())
    aFilter->SetType(edgeType(anArgument));
}
