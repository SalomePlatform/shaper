// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_WireBuilder.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_ShapeExplorer.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools_ReShape.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <cmath>
#include <map>
#include <set>

class SetOfEdges
{
  class DoubleCompare {
  public:
    bool operator() (const double d1, const double d2) const {
      return d1 + Precision::Confusion() < d2;
    }
  };

  typedef std::map<double, std::set<double, DoubleCompare>, DoubleCompare> ParamMap;
  std::map<Handle(Geom_Curve), ParamMap> myShapes;

public:
  bool add(const TopoDS_Shape& theEdge)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(theEdge);
    if (anEdge.IsNull())
      return true;

    double aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);

    bool isAdded = true;
    std::map<Handle(Geom_Curve), ParamMap>::iterator
        aFound = myShapes.find(aCurve);
    if (aFound == myShapes.end())
      myShapes[aCurve][aFirst].insert(aLast);
    else {
      ParamMap::iterator aFoundPar = aFound->second.find(aFirst);
      if (aFoundPar == aFound->second.end())
        aFound->second[aFirst].insert(aLast);
      else if (aFoundPar->second.find(aLast) == aFoundPar->second.end())
        aFoundPar->second.insert(aLast);
      else
        isAdded = false;
    }
    return isAdded;
  }

  static bool isEqual(const TopoDS_Shape& theShape1, const TopoDS_Shape& theShape2)
  {
    const TopoDS_Edge& anEdge1 = TopoDS::Edge(theShape1);
    const TopoDS_Edge& anEdge2 = TopoDS::Edge(theShape2);
    if (anEdge1.IsNull() || anEdge2.IsNull())
      return false;

    double aFirst1, aLast1;
    Handle(Geom_Curve) aCurve1 = BRep_Tool::Curve(anEdge1, aFirst1, aLast1);
    double aFirst2, aLast2;
    Handle(Geom_Curve) aCurve2 = BRep_Tool::Curve(anEdge2, aFirst2, aLast2);
    return aCurve1 == aCurve2 && fabs(aFirst1 - aFirst2) < Precision::Confusion() &&
                                 fabs(aLast1 - aLast2) < Precision::Confusion();
  }
};

static GeomShapePtr fromTopoDS(const TopoDS_Shape& theShape)
{
  GeomShapePtr aResultShape(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(theShape));
  return aResultShape;
}

