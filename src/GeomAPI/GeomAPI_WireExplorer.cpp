// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <GeomAPI_WireExplorer.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Wire.h>

#include <BRepTools_WireExplorer.hxx>

#define MY_EXPLORER implPtr<BRepTools_WireExplorer>()

GeomAPI_WireExplorer::GeomAPI_WireExplorer()
  : GeomAPI_Interface(new BRepTools_WireExplorer())
{
}

GeomAPI_WireExplorer::GeomAPI_WireExplorer(const GeomWirePtr& theWire)
  : GeomAPI_Interface(new BRepTools_WireExplorer(theWire->impl<TopoDS_Wire>()))
{
}

void GeomAPI_WireExplorer::init(const GeomWirePtr& theWire)
{
  MY_EXPLORER->Init(theWire->impl<TopoDS_Wire>());
}

bool GeomAPI_WireExplorer::more() const
{
  return MY_EXPLORER->More() == Standard_True;
}

void GeomAPI_WireExplorer::next()
{
  MY_EXPLORER->Next();
}

std::shared_ptr<GeomAPI_Edge> GeomAPI_WireExplorer::current()
{
  const TopoDS_Edge& aShape = MY_EXPLORER->Current();
  std::shared_ptr<GeomAPI_Edge> aGeomShape(new GeomAPI_Edge());
  aGeomShape->setImpl(new TopoDS_Edge(aShape));
  return aGeomShape;
}

std::shared_ptr<GeomAPI_Vertex> GeomAPI_WireExplorer::currentVertex()
{
  const TopoDS_Vertex& aShape = MY_EXPLORER->CurrentVertex();
  std::shared_ptr<GeomAPI_Vertex> aGeomShape(new GeomAPI_Vertex());
  aGeomShape->setImpl(new TopoDS_Vertex(aShape));
  return aGeomShape;
}

void GeomAPI_WireExplorer::clear()
{
  MY_EXPLORER->Clear();
}
