// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CenterPrs.h
// Created:     25 April 2017
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_CenterPrs_H
#define PartSet_CenterPrs_H

#include <ModelAPI_Object.h>
#include <GeomAPI_Shape.h>

#include <AIS_Point.hxx>
#include <Standard_DefineHandle.hxx>
#include <gp_Pnt.hxx>

DEFINE_STANDARD_HANDLE(PartSet_CenterPrs, AIS_Point)

class PartSet_CenterPrs: public AIS_Point
{
public:
  /// Constructor
  /// \param theResult a result object
  Standard_EXPORT PartSet_CenterPrs(const ObjectPtr& theObject,
                                    const GeomShapePtr& theEdge,
                                    const gp_Pnt& theCenter);

  ObjectPtr object() const { return myObject; }
  GeomShapePtr edge() const { return myEdge; }

  DEFINE_STANDARD_RTTIEXT(PartSet_CenterPrs, AIS_Point)
private:
  ObjectPtr myObject;
  GeomShapePtr myEdge;
};

#endif
