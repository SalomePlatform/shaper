// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <GeomAPI_Shape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopoDS_Shape.hxx>

GeomAPI_DataMapOfShapeShape::GeomAPI_DataMapOfShapeShape()
: GeomAPI_Interface(new TopTools_DataMapOfShapeShape){}

void GeomAPI_DataMapOfShapeShape::clear()
{
  implPtr<TopTools_DataMapOfShapeShape>()->Clear();
}

int GeomAPI_DataMapOfShapeShape::size()
{
  return implPtr<TopTools_DataMapOfShapeShape>()->Extent();
}

bool GeomAPI_DataMapOfShapeShape::bind(std::shared_ptr<GeomAPI_Shape> theKey,
                                       std::shared_ptr<GeomAPI_Shape> theItem)
{
  bool flag(false);
  if (implPtr<TopTools_DataMapOfShapeShape>()->Bind(theKey->impl<TopoDS_Shape>(),
                                                    theItem->impl<TopoDS_Shape>()))
    flag = true;
  return flag;
}

void GeomAPI_DataMapOfShapeShape::merge(const GeomAPI_DataMapOfShapeShape& theDataMap)
{
  const TopTools_DataMapOfShapeShape& aDataMap = theDataMap.impl<TopTools_DataMapOfShapeShape>();
  TopTools_DataMapOfShapeShape* myDataMap = implPtr<TopTools_DataMapOfShapeShape>();
  for(TopTools_DataMapIteratorOfDataMapOfShapeShape anIt(aDataMap); anIt.More(); anIt.Next()) {
    myDataMap->Bind(anIt.Key(), anIt.Value());
  }
}

void GeomAPI_DataMapOfShapeShape::
  merge(const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theDataMap)
{
  if(theDataMap.get()) {
    merge(*theDataMap.get());
  }
}

bool GeomAPI_DataMapOfShapeShape::isBound (std::shared_ptr<GeomAPI_Shape> theKey)
{
  bool flag(false);
  if(impl<TopTools_DataMapOfShapeShape>().IsBound(theKey->impl<TopoDS_Shape>()))
    flag = true;
  return flag;
}

const std::shared_ptr<GeomAPI_Shape>
  GeomAPI_DataMapOfShapeShape::find(std::shared_ptr<GeomAPI_Shape> theKey)
{
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(
    new TopoDS_Shape(impl<TopTools_DataMapOfShapeShape>().Find(theKey->impl<TopoDS_Shape>())));
  return aShape;
}

bool GeomAPI_DataMapOfShapeShape::unBind(std::shared_ptr<GeomAPI_Shape> theKey)
{
  bool flag(false);
  if(implPtr<TopTools_DataMapOfShapeShape>()->UnBind(theKey->impl<TopoDS_Shape>()))
  flag = true;
  return flag;
}

 GeomAPI_DataMapOfShapeShape::~GeomAPI_DataMapOfShapeShape()
 {
  if (!empty()) {
    implPtr<TopTools_DataMapOfShapeShape>()->Clear();
  }
 }
