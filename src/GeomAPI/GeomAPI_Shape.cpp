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

#include "GeomAPI_Shape.h"

#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#include <sstream>
#include <algorithm> // for std::transform

#include <BRepTools.hxx>

#define MY_SHAPE implPtr<TopoDS_Shape>()

GeomAPI_Shape::GeomAPI_Shape()
    : GeomAPI_Interface(new TopoDS_Shape())
{
}

std::shared_ptr<GeomAPI_Shape> GeomAPI_Shape::emptyCopied() const
{
  GeomShapePtr aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(MY_SHAPE->EmptyCopied()));
  return aShape;
}

bool GeomAPI_Shape::isNull() const
{
  return MY_SHAPE->IsNull() == Standard_True;
}

bool GeomAPI_Shape::isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape.get())
    return false;
  if (isNull())
    return theShape->isNull();
  if (theShape->isNull())
    return false;

  return MY_SHAPE->IsEqual(theShape->impl<TopoDS_Shape>()) == Standard_True;
}

bool GeomAPI_Shape::isSame(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape.get())
    return false;
  if (isNull())
    return theShape->isNull();
  if (theShape->isNull())
    return false;

  return MY_SHAPE->IsSame(theShape->impl<TopoDS_Shape>()) == Standard_True;
}

bool GeomAPI_Shape::isVertex() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX;
}

bool GeomAPI_Shape::isEdge() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE;
}

bool GeomAPI_Shape::isFace() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_FACE;
}

bool GeomAPI_Shape::isCompound() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPOUND;
}

bool GeomAPI_Shape::isCompoundOfSolids() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND)
    return false;
  bool isAtLeastOne = false;
  for(TopoDS_Iterator aSubs(aShape); aSubs.More(); aSubs.Next()) {
    if (aSubs.Value().IsNull() || aSubs.Value().ShapeType() != TopAbs_SOLID)
      return false;
    isAtLeastOne = true;
  }
  return isAtLeastOne;
}

GeomAPI_Shape::ShapeType GeomAPI_Shape::typeOfCompoundShapes() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND)
    return SHAPE;
  int aType = -1;
  for(TopoDS_Iterator aSubs(aShape); aSubs.More(); aSubs.Next()) {
    if (!aSubs.Value().IsNull()) {
      if (aType == -1)
        aType = aSubs.Value().ShapeType();
      else if (aSubs.Value().ShapeType() != aType)
        return SHAPE;
    }
  }
  return (GeomAPI_Shape::ShapeType) aType;
}

// adds the nopt-compound elements recursively to the list
static void addSimpleToList(const TopoDS_Shape& theShape, NCollection_List<TopoDS_Shape>& theList)
{
  if (!theShape.IsNull()) {
    if (theShape.ShapeType() == TopAbs_COMPOUND) {
      for(TopoDS_Iterator aSubs(theShape); aSubs.More(); aSubs.Next()) {
        addSimpleToList(aSubs.Value(), theList);
      }
    } else {
      theList.Append(theShape);
    }
  }
}

bool GeomAPI_Shape::isConnectedTopology() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND)
    return false;
  // list of simple elements that are not detected in connection to others
  NCollection_List<TopoDS_Shape> aNotConnected;
  addSimpleToList(aShape, aNotConnected);
  if (aNotConnected.IsEmpty()) // an empty compound
    return false;

  // collect here the group of connected subs, starting with one first element
  NCollection_List<TopoDS_Shape> aNewConnected;
  aNewConnected.Append(aNotConnected.First());
  aNotConnected.RemoveFirst();
  // iterate until some new element become connected
  while(!aNewConnected.IsEmpty() && !aNotConnected.IsEmpty()) {
    NCollection_List<TopoDS_Shape> aNew; // very new connected to new connected
    NCollection_List<TopoDS_Shape>::Iterator aNotIter(aNotConnected);
    while(aNotIter.More()) {
      // optimization to avoid TopExp_Explorer double-cycle, collect all vertices in the list first
      NCollection_List<TopoDS_Shape> aNotVertices;
      for(TopExp_Explorer anExp1(aNotIter.Value(), TopAbs_VERTEX); anExp1.More(); anExp1.Next()) {
        aNotVertices.Append(anExp1.Current());
      }

      bool aConnected =  false;
      NCollection_List<TopoDS_Shape>::Iterator aNewIter(aNewConnected);
      for(; !aConnected && aNewIter.More(); aNewIter.Next()) {
        // checking topological connecion of aNotIter and aNewIter
        // (if shapes are connected, vertices are connected for sure)
        TopExp_Explorer anExp2(aNewIter.Value(), TopAbs_VERTEX);
        for(; !aConnected && anExp2.More(); anExp2.Next()) {
          NCollection_List<TopoDS_Shape>::Iterator aNotIter(aNotVertices);
          for(; aNotIter.More(); aNotIter.Next()) {
            if (aNotIter.Value().IsSame(anExp2.Current())) {
              aConnected = true;
              break;
            }
          }
        }
      }
      if (aConnected) {
        aNew.Append(aNotIter.Value());
        aNotConnected.Remove(aNotIter);
      } else {
        aNotIter.Next();
      }
    }
    // remove all new connected and put to this list very new connected
    aNewConnected.Clear();
    aNewConnected.Append(aNew);
  }
  return aNotConnected.IsEmpty() == Standard_True;
}

