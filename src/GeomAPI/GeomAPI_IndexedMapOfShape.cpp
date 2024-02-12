// Copyright (C) 2014-2024  CEA, EDF
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

#include <GeomAPI_IndexedMapOfShape.h>

#include <gp_Trsf.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

GeomAPI_IndexedMapOfShape::GeomAPI_IndexedMapOfShape(const std::shared_ptr<GeomAPI_Shape> theMainShape)
{
  MapShapes(theMainShape);
}

void GeomAPI_IndexedMapOfShape::MapShapes(const std::shared_ptr<GeomAPI_Shape> theMainShape)
{
  if (!empty()) {
    implPtr<TopTools_IndexedMapOfShape>()->Clear();
  }

  TopoDS_Shape aMainShape = theMainShape->impl<TopoDS_Shape>();
  if (!aMainShape.IsNull()) {
    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(aMainShape, aSubShapesMap);

    setImpl(new TopTools_IndexedMapOfShape(aSubShapesMap));
  }
}

int GeomAPI_IndexedMapOfShape::FindIndex(std::shared_ptr<GeomAPI_Shape> theKey)
{
  return impl<TopTools_IndexedMapOfShape>().FindIndex(theKey->impl<TopoDS_Shape>());
}

// Returns true if transformations are equal with the given precision
static bool isEqual(const gp_Trsf& theT1, const gp_Trsf& theT2, const double thePrecision)
{
  for(int aRow = 1; aRow < 4; aRow++) {
    for(int aCol = 1; aCol < 5; aCol++) {
      double aDiff = theT1.Value(aRow, aCol) - theT2.Value(aRow, aCol);
      if (aDiff < 0) aDiff = -aDiff;
      if (aDiff > thePrecision)
        return false;
    }
  }
  return true;
}

int GeomAPI_IndexedMapOfShape::FindIndexEqualLocations(std::shared_ptr<GeomAPI_Shape> theKey)
{
  int anID = impl<TopTools_IndexedMapOfShape>().FindIndex(theKey->impl<TopoDS_Shape>());
  if (anID == 0) {
    // Try to search shape with the same location if TopLoc_Location is different.
    // It's a special fix for the problem related to the Placement of parts
    // feature - it adds additional transformation to all results and groups.
    const TopoDS_Shape& aMainShape = impl<TopTools_IndexedMapOfShape>().FindKey(1);
    const TopoDS_Shape& aSubShape = theKey->impl<TopoDS_Shape>();
    TopExp_Explorer anExp (aMainShape, aSubShape.ShapeType());
    for (; anExp.More(); anExp.Next()) {
      if (anExp.Current().TShape() == aSubShape.TShape()) {
        const TopLoc_Location aLoc1 = anExp.Current().Location();
        if (isEqual(aLoc1.Transformation(), aSubShape.Location().Transformation(), 1.e-7)) {
          anID = impl<TopTools_IndexedMapOfShape>().FindIndex(anExp.Current());
          break;
        }
      }
    }
  }
  return anID;
}

GeomAPI_IndexedMapOfShape::~GeomAPI_IndexedMapOfShape()
{
  if (!empty()) {
    implPtr<TopTools_IndexedMapOfShape>()->Clear();
  }
}
