// File:        GeomAlgoAPI_SketchBuilder.cpp
// Created:     02 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAPI_PlanarEdges.h>

#include <set>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Curve3D.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <Precision.hxx>

#include <assert.h>

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308 
#endif

const double tolerance = Precision::Confusion();
// This value helps to find direction on the boundaries of curve.
// It is not significant for lines, but is used for circles to avoid 
// wrong directions of movement (when two edges are tangent on the certain vertex)
const double shift = acos(1.0 - 3.0 * tolerance);

/// \brief Search first vertex - the vertex with lowest x coordinate, which is used in 2 edges at least
static const TopoDS_Vertex& findStartVertex(const BOPCol_IndexedDataMapOfShapeListOfShape& theMapVE,
                                           const gp_Dir& theDirX, const gp_Dir& theDirY);

/// \brief Search the vertex on the sketch candidate to be the next one in the loop
static void findNextVertex(const TopoDS_Vertex& theStartVertex,
                           const BOPCol_IndexedDataMapOfShapeListOfShape& theVertexEdgeMap,
                           const gp_Dir& theStartDir, const gp_Dir& theNormal,
                           TopoDS_Vertex& theNextVertex, TopoDS_Edge& theNextEdge,
                           gp_Dir& theNextDir);

/// \brief Create planar face using the edges surrounding it
static void createFace(const TopoDS_Vertex& theStartVertex,
                       const std::list<TopoDS_Edge>::iterator& theStartEdge,
                       const std::list<TopoDS_Edge>::iterator& theEndOfEdges,
                       const gp_Pln& thePlane, TopoDS_Face& theResFace);

/// \bief Create planar wire
static void createWireList(const TopoDS_Vertex& theStartVertex,
                           const std::list<TopoDS_Edge>::iterator& theStartEdge,
                           const std::list<TopoDS_Edge>::iterator& theEndOfEdges,
                           const std::set<TopoDS_Edge*>& theEdgesInLoops,
                           std::list<TopoDS_Wire>& theResWires);

/// \brief Calculate outer tengency on the edge in specified vertex
static gp_Dir getOuterEdgeDirection(const TopoDS_Edge& theEdge, const TopoDS_Vertex& theVertex);

/// \brief Unnecessary edges will be removed from the map.
///        Positions of iterator will be updated
static void removeWasteEdges(std::list<TopoDS_Vertex>::iterator& theStartVertex,
                             std::list<TopoDS_Edge>::iterator& theStartEdge,
                             const std::list<TopoDS_Vertex>::iterator& theEndOfVertexes,
                             const std::list<TopoDS_Edge>::iterator& theEndOfEdges,
                             BOPCol_IndexedDataMapOfShapeListOfShape& theMapVE);


