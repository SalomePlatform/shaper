// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "ConstructionPlugin_Validators.h"

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>

#include <Events_InfoMessage.h>

static std::shared_ptr<GeomAPI_Edge> getEdge(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Face> getFace(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Lin> getLin(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Pln> getPln(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Pnt> getPnt(const GeomShapePtr theShape);

//==================================================================================================
bool ConstructionPlugin_ValidatorPointLines::isValid(const AttributePtr& theAttribute,
                                                     const std::list<std::string>& theArguments,
                                                     Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr aLineAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr aLineAttribute2 = aFeature->selection(theArguments.front());

  GeomShapePtr aLineShape1 = aLineAttribute1->value();
  ResultPtr aContext1 = aLineAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if(!aLineShape1.get()) {
    aLineShape1 = aContext1->shape();
  }
  if(!aLineShape1->isEdge()) {
    theError = "One of the selected shapes not an edge.";
    return false;
  }

  GeomShapePtr aLineShape2 = aLineAttribute2->value();
  ResultPtr aContext2 = aLineAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aLineShape2.get()) {
    aLineShape2 = aContext2->shape();
  }
  if(!aLineShape2->isEdge()) {
    theError = "One of the selected shapes not an edge.";
    return false;
  }

  std::shared_ptr<GeomAPI_Edge> aLineEdge1(new GeomAPI_Edge(aLineShape1));
  std::shared_ptr<GeomAPI_Edge> aLineEdge2(new GeomAPI_Edge(aLineShape2));

  std::shared_ptr<GeomAPI_Lin> aLine1 = aLineEdge1->line();
  std::shared_ptr<GeomAPI_Lin> aLine2 = aLineEdge2->line();

  if (!aLine1.get() || !aLine2.get()) {
    theError = "Selected edge is not a line.";
    return false;
  }

  if(!aLine1->isCoplanar(aLine2)) {
    theError = "Selected lines not coplanar.";
    return false;
  }

  if(aLine1->isParallel(aLine2)) {
    theError = "Selected lines are parallel.";
    return false;
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorPointEdgeAndPlaneNotParallel::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<GeomAPI_Face> aFace;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if(!aShape1.get()) {
    aShape1 = aContext1->shape();
  }

  GeomShapePtr aShape2 = anAttribute2->value();
  ResultPtr aContext2 = anAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aShape2.get()) {
    aShape2 = aContext2->shape();
  }

  anEdge = getEdge(aShape1);
  aFace = getFace(aShape2);
  if(!anEdge.get() || !aFace.get()) {
    anEdge = getEdge(aShape2);
    aFace = getFace(aShape1);
  }

  if(!anEdge.get() || !aFace.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }

  if(GeomAlgoAPI_ShapeTools::isParallel(anEdge, aFace)) {
    theError = "Plane and edge are parallel.";
    return false;
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorPlaneThreePoints::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr aPointAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr aPointAttribute2 = aFeature->selection(theArguments.front());
  AttributeSelectionPtr aPointAttribute3 = aFeature->selection(theArguments.back());

  GeomShapePtr aPointShape1 = aPointAttribute1->value();
  ResultPtr aContext1 = aPointAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if(!aPointShape1.get()) {
    aPointShape1 = aContext1->shape();
  }
  if(!aPointShape1->isVertex()) {
    theError = "One of the selected shapes not a vertex.";
    return false;
  }

  GeomShapePtr aPointShape2 = aPointAttribute2->value();
  ResultPtr aContext2 = aPointAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aPointShape2.get()) {
    aPointShape2 = aContext2->shape();
  }
  if(!aPointShape2->isVertex()) {
    theError = "One of the selected shapes not a vertex.";
    return false;
  }

  GeomShapePtr aPointShape3 = aPointAttribute3->value();
  ResultPtr aContext3 = aPointAttribute3->context();
  if(!aContext3.get()) {
    return true;
  }
  if(!aPointShape3.get()) {
    aPointShape3 = aContext3->shape();
  }
  if(!aPointShape3->isVertex()) {
    theError = "One of the selected shapes not a vertex.";
    return false;
  }

  std::shared_ptr<GeomAPI_Vertex> aVertex1(new GeomAPI_Vertex(aPointShape1));
  std::shared_ptr<GeomAPI_Vertex> aVertex2(new GeomAPI_Vertex(aPointShape2));
  std::shared_ptr<GeomAPI_Vertex> aVertex3(new GeomAPI_Vertex(aPointShape3));

  std::shared_ptr<GeomAPI_Pnt> aPnt1 = aVertex1->point();
  std::shared_ptr<GeomAPI_Pnt> aPnt2 = aVertex2->point();
  std::shared_ptr<GeomAPI_Pnt> aPnt3 = aVertex3->point();

  if (aPnt1->isEqual(aPnt2)) {
    theError = "Selected points are equal";
    return false;
  }

  std::shared_ptr<GeomAPI_Lin> aLin(new GeomAPI_Lin(aPnt1, aPnt2));

  if(aLin->contains(aPnt3)) {
    theError = "Selected points lie on a line.";
    return false;
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorPlaneLinePoint::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Lin> aLin;
  std::shared_ptr<GeomAPI_Pnt> aPnt;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if(!aShape1.get()) {
    aShape1 = aContext1->shape();
  }

  GeomShapePtr aShape2 = anAttribute2->value();
  ResultPtr aContext2 = anAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aShape2.get()) {
    aShape2 = aContext2->shape();
  }

  aLin = getLin(aShape1);
  aPnt = getPnt(aShape2);
  if(!aLin.get() || !aPnt.get()) {
    aLin = getLin(aShape2);
    aPnt = getPnt(aShape1);
  }

  if(!aLin.get() || !aPnt.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }

  // line should not contain point only for not-prependicular case
  AttributeBooleanPtr aBoolAttr = aFeature->boolean(theArguments.back());
  if (aBoolAttr.get() && !aBoolAttr->value()) {
    if(aLin->contains(aPnt)) {
      theError = "Point lies on the line.";
      return false;
    }
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorPlaneTwoParallelPlanes::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Pln> aPln1;
  std::shared_ptr<GeomAPI_Pln> aPln2;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if(!aShape1.get()) {
    aShape1 = aContext1->shape();
  }

  GeomShapePtr aShape2 = anAttribute2->value();
  ResultPtr aContext2 = anAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aShape2.get()) {
    aShape2 = aContext2->shape();
  }

  aPln1 = getPln(aShape1);
  aPln2 = getPln(aShape2);

  if(!aPln1.get() || !aPln2.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }

  std::shared_ptr<GeomAPI_Dir> aDir1 = aPln1->direction();
  std::shared_ptr<GeomAPI_Dir> aDir2 = aPln2->direction();

  if(!aDir1->isParallel(aDir2)) {
    theError = "Planes not parallel.";
    return false;
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Pln> aPln1;
  std::shared_ptr<GeomAPI_Pln> aPln2;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if(!aShape1.get()) {
    aShape1 = aContext1->shape();
  }

  GeomShapePtr aShape2 = anAttribute2->value();
  ResultPtr aContext2 = anAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aShape2.get()) {
    aShape2 = aContext2->shape();
  }

  aPln1 = getPln(aShape1);
  aPln2 = getPln(aShape2);

  if(!aPln1.get() || !aPln2.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }

  std::shared_ptr<GeomAPI_Dir> aDir1 = aPln1->direction();
  std::shared_ptr<GeomAPI_Dir> aDir2 = aPln2->direction();

  if(aDir1->isParallel(aDir2)) {
    theError = "Planes are parallel.";
    return false;
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorPointThreeNonParallelPlanes::isValid(
  const AttributePtr& theAttribute,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());
  AttributeSelectionPtr anAttribute3 = aFeature->selection(theArguments.back());

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if (!aContext1.get()) {
    theError = "One of the attribute is not initialized.";
    return false;
  }
  if (!aShape1.get()) {
    aShape1 = aContext1->shape();
  }

  std::shared_ptr<GeomAPI_Pln> aPln1 = getPln(aShape1);
  if (!aPln1.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }
  std::shared_ptr<GeomAPI_Dir> aDir1 = aPln1->direction();

  if (anAttribute2.get()) {
    GeomShapePtr aShape2 = anAttribute2->value();
    ResultPtr aContext2 = anAttribute2->context();
    if (!aShape2.get() && aContext2.get()) {
      aShape2 = aContext2->shape();
    }

    if (aShape2.get()) {
      std::shared_ptr<GeomAPI_Pln> aPln2 = getPln(aShape2);
      if (!aPln2.get()) {
        theError = "Wrong shape types selected.";
        return false;
      }
      std::shared_ptr<GeomAPI_Dir> aDir2 = aPln2->direction();
      if (aDir1->isParallel(aDir2)) {
        theError = "Planes are parallel.";
        return false;
      }
    }
  }

  if (anAttribute3.get()) {
    GeomShapePtr aShape3 = anAttribute3->value();
    ResultPtr aContext3 = anAttribute3->context();
    if (!aShape3.get() && aContext3.get()) {
      aShape3 = aContext3->shape();
    }

    if (aShape3.get()) {
      std::shared_ptr<GeomAPI_Pln> aPln3 = getPln(aShape3);
      if (!aPln3.get()) {
        theError = "Wrong shape types selected.";
        return false;
      }
      std::shared_ptr<GeomAPI_Dir> aDir3 = aPln3->direction();
      if (aDir1->isParallel(aDir3)) {
        theError = "Planes are parallel.";
        return false;
      }
    }
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorNotFeature::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& /*theArguments*/,
    Events_InfoMessage& theError) const
{
  AttributeSelectionPtr aSelAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  if (!aSelAttr) {
    theError = "Wrong attribute";
    return false;
  }

  FeaturePtr aContextFeature = aSelAttr->contextFeature();
  if (aContextFeature) {
    theError = "Feature should not be selected";
    return false;
  }
  return true;
}

//==================================================================================================

std::shared_ptr<GeomAPI_Edge> getEdge(const GeomShapePtr theShape)
{
  GeomEdgePtr anEdge;

  if(theShape->isEdge()) {
    anEdge = theShape->edge();
  }
  else if (theShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(theShape);
    anEdge = anIt.current()->edge();
  }

  return anEdge;
}

GeomFacePtr getFace(const GeomShapePtr theShape)
{
  GeomFacePtr aFace;

  if (theShape->isFace()) {
    aFace = theShape->face();
  }
  else if (theShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(theShape);
    aFace = anIt.current()->face();
  }

  return aFace;
}

std::shared_ptr<GeomAPI_Lin> getLin(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Lin> aLin;

  GeomEdgePtr anEdge;

  if (theShape->isEdge()) {
    anEdge = theShape->edge();
  }
  else if (theShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(theShape);
    anEdge = anIt.current()->edge();
  }
  else {
    return aLin;
  }

  if(!anEdge->isLine()) {
    return aLin;
  }

  aLin = anEdge->line();

  return aLin;
}

std::shared_ptr<GeomAPI_Pln> getPln(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Pln> aPln;

  GeomFacePtr aFace;

  if(theShape->isFace()) {
    aFace = theShape->face();
  }
  else if (theShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(theShape);
    aFace = anIt.current()->face();
  }
  else {
    return aPln;
  }

  if(!aFace || !aFace->isPlanar()) {
    return aPln;
  }

  aPln = aFace->getPlane();

  return aPln;
}

std::shared_ptr<GeomAPI_Pnt> getPnt(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Pnt> aPnt;

  if(!theShape->isVertex()) {
    return aPnt;
  }

  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(theShape));

  aPnt = aVertex->point();

  return aPnt;
}
