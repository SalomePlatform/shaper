// Copyright (C) 2017  CEA/DEN, EDF R&D
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

// File:        PartSet_CenterPrs.cpp
// Created:     25 April 2017
// Author:      Vitaly SMETANNIKOV

#include "PartSet_CenterPrs.h"

#include <Geom_CartesianPoint.hxx>


IMPLEMENT_STANDARD_RTTIEXT(PartSet_CenterPrs, AIS_Point)

PartSet_CenterPrs::PartSet_CenterPrs(const ObjectPtr& theObject,
                                     const GeomEdgePtr& theEdge,
                                     const gp_Pnt& theCenter,
                                     ModelAPI_AttributeSelection::CenterType theType)
  : AIS_Point(new Geom_CartesianPoint(theCenter)),
  myObject(theObject),
  myEdge(theEdge),
  myCenterType(theType)
{
}