bool GeomAPI_Shape::isSolid() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_SOLID;
}

bool GeomAPI_Shape::isCompSolid() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPSOLID;
}

bool GeomAPI_Shape::isPlanar() const
{
  TopoDS_Shape aShape = impl<TopoDS_Shape>();

  if(aShape.IsNull()) {
    return false;
  }

  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
  if(aShapeType == TopAbs_COMPOUND) {
    TopoDS_Iterator anIt(aShape);
    int aShNum = 0;
    for(; anIt.More(); anIt.Next()) {
      ++aShNum;
    }
    if(aShNum == 1) {
      anIt.Initialize(aShape);
      aShape = anIt.Value();
    }
  }

  aShapeType = aShape.ShapeType();
  if(aShapeType == TopAbs_VERTEX) {
    return true;
  } else if(aShapeType == TopAbs_FACE) {
    const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(TopoDS::Face(aShape));
    Handle(Standard_Type) aType = aSurface->DynamicType();

    if(aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      Handle(Geom_RectangularTrimmedSurface) aTrimSurface =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
      aType = aTrimSurface->BasisSurface()->DynamicType();
    }
    return (aType == STANDARD_TYPE(Geom_Plane)) == Standard_True;
  } else {
    BRepBuilderAPI_FindPlane aFindPlane(aShape);
    bool isFound = aFindPlane.Found() == Standard_True;

    if(!isFound && aShapeType == TopAbs_EDGE) {
      Standard_Real aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape), aFirst, aLast);
      Handle(Standard_Type) aType = aCurve->DynamicType();

      if(aType == STANDARD_TYPE(Geom_TrimmedCurve)) {
        Handle(Geom_TrimmedCurve) aTrimCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
        aType = aTrimCurve->BasisCurve()->DynamicType();
      }

      if(aType == STANDARD_TYPE(Geom_Line)
          || aType == STANDARD_TYPE(Geom_Conic)
          || aType == STANDARD_TYPE(Geom_Circle)
          || aType == STANDARD_TYPE(Geom_Ellipse)
          || aType == STANDARD_TYPE(Geom_Hyperbola)
          || aType == STANDARD_TYPE(Geom_Parabola)) {
        isFound = true;
      }
    }

    return isFound;
  }

  return false;
}

GeomAPI_Shape::ShapeType GeomAPI_Shape::shapeType() const
{
  const TopoDS_Shape& aShape = impl<TopoDS_Shape>();

  ShapeType aST = GeomAPI_Shape::SHAPE;

  switch(aShape.ShapeType()) {
  case TopAbs_COMPOUND:
    aST = GeomAPI_Shape::COMPOUND;
    break;
  case TopAbs_COMPSOLID:
    aST = GeomAPI_Shape::COMPSOLID;
    break;
  case TopAbs_SOLID:
    aST = GeomAPI_Shape::SOLID;
    break;
  case TopAbs_SHELL:
    aST = GeomAPI_Shape::SHELL;
    break;
  case TopAbs_FACE:
    aST = GeomAPI_Shape::FACE;
    break;
  case TopAbs_WIRE:
    aST = GeomAPI_Shape::WIRE;
    break;
  case TopAbs_EDGE:
    aST = GeomAPI_Shape::EDGE;
    break;
  case TopAbs_VERTEX:
    aST = GeomAPI_Shape::VERTEX;
    break;
  case TopAbs_SHAPE:
    aST = GeomAPI_Shape::SHAPE;
    break;
  }

  return aST;
}

GeomAPI_Shape::ShapeType GeomAPI_Shape::shapeTypeByStr(std::string theType)
{
  std::transform(theType.begin(), theType.end(), theType.begin(), ::toupper);
  if (theType == "COMPOUND")
    return COMPOUND;
  if (theType == "COMPSOLID")
    return COMPSOLID;
  if (theType == "SOLID")
    return SOLID;
  if (theType == "SHELL")
    return SHELL;
  if (theType == "FACE")
    return FACE;
  if (theType == "WIRE")
    return WIRE;
  if (theType == "EDGE")
    return EDGE;
  if (theType == "VERTEX")
    return VERTEX;
  return SHAPE; // default
}

