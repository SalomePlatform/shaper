// Copyright (C) 2014-2023  CEA, EDF
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

#include "BuildPlugin_Face.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_NonPlanarFace.h>

//=================================================================================================
BuildPlugin_Face::BuildPlugin_Face()
{
}

//=================================================================================================
void BuildPlugin_Face::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Face::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if(aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Collect base shapes.
  ListOfShape anEdges;
  ListOfShape aNonPlanarEdges;
  ListOfShape anOriginalFaces;
  ListOfShape aContexts;

  getOriginalShapesAndContexts(BASE_OBJECTS_ID(), anOriginalFaces, aContexts);
  anOriginalFaces.clear();
  std::list< std::shared_ptr<GeomAPI_Dir> > aListOfNormals;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    GeomShapePtr aContext = aSelection->context()->shape();
    if(!aShape.get()) {
      aShape = aContext;
    }
    if (aShape->shapeType() == GeomAPI_Shape::FACE) {
      // keep selected faces "as is"
      anOriginalFaces.push_back(aShape);
      continue;
    }
    else if (!aSelection->value() && aShape->shapeType() == GeomAPI_Shape::COMPOUND) {
      // collect faces from the sketch
      ResultConstructionPtr aSketch =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSelection->context());
      if (aSketch && aSketch->facesNum() > 0) {
        for (int i = 0; i < aSketch->facesNum(); ++i)
          anOriginalFaces.push_back(aSketch->face(i));
        continue;
      }
    }

    // check whether the context is a sketch, in this case store its normal for further needs
    std::shared_ptr<GeomAPI_PlanarEdges> aSketch =
        std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContext);
    if (aSketch)
      aListOfNormals.push_back(aSketch->norm());

    bool isPlanar(aShape->isPlanar() || aSketch);

    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      isPlanar? anEdges.push_back(anEdge) : aNonPlanarEdges.push_back(anEdge);
    }
  }

  if (!anEdges.empty())
  {
    //check is planar objects belong to nke
    std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_ShapeTools::findPlane(anEdges);
    if(!aPln.get())
    {
      aNonPlanarEdges.insert(aNonPlanarEdges.end(), anEdges.begin(), anEdges.end());
      anEdges.clear();
    }
  }

  // Build faces by edges.
  ListOfShape aFaces;
  GeomMakeShapePtr aFaceBuilder;
  if (!anEdges.empty())
    buildFacesByEdges(anEdges, aListOfNormals, aFaces, aFaceBuilder);
  int aNbFacesFromEdges = (int)aFaces.size();

  // Build non-planar faces by edges.
  GeomMakeShapePtr aNonPlanarFaceBuilder;
  int aNbNonPlanarFaces = (int)aFaces.size();
  if(!aNonPlanarEdges.empty())
  {
    ListOfShape aNonPlanarFaces;
    buildNonPlanarFacesByEdges(aNonPlanarEdges, aNonPlanarFaces, aNonPlanarFaceBuilder);
    // Add non-planar faces to common faces list.
    aFaces.insert(aFaces.end(), aNonPlanarFaces.begin(), aNonPlanarFaces.end());
    aNbNonPlanarFaces += (int)aNonPlanarFaces.size();
  }

  // Add faces selected by user.
  aFaces.insert(aFaces.end(), anOriginalFaces.begin(), anOriginalFaces.end());

  // Store result.
  int anIndex = 0;
  for(ListOfShape::const_iterator anIt = aFaces.cbegin(); anIt != aFaces.cend(); ++anIt) {
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
    if (anIndex < aNbFacesFromEdges)
      aMakeShapeList->appendAlgo(aFaceBuilder);
    else if(anIndex < aNbNonPlanarFaces)
      aMakeShapeList->appendAlgo(aNonPlanarFaceBuilder);

    GeomShapePtr aShape = *anIt;
    GeomMakeShapePtr aCopy(new GeomAlgoAPI_Copy(aShape));
    aMakeShapeList->appendAlgo(aCopy);

    ListOfShape aBaseShapes;
    if (anIndex < aNbFacesFromEdges)
      aBaseShapes = anEdges;
    else if(anIndex < aNbNonPlanarFaces)
      aBaseShapes = aNonPlanarEdges;
    else
      aBaseShapes.push_back(aShape);
    storeResult(aMakeShapeList, aBaseShapes, aContexts, aCopy->shape(), anIndex++);
  }

  removeResults(anIndex);
}

//==================================================================================================
void BuildPlugin_Face::buildFacesByEdges(
    const ListOfShape& theEdges,
    const std::list< std::shared_ptr<GeomAPI_Dir> >& theNormals,
    ListOfShape& theFaces,
    std::shared_ptr<GeomAlgoAPI_MakeShape>& theBuilderAlgo) const
{
  // Get plane.
  std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_ShapeTools::findPlane(theEdges);
  std::shared_ptr<GeomAPI_Dir> aNormal = aPln->direction();
  bool isReverse = !theNormals.empty();
  std::list< std::shared_ptr<GeomAPI_Dir> >::const_iterator aNormIt = theNormals.begin();
  for (; aNormIt != theNormals.end() && isReverse; ++aNormIt)
    if ((*aNormIt)->dot(aNormal) > 1.e-7)
      isReverse = false;
  if (isReverse) {
    aNormal->reverse();
    aPln = std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aPln->location(), aNormal));
  }

  // Get faces.
  std::shared_ptr<GeomAlgoAPI_SketchBuilder> aSketchBuilder(
      new GeomAlgoAPI_SketchBuilder(aPln, theEdges));
  theFaces = aSketchBuilder->faces();
  theBuilderAlgo = aSketchBuilder;

  // Get wires from faces.
  ListOfShape aWires;
  for(ListOfShape::const_iterator anIt = theFaces.cbegin(); anIt != theFaces.cend(); ++anIt)
    aWires.push_back(GeomAlgoAPI_ShapeTools::getFaceOuterWire(*anIt));

  // Make faces with holes.
  theFaces.clear();
  GeomAlgoAPI_ShapeTools::makeFacesWithHoles(aPln->location(), aPln->direction(),
                                             aWires, theFaces);
}

//==================================================================================================
void BuildPlugin_Face::buildNonPlanarFacesByEdges(
    const ListOfShape& theShapes,
    ListOfShape& theFaces,
    std::shared_ptr<GeomAlgoAPI_MakeShape>& theBuilderAlgo) const
{

    // Get faces.
    std::shared_ptr<GeomAlgoAPI_NonPlanarFace> aNonPlanarFaceBuilder
            (new GeomAlgoAPI_NonPlanarFace(theShapes));
    theFaces = aNonPlanarFaceBuilder->faces();
    theBuilderAlgo = aNonPlanarFaceBuilder;

}
