// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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
#include <Geom_Point.hxx>

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

  virtual void HilightSelected(const Handle(PrsMgr_PresentationManager3d)& PM,
                               const SelectMgr_SequenceOfOwner& Seq);
  virtual void HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager3d)&,
                                      const Handle(Prs3d_Drawer)&,
                                      const Handle(SelectMgr_EntityOwner)&);

  DEFINE_STANDARD_RTTIEXT(PartSet_CenterPrs, AIS_Point)

private:
  void drawPoint(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor);

  ObjectPtr myObject;
  GeomEdgePtr myEdge;
  ModelAPI_AttributeSelection::CenterType myCenterType;
};

#endif