void GeomAlgoAPI_SketchBuilder::createFaces(
    const std::shared_ptr<GeomAPI_Pnt>& theOrigin, const std::shared_ptr<GeomAPI_Dir>& theDirX,
    const std::shared_ptr<GeomAPI_Dir>& theDirY, const std::shared_ptr<GeomAPI_Dir>& theNorm,
    const std::list<std::shared_ptr<GeomAPI_Shape> >& theFeatures,
    std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces,
    std::list<std::shared_ptr<GeomAPI_Shape> >& theResultWires)
{
  if (theFeatures.empty())
    return;

  // Create the list of edges with shared vertexes
  BOPAlgo_Builder aBuilder;
  BOPAlgo_PaveFiller aPF;
  TopoDS_Shape aFeaturesCompound;

  // Obtain only edges from the features list
  std::list<std::shared_ptr<GeomAPI_Shape> > anEdges;
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aFeatIt = theFeatures.begin();
  for (; aFeatIt != theFeatures.end(); aFeatIt++) {
    std::shared_ptr<GeomAPI_Shape> aShape(*aFeatIt);
    const TopoDS_Edge& anEdge = aShape->impl<TopoDS_Edge>();
    if (anEdge.ShapeType() == TopAbs_EDGE)
      anEdges.push_back(aShape);
  }

  if (anEdges.size() == 1) {  // If there is only one feature, BOPAlgo_Builder will decline to work. Need to process it anyway
    aFeaturesCompound = anEdges.front()->impl<TopoDS_Shape>();
  } else {
    std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = anEdges.begin();
    for (; anIt != anEdges.end(); anIt++) {
      std::shared_ptr<GeomAPI_Shape> aPreview(*anIt);
      aBuilder.AddArgument(aPreview->impl<TopoDS_Edge>());
    }
    aPF.SetArguments(aBuilder.Arguments());
    aPF.Perform();
    int aErr = aPF.ErrorStatus();
    if (aErr)
      return;
    aBuilder.PerformWithFiller(aPF);
    aErr = aBuilder.ErrorStatus();
    if (aErr)
      return;
    aFeaturesCompound = aBuilder.Shape();
  }

  BOPCol_IndexedDataMapOfShapeListOfShape aMapVE;  // map between vertexes and edges
  BOPTools::MapShapesAndAncestors(aFeaturesCompound, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);
  if (aMapVE.IsEmpty())  // in case of not-initialized circle
    return;

  gp_Dir aDirX = theDirX->impl<gp_Dir>();
  gp_Dir aDirY = theDirY->impl<gp_Dir>();
  gp_Dir aNorm = theNorm->impl<gp_Dir>();

  gp_Pln aPlane(theOrigin->impl<gp_Pnt>(), aNorm);

  // Set of edges used in loops
  std::set<TopoDS_Edge*> anEdgesInLoops;
  // Lists for processed vertexes and edges
  std::list<TopoDS_Vertex> aProcVertexes;
  std::list<TopoDS_Edge> aProcEdges;

  // Search the start vertex
  TopoDS_Vertex aStartVertex = findStartVertex(aMapVE, aDirX, aDirY);
  aProcVertexes.push_back(aStartVertex);

  TopoDS_Vertex aCurVertex = aStartVertex;
  gp_Dir aCurDir = aDirY.Reversed();
  gp_Dir aCurNorm = aNorm.Reversed();

  // Go through the edges and find loops
  TopoDS_Vertex aNextVertex;
  TopoDS_Edge aBindingEdge;
  gp_Dir aNextDir;
  while (aMapVE.Extent() > 0) {
    if (aCurVertex.IsNull())
      return;
    if (!aProcEdges.empty())
      aBindingEdge = aProcEdges.back();
    findNextVertex(aCurVertex, aMapVE, aCurDir, aCurNorm, aNextVertex, aBindingEdge, aNextDir);
    aCurNorm = aNorm;

    // Try to find next vertex in the list of already processed
    bool isLoopFound = false;
    std::list<TopoDS_Vertex>::iterator aVertIter = aProcVertexes.begin();
    std::list<TopoDS_Edge>::iterator anEdgeIter = aProcEdges.begin();
    for (; aVertIter != aProcVertexes.end(); aVertIter++) {
      if (aVertIter->IsSame(aNextVertex)) {
        isLoopFound = true;
        break;
      }
      if (anEdgeIter != aProcEdges.end())
        anEdgeIter++;
    }

    bool isCircleFound = (isLoopFound && anEdgeIter == aProcEdges.end());
    aProcVertexes.push_back(aNextVertex);
    aProcEdges.push_back(aBindingEdge);

    if (isLoopFound) {
      // If the binding edge is a full circle, then it may be added recently. Need to update edge iterator
      if (isCircleFound) {
        anEdgeIter = aProcEdges.end();
        anEdgeIter--;
      }
      else if (aVertIter != aProcVertexes.begin()) {
        // Check the orientation of the loop
        gp_Dir aCN = getOuterEdgeDirection(*anEdgeIter, *aVertIter);
        gp_Dir aCP = getOuterEdgeDirection(aProcEdges.back(), *aVertIter);
        aCN.Reverse();
        aCP.Reverse();
        if (aCN.DotCross(aCP, aNorm) < -tolerance) {
          // The found loop has wrong orientation and may contain sub-loops.
          // Need to check it once again with another initial direction.
          aCurVertex = *aVertIter;
          do {
            aProcVertexes.pop_back();
            aProcEdges.pop_back();
          } while (aCurVertex != aProcVertexes.back());
          aCurDir = aCN.Reversed();
          aCurNorm = aNorm.Reversed();
          continue;
        }
      }

      if (!isCircleFound && anEdgeIter != aProcEdges.end() && 
          anEdgeIter->IsSame(aProcEdges.back())) { // The loop on the same edge taken twice
        aProcVertexes.pop_back();
        aProcEdges.pop_back();
        aCurVertex = aProcVertexes.back();
        aCurDir = getOuterEdgeDirection(aProcEdges.back(), aCurVertex);
        aCurNorm = aNorm.Reversed();
        continue;
      }

      // When the orientation is correct or the edges looped through
      // the first element, create new face and remove unnecessary edges.
      TopoDS_Face aPatch;
      createFace(*aVertIter, anEdgeIter, aProcEdges.end(), aPlane, aPatch);
      if (!aPatch.IsNull()) {
        std::shared_ptr<GeomAPI_Shape> aFace(new GeomAPI_Shape);
        aFace->setImpl(new TopoDS_Face(aPatch));
        theResultFaces.push_back(aFace);
      }
      // push the edges used in the loop to the map
      std::list<TopoDS_Edge>::iterator anIter;
      for (anIter = anEdgeIter; anIter != aProcEdges.end(); anIter++)
        anEdgesInLoops.insert(&(*anIter));
      // remove unnecessary edges
      std::list<TopoDS_Vertex>::iterator aCopyVLoop = aVertIter;
      std::list<TopoDS_Edge>::iterator aCopyELoop = anEdgeIter;
      removeWasteEdges(aVertIter, anEdgeIter, aProcVertexes.end(), aProcEdges.end(), aMapVE);

      // revert the list of remaining edges
      std::list<TopoDS_Vertex> aRemainVertexes;
      for (; aVertIter != aProcVertexes.end(); aVertIter++)
        aRemainVertexes.push_front(*aVertIter);
      std::list<TopoDS_Edge> aRemainEdges;
      for (; anEdgeIter != aProcEdges.end(); anEdgeIter++)
        aRemainEdges.push_front(*anEdgeIter);
      // remove edges and vertexes used in the loop and add remaining ones
      if (aCopyVLoop != aProcVertexes.begin()) {
        aVertIter = aCopyVLoop;
        aVertIter--;
      } else
        aVertIter = aProcVertexes.end();
      aProcVertexes.erase(aCopyVLoop, aProcVertexes.end());
      aProcVertexes.insert(aProcVertexes.end(), aRemainVertexes.begin(), aRemainVertexes.end());
      if (aCopyELoop != aProcEdges.begin()) {
        anEdgeIter = aCopyELoop;
        anEdgeIter--;
      } else
        anEdgeIter = aProcEdges.end();
      aProcEdges.erase(aCopyELoop, aProcEdges.end());
      aProcEdges.insert(aProcEdges.end(), aRemainEdges.begin(), aRemainEdges.end());

      if (aVertIter == aProcVertexes.end())
        aVertIter = aProcVertexes.begin();
      else
        aVertIter++;
      if (anEdgeIter == aProcEdges.end())
        anEdgeIter = aProcEdges.begin();
      else
        anEdgeIter++;
      aCopyVLoop = aVertIter;
      aCopyELoop = anEdgeIter;

      if (aVertIter != aProcVertexes.end() && 
          aMapVE.Contains(*aVertIter) && aMapVE.FindFromKey(*aVertIter).Extent() <= 1)
        removeWasteEdges(aVertIter, anEdgeIter, aProcVertexes.end(), aProcEdges.end(), aMapVE);
      if (aCopyVLoop != aVertIter)
        aProcVertexes.erase(aCopyVLoop, aVertIter);
      if (aCopyELoop != anEdgeIter)
        aProcEdges.erase(aCopyELoop, anEdgeIter);

      // Check whether the next vertex already exists
      if (aVertIter != aProcVertexes.end())
        aVertIter++;
      if (aVertIter != aProcVertexes.end() && anEdgeIter != aProcEdges.end()) {
        aNextVertex = *aVertIter;
        aNextDir = getOuterEdgeDirection(*anEdgeIter, aNextVertex);
        aProcVertexes.erase(++aVertIter, aProcVertexes.end());
        aProcEdges.erase(++anEdgeIter, aProcEdges.end());
      } else {
        // Recalculate current vertex and current direction
        aProcEdges.clear();
        aProcVertexes.clear();
        if (aMapVE.Extent() > 0) {
          aNextVertex = findStartVertex(aMapVE, aDirX, aDirY);
          aProcVertexes.push_back(aNextVertex);
        }
        aNextDir = aDirY.Reversed();
        aCurNorm = aNorm.Reversed();
      }
    }

    // if next vertex connected only to alone edge, this is a part of wire (not a closed loop),
    // we need to go back through the list of already checked edges to find a branching vertex
    if (!aMapVE.IsEmpty() && aMapVE.Contains(aNextVertex)
        && aMapVE.FindFromKey(aNextVertex).Size() == 1) {
      std::list<TopoDS_Vertex>::reverse_iterator aVRIter = aProcVertexes.rbegin();
      std::list<TopoDS_Edge>::reverse_iterator aERIter = aProcEdges.rbegin();
      if (aVRIter != aProcVertexes.rend())
        aVRIter++;
      if (aERIter != aProcEdges.rend())
        aERIter++;

      for (; aERIter != aProcEdges.rend(); aERIter++, aVRIter++)
        if (aMapVE.FindFromKey(*aVRIter).Size() > 2)
          break;
      if (aERIter != aProcEdges.rend()
          || (aVRIter != aProcVertexes.rend() && aMapVE.FindFromKey(*aVRIter).Size() == 1)) {  // the branching vertex was found or current list of edges is a wire without branches
        std::list<TopoDS_Edge>::iterator aEIter;
        TopoDS_Edge aCurEdge;
        if (aERIter != aProcEdges.rend()) {
          aEIter = aERIter.base();
          aCurEdge = *aERIter;
        } else
          aEIter = aProcEdges.begin();
        std::list<TopoDS_Wire> aTail;
        createWireList(*aVRIter, aEIter, aProcEdges.end(), anEdgesInLoops, aTail);
        std::list<TopoDS_Wire>::const_iterator aTailIter = aTail.begin();
        for (; aTailIter != aTail.end(); aTailIter++)
          if (!aTailIter->IsNull()) {
            std::shared_ptr<GeomAPI_Shape> aWire(new GeomAPI_Shape);
            aWire->setImpl(new TopoDS_Shape(*aTailIter));
            theResultWires.push_back(aWire);
          }
        std::list<TopoDS_Vertex>::iterator aVIter = aVRIter.base();
        std::list<TopoDS_Edge>::iterator aEItCopy = aEIter;
        removeWasteEdges(--aVIter, aEItCopy, aProcVertexes.end(), aProcEdges.end(), aMapVE);

        aProcEdges.erase(aEIter, aProcEdges.end());
        aVIter = aVRIter.base();
        aProcVertexes.erase(aVIter, aProcVertexes.end());

        if (!aProcVertexes.empty()) {
          aNextVertex = aProcVertexes.back();
          if (!aCurEdge.IsNull())
            aNextDir = getOuterEdgeDirection(aCurEdge, aNextVertex);
        }
      } else {  // there is no branching vertex in the list of proceeded,
                // so we should revert the list and go the opposite way
        aProcVertexes.reverse();
        aProcEdges.reverse();
        aNextVertex = aProcVertexes.back();
        aNextDir =
            aProcEdges.empty() ? aDirY : getOuterEdgeDirection(aProcEdges.back(), aNextVertex);
      }
    }

    // When all edges of current component of connectivity are processed,
    // we should repeat procedure for finding initial vertex in the remaining
    if (!aMapVE.IsEmpty() && !aMapVE.Contains(aNextVertex)) {
      aProcVertexes.clear();
      aProcEdges.clear();

      aStartVertex = findStartVertex(aMapVE, aDirX, aDirY);
      aProcVertexes.push_back(aStartVertex);
      aNextVertex = aStartVertex;
      aNextDir = aDirY.Reversed();
      aCurNorm = aNorm.Reversed();
    }

    aCurVertex = aNextVertex;
    aCurDir = aNextDir;
  }

  if (theResultFaces.size() > 1)
    fixIntersections(theResultFaces);
}