std::string GeomAPI_Shape::shapeTypeStr() const
{
  ShapeType aShapeType = shapeType();
  std::string aShapeTypeStr;

  switch(aShapeType) {
    case COMPOUND: {
      aShapeTypeStr = "COMPOUND";
      break;
    }
    case COMPSOLID: {
      aShapeTypeStr = "COMPSOLID";
      break;
    }
    case SOLID: {
      aShapeTypeStr = "SOLID";
      break;
    }
    case SHELL: {
      aShapeTypeStr = "SHELL";
      break;
    }
    case FACE: {
      aShapeTypeStr = "FACE";
      break;
    }
    case WIRE: {
      aShapeTypeStr = "WIRE";
      break;
    }
    case EDGE: {
      aShapeTypeStr = "EDGE";
      break;
    }
    case VERTEX: {
      aShapeTypeStr = "VERTEX";
      break;
    }
    case SHAPE: {
      aShapeTypeStr = "SHAPE";
      break;
    }
  }

  return aShapeTypeStr;
}

GeomAPI_Shape::Orientation GeomAPI_Shape::orientation() const
{
  TopAbs_Orientation anOrientation = MY_SHAPE->Orientation();

  switch(anOrientation) {
    case TopAbs_FORWARD:  return FORWARD;
    case TopAbs_REVERSED: return REVERSED;
    case TopAbs_INTERNAL: return INTERNAL;
    case TopAbs_EXTERNAL: return EXTERNAL;
    default:              return FORWARD;
  }
}

void GeomAPI_Shape::setOrientation(const GeomAPI_Shape::Orientation theOrientation)
{
  TopAbs_Orientation anOrientation = MY_SHAPE->Orientation();

  switch(theOrientation) {
    case FORWARD:  MY_SHAPE->Orientation(TopAbs_FORWARD);  break;
    case REVERSED: MY_SHAPE->Orientation(TopAbs_REVERSED); break;
    case INTERNAL: MY_SHAPE->Orientation(TopAbs_INTERNAL); break;
    case EXTERNAL: MY_SHAPE->Orientation(TopAbs_EXTERNAL); break;
  }
}

bool GeomAPI_Shape::isSubShape(const std::shared_ptr<GeomAPI_Shape> theShape,
                               const bool theCheckOrientation) const
{
  if(!theShape.get()) {
    return false;
  }

  const TopoDS_Shape& aShapeToSearch = theShape->impl<TopoDS_Shape>();
  if(aShapeToSearch.IsNull()) {
    return false;
  }

  for(TopExp_Explorer anExp(*MY_SHAPE, aShapeToSearch.ShapeType()); anExp.More(); anExp.Next()) {
    if(theCheckOrientation ?
       aShapeToSearch.IsEqual(anExp.Current()) : aShapeToSearch.IsSame(anExp.Current())) {
      return true;
    }
  }

  return false;
}

bool GeomAPI_Shape::computeSize(double& theXmin, double& theYmin, double& theZmin,
                                double& theXmax, double& theYmax, double& theZmax) const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;
  Bnd_Box aBndBox;
  BRepBndLib::Add(aShape, aBndBox);
  aBndBox.Get(theXmin, theYmin, theZmin, theXmax, theYmax, theZmax);
  return true;
}

std::string GeomAPI_Shape::getShapeStream() const
{
  std::ostringstream aStream;
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  BRepTools::Write(aShape, aStream);
  return aStream.str();
}

GeomShapePtr GeomAPI_Shape::intersect(const GeomShapePtr theShape) const
{
  const TopoDS_Shape& aShape1 = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aShape2 = theShape->impl<TopoDS_Shape>();

  BRepAlgoAPI_Section aCommon(aShape1, aShape2);
  if (!aCommon.IsDone())
    return GeomShapePtr();

  TopoDS_Shape aResult = aCommon.Shape();
  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    NCollection_List<TopoDS_Shape> aSubs;
    addSimpleToList(aResult, aSubs);
    if(aSubs.Size() == 1) {
      aResult = aSubs.First();
    } else if(aSubs.Size() == 0) {
      return GeomShapePtr();
    }
  }

  GeomShapePtr aResShape(new GeomAPI_Shape);
  aResShape->setImpl(new TopoDS_Shape(aResult));
  return aResShape;
}

bool GeomAPI_Shape::isIntersect(const GeomShapePtr theShape) const
{
  if(!theShape.get()) {
    return false;
  }

  const TopoDS_Shape& aShape1 = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aShape2 = theShape->impl<TopoDS_Shape>();

  BRepExtrema_DistShapeShape aDist(aShape1, aShape2);
  aDist.Perform();
  if(aDist.IsDone() && aDist.Value() < Precision::Confusion()) {
    return true;
  }

  return false;
}

void GeomAPI_Shape::translate(const std::shared_ptr<GeomAPI_Dir> theDir, const double theOffset)
{
  gp_Dir aDir = theDir->impl<gp_Dir>();
  gp_Vec aTrsfVec(aDir.XYZ() * theOffset);
  gp_Trsf aTranslation;
  aTranslation.SetTranslation(aTrsfVec);
  TopoDS_Shape aResult = MY_SHAPE->Moved(aTranslation);
  setImpl(new TopoDS_Shape(aResult));
}
