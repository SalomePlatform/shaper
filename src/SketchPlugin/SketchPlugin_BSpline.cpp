// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#include <SketchPlugin_BSpline.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>


SketchPlugin_BSpline::SketchPlugin_BSpline()
  : SketchPlugin_BSplineBase()
{
}

void SketchPlugin_BSpline::initDerivedClassAttributes()
{
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId());

  SketchPlugin_BSplineBase::initDerivedClassAttributes();
}

void SketchPlugin_BSpline::attributeChanged(const std::string& theID) {
  if (theID == POLES_ID()) {
    AttributePoint2DArrayPtr aPolesArray =
        std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POLES_ID()));
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        attribute(START_ID()))->setValue(aPolesArray->pnt(0));
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        attribute(END_ID()))->setValue(aPolesArray->pnt(aPolesArray->size() - 1));
  }
  else
    SketchPlugin_BSplineBase::attributeChanged(theID);
}
