// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

#include <GeomAPI_ShapeExplorer.h>

#include <Standard_NoMoreObject.hxx>
#include <TopExp_Explorer.hxx>

#define MY_EXPLORER implPtr<TopExp_Explorer>()

//=================================================================================================
GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer()
: GeomAPI_Interface(new TopExp_Explorer())
{
}

// returns a type of shape to expolode, but if toFind==SHAPE, it will return the type
// of the first sub-element of compoud if theSHape is compound
static TopAbs_ShapeEnum ShapeType(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                  const GeomAPI_Shape::ShapeType toFind)
{
  if (toFind == GeomAPI_Shape::SHAPE) {
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator anIter(aShape);
      if (anIter.More()) {
        return anIter.Value().ShapeType();
      }
    }
  }
  return (TopAbs_ShapeEnum)toFind;
}

//=================================================================================================
GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                             const GeomAPI_Shape::ShapeType toFind,
                                             const GeomAPI_Shape::ShapeType toAvoid)
: GeomAPI_Interface(new TopExp_Explorer(theShape->impl<TopoDS_Shape>(),
                                       ShapeType(theShape, toFind),
                                       (TopAbs_ShapeEnum)toAvoid))
{
}

//=================================================================================================
void GeomAPI_ShapeExplorer::init(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                 const GeomAPI_Shape::ShapeType toFind,
                                 const GeomAPI_Shape::ShapeType toAvoid)
{
  MY_EXPLORER->Init(theShape->impl<TopoDS_Shape>(),
                   ShapeType(theShape, toFind),
                   (TopAbs_ShapeEnum)toAvoid);
}

//=================================================================================================
bool GeomAPI_ShapeExplorer::more() const
{
  return MY_EXPLORER->More() == Standard_True;
}

//=================================================================================================
void GeomAPI_ShapeExplorer::next()
{
  try {
    MY_EXPLORER->Next();
  } catch (Standard_NoMoreObject) {
  }
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAPI_ShapeExplorer::current()
{
  try {
    const TopoDS_Shape& aShape = MY_EXPLORER->Current();
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(aShape));
    return aGeomShape;
  } catch (Standard_NoMoreObject) {
    return std::shared_ptr<GeomAPI_Shape>();
  }
}

//=================================================================================================
void GeomAPI_ShapeExplorer::reinit()
{
  MY_EXPLORER->ReInit();
}

//=================================================================================================
int GeomAPI_ShapeExplorer::depth() const
{
  return MY_EXPLORER->Depth();
}

//=================================================================================================
void GeomAPI_ShapeExplorer::clear()
{
  MY_EXPLORER->Clear();
}
