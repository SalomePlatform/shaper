// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Sketch.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Config_PropManager.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <GeomAPI_AISObject.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_XYZ.h>

#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_SketchEntity.h>

#include <memory>

#include <math.h>
#include <vector>

using namespace std;

SketchPlugin_Sketch::SketchPlugin_Sketch()
{
}

void SketchPlugin_Sketch::initAttributes()
{
  data()->addAttribute(SketchPlugin_Sketch::ORIGIN_ID(), GeomDataAPI_Point::type());
  data()->addAttribute(SketchPlugin_Sketch::DIRX_ID(), GeomDataAPI_Dir::type());
  data()->addAttribute(SketchPlugin_Sketch::DIRY_ID(), GeomDataAPI_Dir::type());
  data()->addAttribute(SketchPlugin_Sketch::NORM_ID(), GeomDataAPI_Dir::type());
  data()->addAttribute(SketchPlugin_Sketch::FEATURES_ID(), ModelAPI_AttributeRefList::type());
  // the selected face, base for the sketcher plane, not obligatory
  data()->addAttribute(SketchPlugin_SketchEntity::EXTERNAL_ID(), ModelAPI_AttributeSelection::type());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_SketchEntity::EXTERNAL_ID());
}

void SketchPlugin_Sketch::execute()
{
  if (!data()->isValid())
    return;
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aDirY = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRY_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  if (aFeatures.empty())
    return;

  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  std::shared_ptr<SketchPlugin_Feature> aFeature;
  std::list<std::shared_ptr<GeomAPI_Shape> > aFeaturesPreview;
  for (; anIt != aLast; anIt++) {
    aFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
    if (aFeature) {
      if (!aFeature->sketch()) // on load document the back references are missed
        aFeature->setSketch(this);
      // do not include the external edges into the result
      if (aFeature->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID())) {
        if (aFeature->data()->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->value())
          continue;
      }
      // do not include the construction entities in the result
      if (aFeature->data()->attribute(SketchPlugin_SketchEntity::CONSTRUCTION_ID())) {
        if (aFeature->data()->boolean(SketchPlugin_SketchEntity::CONSTRUCTION_ID())->value())
          continue;
      }

      const std::list<std::shared_ptr<ModelAPI_Result> >& aRes = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aRes.cbegin();
      for (; aResIter != aRes.cend(); aResIter++) {
        std::shared_ptr<ModelAPI_ResultConstruction> aConstr = std::dynamic_pointer_cast<
            ModelAPI_ResultConstruction>(*aResIter);
        if (aConstr) {
          std::shared_ptr<GeomAPI_Shape> aShape = aConstr->shape();
          if (aShape)
            aFeaturesPreview.push_back(aShape);
        }
      }
    }
  }

  if (aFeaturesPreview.empty())
    return;

  // Collect all edges as one big wire
  std::shared_ptr<GeomAPI_PlanarEdges> aBigWire(new GeomAPI_PlanarEdges);
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aShapeIt = aFeaturesPreview.begin();
  for (; aShapeIt != aFeaturesPreview.end(); ++aShapeIt) {
    aBigWire->addEdge(*aShapeIt);
  }
  aBigWire->setPlane(anOrigin->pnt(), aDirX->dir(), aDirY->dir(), aNorm->dir());

//  GeomAlgoAPI_SketchBuilder::createFaces(anOrigin->pnt(), aDirX->dir(), aDirY->dir(), aNorm->dir(),
//                                         aFeaturesPreview, aLoops, aWires);
  std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(aBigWire);
  setResult(aConstr);
}

std::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::addFeature(std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID);
  if (aNew) {
    std::dynamic_pointer_cast<SketchPlugin_Feature>(aNew)->setSketch(this);
    data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->append(aNew);
  }
  return aNew;
}

void SketchPlugin_Sketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!data().get()) // sketch is already removed (case on undo of sketch), sync is not needed
    return;
  list<ObjectPtr> aSubs = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->list();
  list<ObjectPtr>::iterator aSubIt = aSubs.begin(), aLastIt = aSubs.end();
  bool isRemoved = false;
  bool aHasEmtpyFeature = false;
  for(; aSubIt != aLastIt && !isRemoved; aSubIt++) {
    std::shared_ptr<ModelAPI_Feature> aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*aSubIt);
    if (aFeature.get() != NULL && aFeature == theFeature) {
      data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->remove(aFeature);
      isRemoved = true;
    }
    else if (aFeature.get() == NULL)
      aHasEmtpyFeature = true;
  }
  // if the object is not found in the sketch sub-elements, that means that the object is removed already.
  // Find the first empty element and remove it
  if (!isRemoved && aHasEmtpyFeature)
    data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->remove(ObjectPtr());
}