void GeomAlgoAPI_SketchBuilder::createFaces(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                            const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                            const std::shared_ptr<GeomAPI_Dir>& theDirY,
                                            const std::shared_ptr<GeomAPI_Dir>& theNorm,
                                            const std::shared_ptr<GeomAPI_Shape>& theWire,
                                            std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces)
{
  std::shared_ptr<GeomAPI_PlanarEdges> aWire = std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(theWire);
  if(!aWire)
    return;
  // Filter wires, return only faces.
  std::list<std::shared_ptr<GeomAPI_Shape> > aFilteredWires;
  createFaces(theOrigin, theDirX, theDirY, theNorm,
              aWire->getEdges(), theResultFaces, aFilteredWires);
}


void GeomAlgoAPI_SketchBuilder::fixIntersections(
    std::list<std::shared_ptr<GeomAPI_Shape> >& theFaces)
{
  BRepClass_FaceClassifier aClassifier;

  std::list<std::shared_ptr<GeomAPI_Shape> >::iterator anIter1 = theFaces.begin();
  std::list<std::shared_ptr<GeomAPI_Shape> >::iterator anIter2;
  for (; anIter1 != theFaces.end(); anIter1++) {
    anIter2 = anIter1;
    for (++anIter2; anIter2 != theFaces.end(); anIter2++) {
      const TopoDS_Face& aF1 = (*anIter1)->impl<TopoDS_Face>();
      assert(aF1.ShapeType() == TopAbs_FACE);  // all items in result list should be faces
      TopExp_Explorer aVert2((*anIter2)->impl<TopoDS_Shape>(), TopAbs_VERTEX);
      for (; aVert2.More(); aVert2.Next()) {
        const TopoDS_Vertex& aV = (const TopoDS_Vertex&)aVert2.Current();
        aClassifier.Perform(aF1, BRep_Tool::Pnt(aV), tolerance);
        TopAbs_State aState = aClassifier.State();
        if (aState != TopAbs_IN && aState != TopAbs_ON)
          break;
      }
      if (aVert2.More()) {  // second shape is not inside first, change the shapes order and repeat comparision
        const TopoDS_Face& aF2 = (*anIter2)->impl<TopoDS_Face>();
        assert(aF2.ShapeType() == TopAbs_FACE);  // all items in result list should be faces
        TopExp_Explorer aVert1((*anIter1)->impl<TopoDS_Shape>(), TopAbs_VERTEX);
        for (; aVert1.More(); aVert1.Next()) {
          const TopoDS_Vertex& aV = (const TopoDS_Vertex&)aVert2.Current();
          aClassifier.Perform(aF2, BRep_Tool::Pnt(aV), tolerance);
          TopAbs_State aState = aClassifier.State();
          if (aState != TopAbs_IN && aState != TopAbs_ON)
            break;
        }
        if (!aVert1.More()) {  // first shape should be cut from the second
          BRepAlgoAPI_Cut aCut((*anIter2)->impl<TopoDS_Shape>(), (*anIter1)->impl<TopoDS_Shape>());
          aCut.Build();
          TopExp_Explorer anExp(aCut.Shape(), TopAbs_FACE);
          bool isFirstFace = true;
          for (; anExp.More(); anExp.Next()) {
            if (anExp.Current().ShapeType() != TopAbs_FACE) continue;
            if (isFirstFace) {
              (*anIter2)->setImpl(new TopoDS_Shape(anExp.Current()));
              isFirstFace = false;
            } else {
              std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
              aShape->setImpl(new TopoDS_Shape(anExp.Current()));
              theFaces.push_back(aShape);
            }
          }
        }
      } else {  // second shape should be cut from the first
        BRepAlgoAPI_Cut aCut((*anIter1)->impl<TopoDS_Shape>(), (*anIter2)->impl<TopoDS_Shape>());
        aCut.Build();
        TopExp_Explorer anExp(aCut.Shape(), TopAbs_FACE);
        bool isFirstFace = true;
        for (; anExp.More(); anExp.Next()) {
          if (anExp.Current().ShapeType() != TopAbs_FACE) continue;
          if (isFirstFace) {
            (*anIter1)->setImpl(new TopoDS_Shape(anExp.Current()));
            isFirstFace = false;
          } else {
            std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
            aShape->setImpl(new TopoDS_Shape(anExp.Current()));
            theFaces.push_back(aShape);
          }
        }
      }
    }
  }
}

