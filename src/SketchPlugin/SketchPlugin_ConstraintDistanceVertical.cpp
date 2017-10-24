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

// File:    SketchPlugin_ConstraintDistanceVertical.cpp
// Created: 10 May 2017
// Author:  Artem ZHIDKOV

#include <SketchPlugin_ConstraintDistanceVertical.h>

#include <SketcherPrs_Tools.h>

#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>


SketchPlugin_ConstraintDistanceVertical::SketchPlugin_ConstraintDistanceVertical()
  : SketchPlugin_ConstraintDistanceAlongDir()
{
}

double SketchPlugin_ConstraintDistanceVertical::calculateCurrentDistance()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
    SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B(), aPlane);

  return aPointB->y() - aPointA->y();
}

void SketchPlugin_ConstraintDistanceVertical::updateFlyoutPoint()
{
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = aFlyoutAttr->pnt();

  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aEndPoint = SketcherPrs_Tools::getFeaturePoint(
      data(), SketchPlugin_Constraint::ENTITY_B(), aPlane);

  std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aEndPoint->pnt()->xy());
  double X =  aFlyoutDir->y(); // Dot on OY axis
  double Y = -aFlyoutDir->x(); // Cross to OY axis
  aFlyoutAttr->setValue(X, Y);
}