GeomAlgoAPI_WireBuilder::GeomAlgoAPI_WireBuilder(const ListOfShape& theShapes,
                                                 const bool theForceOpenWire)
{
  TopTools_ListOfShape aListOfEdges;
  SetOfEdges aProcessedEdges;

  ListOfShape::const_iterator anIt = theShapes.cbegin();
  for (; anIt != theShapes.cend(); ++anIt) {
    TopoDS_Shape aShape = (*anIt)->impl<TopoDS_Shape>();
    switch(aShape.ShapeType()) {
      case TopAbs_EDGE: {
        aShape.Orientation(TopAbs_FORWARD);
        if (aProcessedEdges.add(aShape))
          aListOfEdges.Append(aShape);
        break;
      }
      case TopAbs_WIRE: {
        for (TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
          TopoDS_Shape anEdge = anExp.Current();
          anEdge.Orientation(TopAbs_FORWARD);
          // if the edge was already processed, remove it to keep original order of the current wire
          if (!aProcessedEdges.add(anEdge)) {
            for (TopTools_ListIteratorOfListOfShape aEIt(aListOfEdges); aEIt.More(); aEIt.Next())
              if (SetOfEdges::isEqual(anEdge, aEIt.Value())) {
                aListOfEdges.Remove(aEIt);
                break;
              }
          }
          aListOfEdges.Append(anEdge);
        }
        break;
      }
    default:
      break;
    }
  }

  bool isSplitWire = false;
  gp_Pnt aSplitPoint;
  if (theForceOpenWire && aListOfEdges.Size() > 1) {
    // find a vertex to split the wire
    TopoDS_Vertex V1[2];
    TopExp::Vertices(TopoDS::Edge(aListOfEdges.First()), V1[0], V1[1]);
    TopoDS_Vertex V2[2];
    TopExp::Vertices(TopoDS::Edge(aListOfEdges.Last()), V2[0], V2[1]);
    gp_Pnt P1[2] = { BRep_Tool::Pnt(V1[0]), BRep_Tool::Pnt(V1[1]) };
    gp_Pnt P2[2] = { BRep_Tool::Pnt(V2[0]), BRep_Tool::Pnt(V2[1]) };
    double Tol1[2] = { BRep_Tool::Tolerance(V1[0]), BRep_Tool::Tolerance(V1[1]) };
    double Tol2[2] = { BRep_Tool::Tolerance(V2[0]), BRep_Tool::Tolerance(V2[1]) };
    for (int i = 0; i < 2 && !isSplitWire; ++i)
      for (int j = 0; j < 2 && !isSplitWire; ++j)
        if (P1[i].Distance(P2[j]) < Max(Tol1[i], Tol2[j])) {
          aSplitPoint = P1[i];
          isSplitWire = true;
        }
  }

  BRepBuilderAPI_MakeWire* aWireBuilder = new BRepBuilderAPI_MakeWire;
  aWireBuilder->Add(aListOfEdges);
  if (aWireBuilder->Error() == BRepBuilderAPI_WireDone) {
    setImpl(aWireBuilder);
    setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

    // split the result wire
    TopoDS_Wire aWire = aWireBuilder->Wire();
    if (isSplitWire && BRep_Tool::IsClosed(aWire)) {
      TopoDS_Wire aNewWire;
      BRep_Builder aBuilder;
      aBuilder.MakeWire(aNewWire);
      for (TopExp_Explorer anExp(aWire, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        TopoDS_Edge aNewCurrent = TopoDS::Edge(anExp.Current());
        if (isSplitWire) {
          bool isToReshape = false;
          BRepTools_ReShape aReshape;
          TopoDS_Vertex aVF, aVL;
          TopExp::Vertices(aNewCurrent, aVF, aVL);
          gp_Pnt aPF = BRep_Tool::Pnt(aVF);
          double aTolF = BRep_Tool::Tolerance(aVF);
          gp_Pnt aPL = BRep_Tool::Pnt(aVL);
          double aTolL = BRep_Tool::Tolerance(aVL);
          if (aSplitPoint.SquareDistance(aPF) < aTolF * aTolF) {
            aReshape.Replace(aVF, aReshape.CopyVertex(aVF));
            isToReshape = true;
          }
          else if (aSplitPoint.SquareDistance(aPL) < aTolL * aTolL) {
            aReshape.Replace(aVL, aReshape.CopyVertex(aVL));
            isToReshape = true;
          }
          if (isToReshape) {
            aNewCurrent = TopoDS::Edge(aReshape.Apply(aNewCurrent));
            isSplitWire = false; // no need to continue splitting
          }
        }
        aBuilder.Add(aNewWire, aNewCurrent);
      }
      aWire = aNewWire;
    }

    // store generated/modified shapes
    for (TopTools_ListOfShape::Iterator aBaseIt(aListOfEdges); aBaseIt.More(); aBaseIt.Next()) {
      TopoDS_Edge aBaseCurrent = TopoDS::Edge(aBaseIt.Value());
      Standard_Real aFirst, aLast;
      Handle(Geom_Curve) aBaseCurve = BRep_Tool::Curve(aBaseCurrent, aFirst, aLast);

      for (TopExp_Explorer anExp(aWire, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        TopoDS_Edge aNewCurrent = TopoDS::Edge(anExp.Current());
        Handle(Geom_Curve) aNewCurve = BRep_Tool::Curve(aNewCurrent, aFirst, aLast);
        if (aBaseCurve == aNewCurve) {
          GeomShapePtr aBaseShape = fromTopoDS(aBaseCurrent);
          GeomShapePtr aNewShape = fromTopoDS(aNewCurrent);
          addGenerated(aBaseShape, aNewShape);
          addModified(aBaseShape, aNewShape);
        }
      }
    }

    setShape(fromTopoDS(aWire));
    setDone(true);
  }
}

//=================================================================================================
GeomShapePtr GeomAlgoAPI_WireBuilder::wire(const ListOfShape& theShapes)
{
  return GeomAlgoAPI_WireBuilder(theShapes).shape();
}

//=================================================================================================
bool GeomAlgoAPI_WireBuilder::isSelfIntersected(const GeomShapePtr& theWire)
{
  // Collect edges.
  ListOfShape anEdges;

  GeomAPI_ShapeExplorer anExp(theWire, GeomAPI_Shape::EDGE);
  for (; anExp.more(); anExp.next()) {
    GeomShapePtr anEdge = anExp.current();
    anEdges.push_back(anEdge);
  }

  // Check intersections between edges pair-wise
  std::list<GeomShapePtr>::const_iterator anEdgesIt = anEdges.begin();
  for (int i = 0; anEdgesIt != anEdges.end(); ++anEdgesIt, i++) {
    GeomEdgePtr anEdge1(new GeomAPI_Edge(*anEdgesIt));

    std::list<GeomShapePtr>::const_iterator anOtherEdgesIt = std::next(anEdgesIt);
    for (int j = i + 1; anOtherEdgesIt != anEdges.end(); ++anOtherEdgesIt, j++) {
      GeomEdgePtr anEdge2(new GeomAPI_Edge(*anOtherEdgesIt));
      GeomShapePtr anInter = anEdge1->intersect(anEdge2);
      if (!anInter.get()) {
        continue;
      }

      bool isOk = false;

      if (anInter->isVertex()) {
        GeomVertexPtr aVertex(new GeomAPI_Vertex(anInter));
        GeomPointPtr aPnt = aVertex->point();

        GeomPointPtr aFirstPnt1 = anEdge1->orientation() == GeomAPI_Shape::FORWARD ?
                                  anEdge1->firstPoint() : anEdge1->lastPoint();
        GeomPointPtr aLastPnt1 = anEdge1->orientation() == GeomAPI_Shape::FORWARD ?
                                 anEdge1->lastPoint() : anEdge1->firstPoint();
        GeomPointPtr aFirstPnt2 = anEdge2->orientation() == GeomAPI_Shape::FORWARD ?
                                  anEdge2->firstPoint() : anEdge2->lastPoint();
        GeomPointPtr aLastPnt2 = anEdge2->orientation() == GeomAPI_Shape::FORWARD ?
                                 anEdge2->lastPoint() : anEdge2->firstPoint();

        GeomPointPtr aCommonEndPnt;
        if (aFirstPnt1->isEqual(aLastPnt2)) {
          aCommonEndPnt = aFirstPnt1;
        } else if(aLastPnt1->isEqual(aFirstPnt2)) {
          aCommonEndPnt = aLastPnt1;
        }

        isOk = aCommonEndPnt && aPnt->isEqual(aCommonEndPnt);
      }

      if (!isOk) {
        return true;
      }
    }
  }

  return false;
}