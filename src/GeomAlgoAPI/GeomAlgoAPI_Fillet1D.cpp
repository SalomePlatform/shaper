// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Fillet1D.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_MapShapesAndAncestors.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Wire.h>
#include <GeomAPI_WireExplorer.h>

#include <GEOMImpl_Fillet1d.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ShapeFix_Wire.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

static GeomShapePtr convert(const TopoDS_Shape& theShape)
{
  GeomShapePtr aNewShape(new GeomAPI_Shape);
  aNewShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(theShape));
  return aNewShape;
}

static void substituteNewEdge(GeomEdgePtr theEdge,
    std::map<GeomShapePtr, ListOfShape, GeomAPI_Shape::Comparator>& theMap)
{
  std::map<GeomShapePtr, ListOfShape, GeomAPI_Shape::Comparator>::iterator anIt = theMap.begin();
  for (; anIt != theMap.end(); ++anIt) {
    for (ListOfShape::iterator aEIt = anIt->second.begin(); aEIt != anIt->second.end(); ++aEIt)
      if (theEdge->isEqual(*aEIt)) {
        // substitute edge and stop iteration
        *aEIt = theEdge;
        return;
      }
  }
}


GeomAlgoAPI_Fillet1D::GeomAlgoAPI_Fillet1D(const GeomShapePtr& theBaseWire,
                                           const ListOfShape&  theFilletVertices,
                                           const double        theFilletRadius)
{
  build(theBaseWire, theFilletVertices, theFilletRadius);
}