int SketchPlugin_Sketch::numberOfSubs() const
{
  return data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->size();
}

std::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::subFeature(const int theIndex) const
{
  ObjectPtr anObj = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->object(theIndex);
  return std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
}

int SketchPlugin_Sketch::subFeatureId(const int theIndex) const
{
  return subFeature(theIndex)->data()->featureId();
}

bool SketchPlugin_Sketch::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature) {
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aRes)
      aFeature = document()->feature(aRes);
  }
  if (aFeature) {
    list<ObjectPtr> aSubs = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->list();
    for(list<ObjectPtr>::iterator aSubIt = aSubs.begin(); aSubIt != aSubs.end(); aSubIt++) {
      if (*aSubIt == aFeature)
        return true;
    }
  }
  return false;
}

std::shared_ptr<GeomAPI_Pnt> SketchPlugin_Sketch::to3D(const double theX, const double theY)
{
  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aY = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRY_ID()));

  std::shared_ptr<GeomAPI_XYZ> aSum = aC->pnt()->xyz()->added(aX->dir()->xyz()->multiplied(theX))
      ->added(aY->dir()->xyz()->multiplied(theY));

  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum));
}

std::shared_ptr<GeomAPI_Pnt2d> SketchPlugin_Sketch::to2D(
  const std::shared_ptr<GeomAPI_Pnt>& thePnt)
{
  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aY = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRY_ID()));
  return thePnt->to2D(aC->pnt(), aX->dir(), aY->dir());
}


bool SketchPlugin_Sketch::isPlaneSet()
{
  std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  return aNormal && !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);
}

std::shared_ptr<GeomAPI_Pln> SketchPlugin_Sketch::plane()
{
  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  if (!anOrigin || !aNorm)
    return std::shared_ptr<GeomAPI_Pln>();

  return std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(anOrigin->pnt(), aNorm->dir()));
}

std::shared_ptr<GeomAPI_Ax3> SketchPlugin_Sketch::coordinatePlane() const
{
  DataPtr aData = data();
  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
    aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aY = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aData->attribute(SketchPlugin_Sketch::DIRY_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aData->attribute(SketchPlugin_Sketch::NORM_ID()));

  return std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(aC->pnt(), aX->dir(), aY->dir(), aNorm->dir()));
}

void SketchPlugin_Sketch::erase()
{
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));
  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin();
  for (; anIt != aFeatures.end(); anIt++) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    if (aFeature) {
      // subs are referenced from sketch, but must be removed for sure, so not checkings
      document()->removeFeature(aFeature);
    }
  }
  ModelAPI_CompositeFeature::erase();
}

void SketchPlugin_Sketch::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_SketchEntity::EXTERNAL_ID()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = 
      data()->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->value();
    if (aSelection) { // update arguments due to the selection value
      // update the sketch plane
      std::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aSelection);
      if (aPlane) {
        double anA, aB, aC, aD;
        aPlane->coefficients(anA, aB, aC, aD);

        // calculate attributes of the sketch
        std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
        std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
        std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
        aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
        std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
        // X axis is preferable to be dirX on the sketch
        static const double tol = 1.e-7;
        bool isX = fabs(anA - 1.0) < tol && fabs(aB) < tol && fabs(aC) < tol;
        std::shared_ptr<GeomAPI_Dir> aTempDir(
          isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
        std::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
        std::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

        // update position of the sketch
        std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast
          <GeomDataAPI_Point>(data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
        anOrigin->setValue(anOrigPnt);
        std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          data()->attribute(SketchPlugin_Sketch::NORM_ID()));
        aNormal->setValue(aNormDir);
        std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
        aDirX->setValue(aXDir);
        std::shared_ptr<GeomDataAPI_Dir> aDirY = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          data()->attribute(SketchPlugin_Sketch::DIRY_ID()));
        aDirY->setValue(aYDir);
        std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
      }
    }
  }
}