// =================== Auxiliary functions ====================================
const TopoDS_Vertex& findStartVertex(const BOPCol_IndexedDataMapOfShapeListOfShape& theMapVE,
                                    const gp_Dir& theDirX, const gp_Dir& theDirY)
{
  int aStartVertexInd = 1;
  double aMaxX = -DBL_MAX;
  double aMaxY = -DBL_MAX;
  int aNbVert = theMapVE.Extent();
  for (int i = 1; i <= aNbVert; i++) {
    const TopoDS_Vertex& aV = (const TopoDS_Vertex&) theMapVE.FindKey(i);
    const gp_Pnt& aVertPnt = BRep_Tool::Pnt(aV);

    double aX = aVertPnt.XYZ().Dot(theDirX.XYZ());
    double aY = aVertPnt.XYZ().Dot(theDirY.XYZ());
    if ((aX > aMaxX || (fabs(aX - aMaxX) < tolerance && aY > aMaxY))
        && theMapVE.FindFromIndex(i).Extent() > 1) {
      aMaxX = aX;
      aMaxY = aY;
      aStartVertexInd = i;
    }
  }
  return static_cast<const TopoDS_Vertex&>(theMapVE.FindKey(aStartVertexInd));
}

void findNextVertex(const TopoDS_Vertex& theStartVertex,
                    const BOPCol_IndexedDataMapOfShapeListOfShape& theVertexEdgeMap,
                    const gp_Dir& theStartDir, const gp_Dir& theNormal, TopoDS_Vertex& theNextVertex,
                    TopoDS_Edge& theNextEdge, gp_Dir& theNextDir)
{
  theNextVertex = TopoDS_Vertex();
  const BOPCol_ListOfShape& anEdgesList = theVertexEdgeMap.FindFromKey(theStartVertex);
  int anEdgesNum = anEdgesList.Extent();
  BOPCol_ListOfShape::Iterator aEdIter(anEdgesList);
  double aBestEdgeProj = DBL_MAX;
  for (; aEdIter.More(); aEdIter.Next()) {
    const TopoDS_Edge& anEdge = static_cast<const TopoDS_Edge&>(aEdIter.Value());
    gp_Dir aTang = getOuterEdgeDirection(anEdge, theStartVertex);
    aTang.Reverse();

    // The projection is normalized in segment (-1, 1),
    // where (-1, 0] corresponds to the angles (pi/2, 0] between theStartDir and aTang
    // and [0, 1) corresponds to the angles [0, -pi/2)
    double aProj = (aTang.Dot(theStartDir) - 1.0) * 0.5;
    if (anEdgesNum > 1 && fabs(fabs(aProj) - 1) < tolerance)
      continue;
    if (theStartDir.DotCross(aTang, theNormal) < tolerance)
      aProj *= -1.0;

    if (aProj < aBestEdgeProj) {
      aBestEdgeProj = aProj;
      theNextEdge = anEdge;
      TopExp_Explorer aVertExp(theNextEdge, TopAbs_VERTEX);
      for (; aVertExp.More(); aVertExp.Next())
        if (!aVertExp.Current().IsSame(theStartVertex)) {
          theNextVertex = static_cast<const TopoDS_Vertex&>(aVertExp.Current());
          theNextDir = getOuterEdgeDirection(anEdge, theNextVertex);
          break;
        }
      if (!aVertExp.More()) {  // This edge is a full circle
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(theNextEdge, aV1, aV2);
        if (aV1.Orientation() == theStartVertex.Orientation())
          theNextVertex = aV2;
        else
          theNextVertex = aV1;
        theNextDir = getOuterEdgeDirection(anEdge, theNextVertex);
      }
    }
  }

  // Probably there are two tangent edges. We will take the edge differs from current one
  if (theNextVertex.IsNull() && anEdgesNum == 2) {
    BOPCol_ListOfShape::Iterator aEdIter(anEdgesList);
    if (aEdIter.Value() == theNextEdge)
      aEdIter.Next();
    theNextEdge = static_cast<const TopoDS_Edge&>(aEdIter.Value());
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(theNextEdge, aV1, aV2);
    theNextVertex = theStartVertex.IsSame(aV1) ? aV2 : aV1;
    theNextDir = getOuterEdgeDirection(theNextEdge, theNextVertex);
  }
}

