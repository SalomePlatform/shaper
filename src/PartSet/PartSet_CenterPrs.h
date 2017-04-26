// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CenterPrs.h
// Created:     25 April 2017
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_CenterPrs_H
#define PartSet_CenterPrs_H

#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeSelection.h>
#include <GeomAPI_Edge.h>

#include <AIS_Point.hxx>
#include <Standard_DefineHandle.hxx>
#include <gp_Pnt.hxx>

DEFINE_STANDARD_HANDLE(PartSet_CenterPrs, AIS_Point)

/**
* \ingroup GUI
* A presentation class for displaying of centers of external curcular objects in a sketch
*/
class PartSet_CenterPrs: public AIS_Point
{
public:
  /// Constructor
  /// \param theObject an object with circular edge
  /// \param theEdge a circular edge
  /// \param theCenter a center point of the circular edge
  /// \param theType a type of the center
  Standard_EXPORT PartSet_CenterPrs(const ObjectPtr& theObject,
                                    const GeomEdgePtr& theEdge,
                                    const gp_Pnt& theCenter,
                                    ModelAPI_AttributeSelection::CenterType theType);

  /// Returns an Object which contains the circular edge
  ObjectPtr object() const { return myObject; }

  /// Returns a circular edge shape
  GeomEdgePtr edge() const { return myEdge; }

  /// Returns type of the center
  ModelAPI_AttributeSelection::CenterType centerType() const { return myCenterType; }

  DEFINE_STANDARD_RTTIEXT(PartSet_CenterPrs, AIS_Point)

private:
  ObjectPtr myObject;
  GeomEdgePtr myEdge;
  ModelAPI_AttributeSelection::CenterType myCenterType;
};

#endif