void GeomAlgoAPI_Fillet1D::build(const GeomShapePtr& theBaseWire,
                                 const ListOfShape&  theFilletVertices,
                                 const double        theRadius)
{
  if (!theBaseWire || theFilletVertices.empty() || theRadius < 0.)
    return;

  myFailedVertices.clear();
  // store all edges of a base wire as modified, because they will be rebuild by ShapeFix
  for (GeomAPI_WireExplorer aWExp(theBaseWire->wire()); aWExp.more(); aWExp.next()) {
    GeomShapePtr aCurrent = aWExp.current();
    GeomAlgoAPI_Copy aCopy(aCurrent);
    myModified[aCurrent].push_back(aCopy.shape());
  }

  GeomAlgoAPI_MapShapesAndAncestors aMapVE(theBaseWire, GeomAPI_Shape::VERTEX,
                                                        GeomAPI_Shape::EDGE);

  for (ListOfShape::const_iterator aVIt = theFilletVertices.begin();
       aVIt != theFilletVertices.end(); ++aVIt) {
    // get edges to perform fillet
    MapShapeToShapes::const_iterator aVE = aMapVE.map().find(*aVIt);
    if (aVE == aMapVE.map().end())
      continue;
    ListOfShape anEdges;
    for (SetOfShapes::const_iterator aEIt = aVE->second.begin();
         aEIt != aVE->second.end(); ++aEIt) {
      ListOfShape aNewEdges;
      modified(*aEIt, aNewEdges);
      if (aNewEdges.empty())
        anEdges.push_back(*aEIt);
      else
        anEdges.insert(anEdges.end(), aNewEdges.begin(), aNewEdges.end());
    }

    GeomPlanePtr aPlane = GeomAlgoAPI_ShapeTools::findPlane(anEdges);
    if (!aPlane)
      return; // non-planar edges

    TopoDS_Edge anEdge1 = TopoDS::Edge(anEdges.front()->impl<TopoDS_Shape>());
    TopoDS_Edge anEdge2 = TopoDS::Edge(anEdges.back()->impl<TopoDS_Shape>());

    // create fillet builder
    GEOMImpl_Fillet1d aFilletBuilder(anEdge1, anEdge2, aPlane->impl<gp_Pln>());
    bool isOk = aFilletBuilder.Perform(theRadius);
    TopoDS_Edge aFilletEdge;
    if (isOk) {
      GeomPointPtr aPoint = aVE->first->vertex()->point();
      aFilletEdge = aFilletBuilder.Result(aPoint->impl<gp_Pnt>(), anEdge1, anEdge2);
      isOk = !aFilletEdge.IsNull();
    }

    if (!isOk) {
      // something gone wrong and the fillet edge is not constructed,
      // just store the failed vertex and continue
      myFailedVertices.push_back(*aVIt);
      continue;
    }

    // store modified shapes
    myGenerated[aVE->first].push_back(convert(aFilletEdge));
    SetOfShapes::const_iterator aEIt = aVE->second.begin();
    myModified[*aEIt].clear();
    myModified[*aEIt].push_back(convert(anEdge1));
    myModified[*(++aEIt)].clear();
    myModified[*aEIt].push_back(convert(anEdge2));
  }

  // compose a new wire
  TopoDS_Wire aNewWire;
  BRep_Builder aBuilder;
  aBuilder.MakeWire(aNewWire);
  GeomWirePtr aBaseWire = theBaseWire->wire();
  GeomAPI_WireExplorer aWExp(aBaseWire);
  GeomShapePtr aBaseFirstEdge = aWExp.current();
  for (; aWExp.more(); aWExp.next()) {
    ListOfShape aNewEdges;
    modified(aWExp.current(), aNewEdges);
    if (aNewEdges.empty())
      aNewEdges.push_back(aWExp.current());
    for (ListOfShape::iterator anIt = aNewEdges.begin(); anIt != aNewEdges.end(); ++anIt)
      aBuilder.Add(aNewWire, TopoDS::Edge((*anIt)->impl<TopoDS_Shape>()));
  }
  for (MapModified::iterator aGenIt = myGenerated.begin(); aGenIt != myGenerated.end(); ++aGenIt) {
    for (ListOfShape::iterator anIt = aGenIt->second.begin(); anIt != aGenIt->second.end(); ++anIt)
      aBuilder.Add(aNewWire, TopoDS::Edge((*anIt)->impl<TopoDS_Shape>()));
  }
  // fix the wire connectivity
  ShapeFix_Wire aFixWire;
  aFixWire.Load(aNewWire);
  aFixWire.ClosedWireMode() = aBaseWire->isClosed();
  aFixWire.FixReorder();
  aNewWire = aFixWire.WireAPIMake();
  if (aNewWire.IsNull()) {
    myFailedVertices = theFilletVertices;
    return;
  }

  // update the map of modified shapes, because the edges are changed by ShapeFix
  for (BRepTools_WireExplorer anExp(aNewWire); anExp.More(); anExp.Next()) {
    GeomEdgePtr aCurrent(new GeomAPI_Edge);
    aCurrent->setImpl(new TopoDS_Edge(anExp.Current()));
    substituteNewEdge(aCurrent, myGenerated);
    substituteNewEdge(aCurrent, myModified);
  }

  // rebuild the wire once again to get the first edge of fillet wire correspond
  // to the first edge of original wire
  TopoDS_Edge aFirstEdge = TopoDS::Edge(aBaseFirstEdge->impl<TopoDS_Shape>());
  ListOfShape aNewEdges;
  modified(aBaseFirstEdge, aNewEdges);
  if (!aNewEdges.empty())
    aFirstEdge = TopoDS::Edge(aNewEdges.front()->impl<TopoDS_Shape>());
  TopTools_ListOfShape aKeptForEnd;
  BRepTools_WireExplorer aNewExp(aNewWire);
  for (; aNewExp.More(); aNewExp.Next())
    if (aNewExp.Current().IsEqual(aFirstEdge))
      break;
  if (aNewExp.More()) {
    TopoDS_Wire aReorderedWire;
    aBuilder.MakeWire(aReorderedWire);
    for (; aNewExp.More(); aNewExp.Next())
      aBuilder.Add(aReorderedWire, aNewExp.Current());
    for (aNewExp.Init(aNewWire); aNewExp.More(); aNewExp.Next()) {
      if (aNewExp.Current().IsEqual(aFirstEdge))
        break;
      aBuilder.Add(aReorderedWire, aNewExp.Current());
    }
    aNewWire = aReorderedWire;
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aNewWire));
  myModified[theBaseWire].push_back(aShape);

  setShape(aShape);
  setDone(myFailedVertices.empty());
}

void GeomAlgoAPI_Fillet1D::generated(const GeomShapePtr theOldShape,
                                     ListOfShape& theNewShapes)
{
  MapModified::iterator aFound = myGenerated.find(theOldShape);
  if (aFound != myGenerated.end())
    theNewShapes = aFound->second;
}

void GeomAlgoAPI_Fillet1D::modified(const GeomShapePtr theOldShape,
                                     ListOfShape& theNewShapes)
{
  MapModified::iterator aFound = myModified.find(theOldShape);
  if (aFound != myModified.end())
    theNewShapes = aFound->second;
}