static void addEdgeToWire(const TopoDS_Edge& theEdge, const BRep_Builder& theBuilder,
                          TopoDS_Shape& theSpliceVertex, TopoDS_Wire& theWire)
{
  TopoDS_Edge anEdge = theEdge;
  bool isCurVertChanged = false;
  TopoDS_Shape aCurVertChanged;

  TopExp_Explorer aVertExp(theEdge, TopAbs_VERTEX);
  for (; aVertExp.More(); aVertExp.Next()) {
    const TopoDS_Shape& aVertex = aVertExp.Current();
    if (aVertex.IsSame(theSpliceVertex)
        && aVertex.Orientation() != theEdge.Orientation()) {  // Current vertex is the last for the edge, so its orientation is wrong, need to revert the edge
      anEdge.Reverse();
      break;
    }
    if (!aVertex.IsSame(theSpliceVertex)) {
      aCurVertChanged = aVertex;
      isCurVertChanged = true;
    }
  }
  theSpliceVertex = isCurVertChanged ? aCurVertChanged : aVertExp.Current();

  theBuilder.Add(theWire, anEdge);
}

void createFace(const TopoDS_Vertex& theStartVertex,
                const std::list<TopoDS_Edge>::iterator& theStartEdge,
                const std::list<TopoDS_Edge>::iterator& theEndOfEdges,
                const gp_Pln& thePlane,
                TopoDS_Face& theResFace)
{
  TopoDS_Wire aResWire;
  BRep_Builder aBuilder;
  aBuilder.MakeWire(aResWire);

  TopoDS_Vertex aCurVertex = theStartVertex;
  std::list<TopoDS_Edge>::const_iterator anEdgeIter = theStartEdge;
  for (; anEdgeIter != theEndOfEdges; anEdgeIter++) {
    if (!anEdgeIter->IsNull())
      addEdgeToWire(*anEdgeIter, aBuilder, aCurVertex, aResWire);
  }

  BRepBuilderAPI_MakeFace aFaceBuilder(thePlane, aResWire);
  if (aFaceBuilder.Error() == BRepBuilderAPI_FaceDone)
    theResFace = aFaceBuilder.Face();
}

