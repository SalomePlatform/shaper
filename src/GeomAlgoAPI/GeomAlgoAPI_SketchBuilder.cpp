// File:        GeomAlgoAPI_SketchBuilder.cpp
// Created:     02 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_SketchBuilder.h>

#include <set>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Curve3D.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TVertex.hxx>
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
const double tolerance = Precision::Confusion();
// This value helps to find direction on the boundaries of curve.
// It is not significant for lines, but is used for circles to avoid 
// wrong directions of movement (when two edges are tangent on the certain vertex)
const double shift     = acos(1.0 - 2.0 * tolerance);

/// \brief Search first vertex - the vertex with lowest x coordinate, which is used in 2 edges at least
static const TopoDS_Shape& findStartVertex(
          const BOPCol_IndexedDataMapOfShapeListOfShape& theMapVE,
          const gp_Dir& theDirX, const gp_Dir& theDirY);

/// \brief Search the vertex on the sketch candidate to be the next one in the loop
static void findNextVertex(
          const TopoDS_Shape&                            theStartVertex, 
          const BOPCol_IndexedDataMapOfShapeListOfShape& theVertexEdgeMap,
          const gp_Dir&                                  theStartDir,
          const gp_Dir&                                  theNormal,
                TopoDS_Shape&                            theNextVertex,
                TopoDS_Shape&                            theNextEdge,
                gp_Dir&                                  theNextDir);

/// \brief Create planar face using the edges surrounding it
static void createFace(const TopoDS_Shape&                      theStartVertex,
                       const std::list<TopoDS_Shape>::iterator& theStartEdge,
                       const std::list<TopoDS_Shape>::iterator& theEndOfEdges,
                       const gp_Pln&                            thePlane,
                             TopoDS_Face&                       theResFace);

/// \bief Create planar wire
static void createWireList(const TopoDS_Shape&                      theStartVertex,
                           const std::list<TopoDS_Shape>::iterator& theStartEdge,
                           const std::list<TopoDS_Shape>::iterator& theEndOfEdges,
                           const std::set<Handle(TopoDS_TShape)>&   theEdgesInLoops,
                                 std::list<TopoDS_Wire>&            theResWires);

/// \brief Calculate outer tengency on the edge in specified vertex
static gp_Dir getOuterEdgeDirection(const TopoDS_Shape& theEdge,
                                    const TopoDS_Shape& theVertex);

/// \brief Unnecessary edges will be removed from the map.
///        Positions of iterator will be updated
static void removeWasteEdges(
                std::list<TopoDS_Shape>::iterator&  theStartVertex,
                std::list<TopoDS_Shape>::iterator&  theStartEdge,
          const std::list<TopoDS_Shape>::iterator&  theEndOfVertexes,
          const std::list<TopoDS_Shape>::iterator&  theEndOfEdges,
          BOPCol_IndexedDataMapOfShapeListOfShape&  theMapVE);




