// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ConstructionPlugin_Validators.cpp
// Created:     04 July 2016
// Author:      Dmitry Bobylev

#include "ConstructionPlugin_Validators.h"

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Vertex.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>

#include <Events_InfoMessage.h>

static std::shared_ptr<GeomAPI_Lin> getLin(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Pln> getPln(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Pnt> getPnt(const GeomShapePtr theShape);

//==================================================================================================
bool ConstructionPlugin_ValidatorPointLines::isValid(const AttributePtr& theAttribute,
                                                     const std::list<std::string>& theArguments,
                                                     Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr aLineAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr aLineAttribute2 = aFeature->selection(theArguments.front());

  GeomShapePtr aLineShape1 = aLineAttribute1->value();
  ResultPtr aContext1 = aLineAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
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
bool ConstructionPlugin_ValidatorPointLineAndPlaneNotParallel::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Lin> aLin;
  std::shared_ptr<GeomAPI_Pln> aPln;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
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
  aPln = getPln(aShape2);
  if(!aLin.get() || !aPln.get()) {
    aLin = getLin(aShape2);
    aPln = getPln(aShape1);
  }

  if(!aLin.get() || !aPln.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }

  if(aPln->isParallel(aLin)) {
    theError = "Plane and line are parallel.";
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

  AttributeSelectionPtr aPointAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr aPointAttribute2 = aFeature->selection(theArguments.front());
  AttributeSelectionPtr aPointAttribute3 = aFeature->selection(theArguments.back());

  GeomShapePtr aPointShape1 = aPointAttribute1->value();
  ResultPtr aContext1 = aPointAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
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

  AttributeSelectionPtr anAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Lin> aLin;
  std::shared_ptr<GeomAPI_Pnt> aPnt;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
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

  AttributeSelectionPtr anAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Pln> aPln1;
  std::shared_ptr<GeomAPI_Pln> aPln2;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
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

  AttributeSelectionPtr anAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Pln> aPln1;
  std::shared_ptr<GeomAPI_Pln> aPln2;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
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

std::shared_ptr<GeomAPI_Lin> getLin(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Lin> aLin;

  if(!theShape->isEdge()) {
    return aLin;
  }

  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theShape));

  if(!anEdge->isLine()) {
    return aLin;
  }

  aLin = anEdge->line();

  return aLin;
}

std::shared_ptr<GeomAPI_Pln> getPln(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Pln> aPln;

  if(!theShape->isFace()) {
    return aPln;
  }

  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(theShape));

  if(!aFace->isPlanar()) {
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