void createWireList(const TopoDS_Vertex& theStartVertex,
                    const std::list<TopoDS_Edge>::iterator& theStartEdge,
                    const std::list<TopoDS_Edge>::iterator& theEndOfEdges,
                    const std::set<TopoDS_Edge*>& theEdgesInLoops,
                    std::list<TopoDS_Wire>& theResWires)
{
  BRep_Builder aBuilder;
  bool needNewWire = true;
  TopoDS_Vertex aCurVertex = theStartVertex;

  std::list<TopoDS_Edge>::iterator anIter = theStartEdge;
  while (anIter != theEndOfEdges) {
    while (anIter != theEndOfEdges && needNewWire && theEdgesInLoops.count(&(*anIter)) != 0) {
      TopExp_Explorer aVertExp(*anIter, TopAbs_VERTEX);
      for (; aVertExp.More(); aVertExp.Next())
        if (!aVertExp.Current().IsSame(aCurVertex)) {
          aCurVertex = static_cast<const TopoDS_Vertex&>(aVertExp.Current());
          break;
        }
      anIter++;
    }
    if (anIter == theEndOfEdges)
      break;

    if (needNewWire) {  // The new wire should be created
      TopoDS_Wire aWire;
      aBuilder.MakeWire(aWire);
      theResWires.push_back(aWire);
      needNewWire = false;
    } else if (theEdgesInLoops.count(&(*anIter)) != 0) {  // There was found the edge already used in loop.
                                                          // Current wire should be released and new one should started
      needNewWire = true;
      continue;
    }

    addEdgeToWire(*anIter, aBuilder, aCurVertex, theResWires.back());
    anIter++;
  }
}