void GeomAlgoAPI_SketchBuilder::createFaces(
          const boost::shared_ptr<GeomAPI_Pnt>&                theOrigin,
          const boost::shared_ptr<GeomAPI_Dir>&                theDirX,
          const boost::shared_ptr<GeomAPI_Dir>&                theDirY,
          const boost::shared_ptr<GeomAPI_Dir>&                theNorm,
          const std::list< boost::shared_ptr<GeomAPI_Shape> >& theFeatures,
                std::list< boost::shared_ptr<GeomAPI_Shape> >& theResultFaces,
                std::list< boost::shared_ptr<GeomAPI_Shape> >& theResultWires)
{
  if (theFeatures.empty())
    return ;

  // Create the list of edges with shared vertexes
  BOPAlgo_Builder aBuilder;
  BOPAlgo_PaveFiller aPF;
  TopoDS_Shape aFeaturesCompound;

  if (theFeatures.size() == 1)
  { // If there is only one feature, BOPAlgo_Builder will decline to work. Need to process it anyway
    aFeaturesCompound = theFeatures.front()->impl<TopoDS_Shape>();
  }
  else
  {
    std::list< boost::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = theFeatures.begin();
    for (; anIt != theFeatures.end(); anIt++)
    {
      boost::shared_ptr<GeomAPI_Shape> aPreview(*anIt);
      aBuilder.AddArgument(aPreview->impl<TopoDS_Edge>());
    }
    aPF.SetArguments(aBuilder.Arguments());
    aPF.Perform();
    int aErr = aPF.ErrorStatus();
    if (aErr) return ;
    aBuilder.PerformWithFiller(aPF);
    aErr = aBuilder.ErrorStatus();
    if (aErr) return ;
    aFeaturesCompound = aBuilder.Shape();
  }

  BOPCol_IndexedDataMapOfShapeListOfShape aMapVE; // map between vertexes and edges
  BOPTools::MapShapesAndAncestors(aFeaturesCompound, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);

  gp_Dir aDirX = theDirX->impl<gp_Dir>();
  gp_Dir aDirY = theDirY->impl<gp_Dir>();
  gp_Dir aNorm = theNorm->impl<gp_Dir>();

  gp_Pln aPlane(theOrigin->impl<gp_Pnt>(), aNorm);

  // Set of edges used in loops
  std::set<Handle(TopoDS_TShape)> anEdgesInLoops;
  // Lists for processed vertexes and edges
  std::list<TopoDS_Shape> aProcVertexes;
  std::list<TopoDS_Shape> aProcEdges;

  // Search the start vertex
  TopoDS_Shape aStartVertex = findStartVertex(aMapVE, aDirX, aDirY);
  aProcVertexes.push_back(aStartVertex);

  TopoDS_Shape aCurVertex = aStartVertex;
  gp_Dir aCurDir = aDirY.Reversed();
  gp_Dir aCurNorm = aNorm.Reversed();

  // Go through the edges and find loops
  TopoDS_Shape aNextVertex;
  TopoDS_Shape aBindingEdge;
  gp_Dir aNextDir;
  while (aMapVE.Extent() > 0)
  {
    findNextVertex(aCurVertex, aMapVE, aCurDir, aCurNorm, aNextVertex, aBindingEdge, aNextDir);
    aCurNorm = aNorm;

    // Try to find next vertex in the list of already processed
    std::list<TopoDS_Shape>::iterator          aVertIter = aProcVertexes.begin();
    std::list<TopoDS_Shape>::iterator          anEdgeIter = aProcEdges.begin();
    for ( ; aVertIter != aProcVertexes.end(); aVertIter++)
    {
      if (aVertIter->TShape() == aNextVertex.TShape())
        break;
      if (anEdgeIter != aProcEdges.end())
        anEdgeIter++;
    }

    aProcVertexes.push_back(aNextVertex);
    aProcEdges.push_back(aBindingEdge);

    // The loop was found
    if (aVertIter != aProcVertexes.end())
    {
      // If the binding edge is a full circle, then the list may be empty before addition. Need to update edge iterator
      if (aProcEdges.size() == 1)
        anEdgeIter = aProcEdges.begin();

      if (aVertIter != aProcVertexes.begin())
      {
        // Check the orientation of the loop
        gp_Dir aCN = getOuterEdgeDirection(*anEdgeIter, *aVertIter);
        gp_Dir aCP = getOuterEdgeDirection(aProcEdges.back(), *aVertIter);
        aCN.Reverse();
        aCP.Reverse();
        if (aCN.DotCross(aCP, aNorm) < -tolerance)
        {
          // The found loop has wrong orientation and may contain sub-loops.
          // Need to check it onle again with another initial direction.
          aCurVertex = *aVertIter;
          do {
            aProcVertexes.pop_back();
            aProcEdges.pop_back();
          } while (aCurVertex != aProcVertexes.back());
          aCurDir = getOuterEdgeDirection(aProcEdges.back(), aCurVertex);
          aCurNorm = aNorm.Reversed();
          continue;
        }
      }

      // When the orientation is correct or the edges looped through
      // the first element, create new face and remove unnecessary edges.
      TopoDS_Face aPatch;
      createFace(*aVertIter, anEdgeIter, aProcEdges.end(), aPlane, aPatch);
      if (!aPatch.IsNull())
      {
        boost::shared_ptr<GeomAPI_Shape> aFace(new GeomAPI_Shape);
        aFace->setImpl(new TopoDS_Shape(aPatch));
        theResultFaces.push_back(aFace);
      }
      // push the edges used in the loop to the map
      std::list<TopoDS_Shape>::iterator anIter;
      for (anIter = anEdgeIter; anIter != aProcEdges.end(); anIter++)
        anEdgesInLoops.insert(anIter->TShape());
      // remove unnecessary edges
      std::list<TopoDS_Shape>::iterator aCopyVLoop = aVertIter;
      std::list<TopoDS_Shape>::iterator aCopyELoop = anEdgeIter;
      removeWasteEdges(aVertIter, anEdgeIter, aProcVertexes.end(), aProcEdges.end(), aMapVE);

      // revert the list of remaining edges
      std::list<TopoDS_Shape> aRemainVertexes;
      for ( ; aVertIter != aProcVertexes.end(); aVertIter++)
        aRemainVertexes.push_front(*aVertIter);
      std::list<TopoDS_Shape> aRemainEdges;
      for ( ; anEdgeIter != aProcEdges.end(); anEdgeIter++)
        aRemainEdges.push_front(*anEdgeIter);
      // remove edges and vertexes used in the loop and add remaining ones
      aProcVertexes.erase(aCopyVLoop, aProcVertexes.end());
      aProcVertexes.insert(aProcVertexes.end(), aRemainVertexes.begin(), aRemainVertexes.end());
      aProcEdges.erase(aCopyELoop, aProcEdges.end());
      aProcEdges.insert(aProcEdges.end(), aRemainEdges.begin(), aRemainEdges.end());

      // Recalculate current vertex and current direction
      if (!aProcVertexes.empty())
      {
        aNextVertex = aProcVertexes.back();
        if (!aProcEdges.empty())
          aNextDir = getOuterEdgeDirection(aProcEdges.back(), aNextVertex);
        else aNextDir = aDirY;
      }
    }

    // if next vertex connected only to alone edge, this is a part of wire (not a closed loop),
    // we need to go back through the list of already checked edges to find a branching vertex
    if (!aMapVE.IsEmpty() && aMapVE.Contains(aNextVertex) && 
        aMapVE.FindFromKey(aNextVertex).Size() == 1)
    {
      std::list<TopoDS_Shape>::reverse_iterator aVRIter = aProcVertexes.rbegin();
      std::list<TopoDS_Shape>::reverse_iterator aERIter = aProcEdges.rbegin();
      for (++aERIter, ++aVRIter; aERIter != aProcEdges.rend(); aERIter++, aVRIter++)
        if (aMapVE.FindFromKey(*aVRIter).Size() > 2)
          break;
      if (aERIter != aProcEdges.rend() || aMapVE.FindFromKey(*aVRIter).Size() == 1)
      { // the branching vertex was found or current list of edges is a wire without branches
        std::list<TopoDS_Shape>::iterator aEIter;
        TopoDS_Shape aCurEdge;
        if (aERIter != aProcEdges.rend())
        {
          aEIter = aERIter.base();
          aCurEdge = *aERIter;
        }
        else
          aEIter = aProcEdges.begin();
        std::list<TopoDS_Wire> aTail;
        createWireList(*aVRIter, aEIter, aProcEdges.end(), anEdgesInLoops, aTail);
        std::list<TopoDS_Wire>::const_iterator aTailIter = aTail.begin();
        for ( ; aTailIter != aTail.end(); aTailIter++)
          if (!aTailIter->IsNull())
          {
            boost::shared_ptr<GeomAPI_Shape> aWire(new GeomAPI_Shape);
            aWire->setImpl(new TopoDS_Shape(*aTailIter));
            theResultWires.push_back(aWire);
          }
        std::list<TopoDS_Shape>::iterator aVIter = aVRIter.base();
        std::list<TopoDS_Shape>::iterator aEItCopy = aEIter;
        removeWasteEdges(--aVIter, aEItCopy, aProcVertexes.end(), aProcEdges.end(), aMapVE);

        aProcEdges.erase(aEIter, aProcEdges.end());
        aVIter = aVRIter.base();
        aProcVertexes.erase(aVIter, aProcVertexes.end());

        if (!aProcVertexes.empty())
        {
          aNextVertex = aProcVertexes.back();
          if (!aCurEdge.IsNull())
            aNextDir = getOuterEdgeDirection(aCurEdge, aNextVertex);
        }
      }
      else
      { // there is no branching vertex in the list of proceeded,
        // so we should revert the list and go the opposite way
        aProcVertexes.reverse();
        aProcEdges.reverse();
        aNextVertex = aProcVertexes.back();
        aNextDir = getOuterEdgeDirection(aProcEdges.back(), aNextVertex);
      }
    }

    // When all edges of current component of connectivity are processed,
    // we should repeat procedure for finding initial vertex in the remaining
    if (!aMapVE.IsEmpty() && !aMapVE.Contains(aNextVertex))
    {
      aProcVertexes.clear();
      aProcEdges.clear();

      TopoDS_Shape aStartEdge;
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

void GeomAlgoAPI_SketchBuilder::fixIntersections(
          std::list< boost::shared_ptr<GeomAPI_Shape> >& theFaces)
{
  BRepClass_FaceClassifier aClassifier;

  std::list< boost::shared_ptr<GeomAPI_Shape> >::iterator anIter1 = theFaces.begin();
  std::list< boost::shared_ptr<GeomAPI_Shape> >::iterator anIter2;
  for ( ; anIter1 != theFaces.end(); anIter1++)
  {
    anIter2 = anIter1;
    for (++anIter2; anIter2 != theFaces.end(); anIter2++)
    {
      const TopoDS_Face& aF1 = (*anIter1)->impl<TopoDS_Face>();
      TopExp_Explorer aVert2((*anIter2)->impl<TopoDS_Shape>(), TopAbs_VERTEX);
      for ( ; aVert2.More(); aVert2.Next())
      {
        Handle(BRep_TVertex) aV = Handle(BRep_TVertex)::DownCast(aVert2.Current().TShape());
        aClassifier.Perform(aF1, aV->Pnt(), tolerance);
        if (aClassifier.State() != TopAbs_IN)
          break;
      }
      if (aVert2.More())
      { // second shape is not inside first, change the shapes order and repeat comparision
        const TopoDS_Face& aF2 = (*anIter2)->impl<TopoDS_Face>();
        TopExp_Explorer aVert1((*anIter1)->impl<TopoDS_Shape>(), TopAbs_VERTEX);
        for ( ; aVert1.More(); aVert1.Next())
        {
          Handle(BRep_TVertex) aV = Handle(BRep_TVertex)::DownCast(aVert1.Current().TShape());
          aClassifier.Perform(aF2, aV->Pnt(), tolerance);
          if (aClassifier.State() != TopAbs_IN)
            break;
        }
        if (!aVert1.More())
        { // first shape should be cut from the second
          BRepAlgoAPI_Cut aCut((*anIter2)->impl<TopoDS_Shape>(),
                               (*anIter1)->impl<TopoDS_Shape>());
          (*anIter2)->setImpl(new TopoDS_Shape(aCut.Shape()));
        }
      }
      else
      { // second shape should be cut from the first
        BRepAlgoAPI_Cut aCut((*anIter1)->impl<TopoDS_Shape>(),
                             (*anIter2)->impl<TopoDS_Shape>());
        (*anIter1)->setImpl(new TopoDS_Shape(aCut.Shape()));
      }
    }
  }
}


// =================== Auxiliary functions ====================================
const TopoDS_Shape& findStartVertex(
          const BOPCol_IndexedDataMapOfShapeListOfShape& theMapVE,
          const gp_Dir& theDirX, const gp_Dir& theDirY)
{
  int aStartVertexInd = 1;
  double aMaxX = -DBL_MAX;
  double aMaxY = -DBL_MAX;
  int aNbVert = theMapVE.Extent();
  for (int i = 1; i <= aNbVert; i++)
  {
    const TopoDS_Vertex& aV = (const TopoDS_Vertex&)theMapVE.FindKey(i);
    const Handle(BRep_TVertex)& aVert = (const Handle(BRep_TVertex)&)aV.TShape();
    const gp_Pnt& aVertPnt = aVert->Pnt();

    double aX = aVertPnt.XYZ().Dot(theDirX.XYZ());
    double aY = aVertPnt.XYZ().Dot(theDirY.XYZ());
    if ((aX > aMaxX || (fabs(aX - aMaxX) < tolerance && aY > aMaxY)) && 
        theMapVE.FindFromIndex(i).Extent() > 1)
    {
      aMaxX = aX;
      aMaxY = aY;
      aStartVertexInd = i;
    }
  }
  return theMapVE.FindKey(aStartVertexInd);
}

void findNextVertex(
          const TopoDS_Shape&                            theStartVertex,
          const BOPCol_IndexedDataMapOfShapeListOfShape& theVertexEdgeMap,
          const gp_Dir&                                  theStartDir,
          const gp_Dir&                                  theNormal,
                TopoDS_Shape&                            theNextVertex,
                TopoDS_Shape&                            theNextEdge,
                gp_Dir&                                  theNextDir)
{
  const BOPCol_ListOfShape& anEdgesList = theVertexEdgeMap.FindFromKey(theStartVertex);
  BOPCol_ListOfShape::Iterator aEdIter(anEdgesList);
  double aBestEdgeProj = DBL_MAX;
  for ( ; aEdIter.More(); aEdIter.Next())
  {
    gp_Dir aTang = getOuterEdgeDirection(aEdIter.Value(), theStartVertex);
    aTang.Reverse();

    // The projection is normalized in segment (-1, 1),
    // where (-1, 0] corresponds to the angles (pi/2, 0] between theStartDir and aTang
    // and [0, 1) corresponds to the angles [0, -pi/2)
    double aProj = (aTang.Dot(theStartDir) - 1.0) * 0.5;
    if (fabs(fabs(aProj) - 1) < tolerance)
      continue;
    if (theStartDir.DotCross(aTang, theNormal) < tolerance)
      aProj *= -1.0;

    if (aProj < aBestEdgeProj)
    {
      aBestEdgeProj = aProj;
      theNextEdge = aEdIter.Value();
      TopExp_Explorer aVertExp(theNextEdge, TopAbs_VERTEX);
      for ( ; aVertExp.More(); aVertExp.Next())
        if (aVertExp.Current().TShape() != theStartVertex.TShape())
        {
          theNextVertex = aVertExp.Current();
          theNextDir = getOuterEdgeDirection(aEdIter.Value(), theNextVertex);
          break;
        }
      if (!aVertExp.More())
      { // This edge is a full circle
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(*(const TopoDS_Edge*)(&theNextEdge), aV1, aV2);
        if (aV1.Orientation() == theStartVertex.Orientation())
          theNextVertex = aV2;
        else
          theNextVertex = aV1;
        theNextDir = getOuterEdgeDirection(aEdIter.Value(), theNextVertex);
      }
    }
  }
}

static void addEdgeToWire(const TopoDS_Edge&  theEdge,
                          const BRep_Builder& theBuilder,
                                TopoDS_Shape& theSpliceVertex,
                                TopoDS_Wire&  theWire)
{
  TopoDS_Edge anEdge = theEdge;
  bool isCurVertChanged = false;
  TopoDS_Shape aCurVertChanged;

  TopExp_Explorer aVertExp(theEdge, TopAbs_VERTEX);
  for ( ; aVertExp.More(); aVertExp.Next())
  {
    const TopoDS_Shape& aVertex = aVertExp.Current();
    if (aVertex.TShape() == theSpliceVertex.TShape() && 
        aVertex.Orientation() != theEdge.Orientation())
    { // Current vertex is the last for the edge, so its orientation is wrong, need to revert the edge
      anEdge.Reverse();
      break;
    }
    if (aVertex.TShape() != theSpliceVertex.TShape())
    {
      aCurVertChanged = aVertex;
      isCurVertChanged = true;
    }
  }
  theSpliceVertex = isCurVertChanged ? aCurVertChanged : aVertExp.Current();

  theBuilder.Add(theWire, anEdge);
}

void createFace(const TopoDS_Shape&                      theStartVertex,
                const std::list<TopoDS_Shape>::iterator& theStartEdge,
                const std::list<TopoDS_Shape>::iterator& theEndOfEdges,
                const gp_Pln&                            thePlane,
                      TopoDS_Face&                       theResFace)
{
  TopoDS_Wire aResWire;
  BRep_Builder aBuilder;
  aBuilder.MakeWire(aResWire);

  TopoDS_Shape aCurVertex = theStartVertex;
  std::list<TopoDS_Shape>::const_iterator anEdgeIter = theStartEdge;
  for ( ; anEdgeIter != theEndOfEdges; anEdgeIter++)
  {
    TopoDS_Edge anEdge = *((TopoDS_Edge*)(&(*anEdgeIter)));
    addEdgeToWire(anEdge, aBuilder, aCurVertex, aResWire);
  }

  BRepBuilderAPI_MakeFace aFaceBuilder(thePlane, aResWire);
  if (aFaceBuilder.Error() == BRepBuilderAPI_FaceDone)
    theResFace = aFaceBuilder.Face();
}

void createWireList(const TopoDS_Shape&                      theStartVertex,
                    const std::list<TopoDS_Shape>::iterator& theStartEdge,
                    const std::list<TopoDS_Shape>::iterator& theEndOfEdges,
                    const std::set<Handle(TopoDS_TShape)>&   theEdgesInLoops,
                          std::list<TopoDS_Wire>&            theResWires)
{
  BRep_Builder aBuilder;
  bool needNewWire = true;
  TopoDS_Shape aCurVertex = theStartVertex;

  std::list<TopoDS_Shape>::iterator anIter = theStartEdge;
  while (anIter != theEndOfEdges)
  {
    while (anIter != theEndOfEdges && needNewWire && 
           theEdgesInLoops.count(anIter->TShape()) != 0)
    {
      TopExp_Explorer aVertExp(*anIter, TopAbs_VERTEX);
      for ( ; aVertExp.More(); aVertExp.Next())
        if (aVertExp.Current().TShape() != aCurVertex.TShape())
        {
          aCurVertex = aVertExp.Current();
          break;
        }
      anIter++;
    }
    if (anIter == theEndOfEdges)
      break;

    if (needNewWire)
    { // The new wire should be created
      TopoDS_Wire aWire;
      aBuilder.MakeWire(aWire);
      theResWires.push_back(aWire);
      needNewWire = false;
    }
    else if (theEdgesInLoops.count(anIter->TShape()) != 0)
    { // There was found the edge already used in loop.
      // Current wire should be released and new one should started
      needNewWire = true;
      continue;
    }

    TopoDS_Edge anEdge = *((TopoDS_Edge*)(&(*anIter)));
    addEdgeToWire(anEdge, aBuilder, aCurVertex, theResWires.back());
    anIter++;
  }
}


gp_Dir getOuterEdgeDirection(const TopoDS_Shape& theEdge,
                             const TopoDS_Shape& theVertex)
{
  const Handle(BRep_TVertex)& aVertex = (const Handle(BRep_TVertex)&)theVertex.TShape();
  gp_Pnt aVertexPnt = aVertex->Pnt();

  const Handle(BRep_TEdge)& anEdge = (const Handle(BRep_TEdge)&)theEdge.TShape();

  // Convert the edge to the curve to calculate the tangency.
  // There should be only one curve in the edge.
  Handle(BRep_Curve3D) aEdCurve = 
    Handle(BRep_Curve3D)::DownCast(anEdge->Curves().First());
  double aFirst, aLast;
  aEdCurve->Range(aFirst, aLast);
  Handle(Geom_Curve) aCurve = aEdCurve->Curve3D();

  gp_Pnt aPnt;
  gp_Vec aTang;
  aCurve->D1(aFirst + shift, aPnt, aTang);
  aCurve->D0(aFirst, aPnt);
  if (aVertexPnt.IsEqual(aPnt, tolerance))
    return gp_Dir(aTang.Reversed());

  aCurve->D1(aLast - shift, aPnt, aTang);
  return gp_Dir(aTang);
}

void removeWasteEdges(
                std::list<TopoDS_Shape>::iterator&  theStartVertex,
                std::list<TopoDS_Shape>::iterator&  theStartEdge,
          const std::list<TopoDS_Shape>::iterator&  theEndOfVertexes,
          const std::list<TopoDS_Shape>::iterator&  theEndOfEdges,
          BOPCol_IndexedDataMapOfShapeListOfShape&  theMapVE)
{
  bool isVertStep = true;
  while (theStartVertex != theEndOfVertexes && theStartEdge != theEndOfEdges)
  {
    BOPCol_ListOfShape& aBunch = theMapVE.ChangeFromKey(*theStartVertex);
    BOPCol_ListOfShape::Iterator anApprEdge(aBunch);
    for ( ; anApprEdge.More(); anApprEdge.Next())
      if (anApprEdge.Value() == *theStartEdge)
        break;
    if (anApprEdge.More())
      aBunch.Remove(anApprEdge);

    if (isVertStep)
      theStartVertex++;
    else
    {
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

