// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeRefAttr.h>

void GeomDataAPI_Point2D::move(const double theDeltaX, const double theDeltaY)
{
  setValue(x() + theDeltaX, y() + theDeltaY);
}

std::string GeomDataAPI_Point2D::attributeType()
{
  return typeId();
}

std::shared_ptr<GeomDataAPI_Point2D> GeomDataAPI_Point2D::getPoint2D(
                                  const DataPtr& theData, const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
  if (!theData)
    return aPointAttr;

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
                                 ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr && anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  return aPointAttr;
}


GeomDataAPI_Point2D::GeomDataAPI_Point2D()
{
}

GeomDataAPI_Point2D::~GeomDataAPI_Point2D()
{
}