gp_Dir getOuterEdgeDirection(const TopoDS_Edge& theEdge, const TopoDS_Vertex& theVertex)
{
  gp_Pnt aVertexPnt = BRep_Tool::Pnt(theVertex);

  // Convert the edge to the curve to calculate the tangency.
  // There should be only one curve in the edge.
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, aFirst, aLast);

  gp_Pnt aPnt;
  gp_Vec aTang;
  // A direction is determined not in the boundary points but in the points with small shift.
  // It was done to avoid tangency between circle and other edge in the shared vertex.
  aCurve->D1(aFirst + shift > aLast ? aFirst : aFirst + shift, aPnt, aTang);
  aCurve->D0(aFirst, aPnt);
  if (aVertexPnt.IsEqual(aPnt, tolerance))
    return gp_Dir(aTang.Reversed());

  aCurve->D1(aLast - shift < aFirst ? aLast : aLast - shift, aPnt, aTang);
  return gp_Dir(aTang);
}

void removeWasteEdges(std::list<TopoDS_Vertex>::iterator& theStartVertex,
                      std::list<TopoDS_Edge>::iterator& theStartEdge,
                      const std::list<TopoDS_Vertex>::iterator& theEndOfVertexes,
                      const std::list<TopoDS_Edge>::iterator& theEndOfEdges,
                      BOPCol_IndexedDataMapOfShapeListOfShape& theMapVE)
{
  bool isVertStep = true;
  while (theStartVertex != theEndOfVertexes && theStartEdge != theEndOfEdges) {
    BOPCol_ListOfShape& aBunch = theMapVE.ChangeFromKey(*theStartVertex);
    BOPCol_ListOfShape::Iterator anApprEdge(aBunch);
    for (; anApprEdge.More(); anApprEdge.Next())
      if (anApprEdge.Value().IsSame(*theStartEdge))
        break;
    if (anApprEdge.More())
      aBunch.Remove(anApprEdge);

    if (isVertStep)
      theStartVertex++;
    else {
      theStartEdge++;
      // check current vertex to be a branching point
      // if so, it will be a new starting point to find a loop
      if (aBunch.Size() > 1)
        break;
    }
    isVertStep = !isVertStep;
  }

  // The map of vertex-edges may be changed
  BOPCol_IndexedDataMapOfShapeListOfShape aMapVECopy;
  BOPCol_IndexedDataMapOfShapeListOfShape::Iterator aMapIter(theMapVE);
  for (int ind = 1; aMapIter.More(); aMapIter.Next(), ind++)
    if (!aMapIter.Value().IsEmpty())
      aMapVECopy.Add(theMapVE.FindKey(ind), aMapIter.Value());
  theMapVE.Clear();
  theMapVE.Exchange(aMapVECopy);
}

