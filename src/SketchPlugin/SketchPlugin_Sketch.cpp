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

#include <boost/smart_ptr/shared_ptr.hpp>

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
  data()->addAttribute(SketchPlugin_Feature::EXTERNAL_ID(), ModelAPI_AttributeSelection::type());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_Feature::EXTERNAL_ID());
}

void SketchPlugin_Sketch::execute()
{
  if (!data()->isValid())
    return;
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  boost::shared_ptr<GeomDataAPI_Point> anOrigin = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aDirX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aDirY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRY_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aNorm = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  if (aFeatures.empty())
    return;

  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  boost::shared_ptr<SketchPlugin_Feature> aFeature;
  std::list<boost::shared_ptr<GeomAPI_Shape> > aFeaturesPreview;
  for (; anIt != aLast; anIt++) {
    aFeature = boost::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
    if (aFeature) {
      // do not include the external edges into the result
      if (aFeature->data()->attribute(SketchPlugin_Feature::EXTERNAL_ID())) {
        if (aFeature->data()->selection(SketchPlugin_Feature::EXTERNAL_ID())->value())
          continue;
      }

      const std::list<boost::shared_ptr<ModelAPI_Result> >& aRes = aFeature->results();
      std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aRes.cbegin();
      for (; aResIter != aRes.cend(); aResIter++) {
        boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = boost::dynamic_pointer_cast<
            ModelAPI_ResultConstruction>(*aResIter);
        if (aConstr) {
          boost::shared_ptr<GeomAPI_Shape> aShape = aConstr->shape();
          if (aShape)
            aFeaturesPreview.push_back(aShape);
        }
      }
    }
  }

  if (aFeaturesPreview.empty())
    return;

  // Collect all edges as one big wire
  boost::shared_ptr<GeomAPI_PlanarEdges> aBigWire(new GeomAPI_PlanarEdges);
  std::list<boost::shared_ptr<GeomAPI_Shape> >::const_iterator aShapeIt = aFeaturesPreview.begin();
  for (; aShapeIt != aFeaturesPreview.end(); ++aShapeIt) {
    aBigWire->addEdge(*aShapeIt);
  }
  aBigWire->setOrigin(anOrigin->pnt());
  aBigWire->setDirX(aDirX->dir());
  aBigWire->setDirY(aDirY->dir());
  aBigWire->setNorm(aNorm->dir());

//  GeomAlgoAPI_SketchBuilder::createFaces(anOrigin->pnt(), aDirX->dir(), aDirY->dir(), aNorm->dir(),
//                                         aFeaturesPreview, aLoops, aWires);
  boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(aBigWire);
  setResult(aConstr);
}

boost::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::addFeature(std::string theID)
{
  boost::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID);
  if (aNew) {
    boost::dynamic_pointer_cast<SketchPlugin_Feature>(aNew)->setSketch(this);
    data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->append(aNew);
  }
  return aNew;
}

int SketchPlugin_Sketch::numberOfSubs() const
{
  return data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->size();
}

boost::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::subFeature(const int theIndex) const
{
  ObjectPtr anObj = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->object(theIndex);
  return boost::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
}

int SketchPlugin_Sketch::subFeatureId(const int theIndex) const
{
  return subFeature(theIndex)->data()->featureId();
}

bool SketchPlugin_Sketch::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature) {
    ResultPtr aRes = boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
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

boost::shared_ptr<GeomAPI_Pnt> SketchPlugin_Sketch::to3D(const double theX, const double theY)
{
  boost::shared_ptr<GeomDataAPI_Point> aC = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRY_ID()));

  boost::shared_ptr<GeomAPI_XYZ> aSum = aC->pnt()->xyz()->added(aX->dir()->xyz()->multiplied(theX))
      ->added(aY->dir()->xyz()->multiplied(theY));

  return boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum));
}

boost::shared_ptr<GeomAPI_Pnt2d> SketchPlugin_Sketch::to2D(
  const boost::shared_ptr<GeomAPI_Pnt>& thePnt)
{
  boost::shared_ptr<GeomDataAPI_Point> aC = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRY_ID()));
  return thePnt->to2D(aC->pnt(), aX->dir(), aY->dir());
}


bool SketchPlugin_Sketch::isPlaneSet()
{
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  return aNormal && !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);
}

boost::shared_ptr<GeomAPI_Pln> SketchPlugin_Sketch::plane()
{
  boost::shared_ptr<GeomDataAPI_Point> anOrigin = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aNorm = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  if (!anOrigin || !aNorm)
    return boost::shared_ptr<GeomAPI_Pln>();

  return boost::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(anOrigin->pnt(), aNorm->dir()));
}

void addPlane(double theX, double theY, double theZ,
              std::list<boost::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  boost::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
  boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(theX, theY, theZ));
  double aSize = Config_PropManager::integer("Sketch planes", "Size of planes", PLANE_SIZE);
  boost::shared_ptr<GeomAPI_Shape> aFace = GeomAlgoAPI_FaceBuilder::square(anOrigin, aNormal,
                                                                           aSize);
  theShapes.push_back(aFace);
}

AISObjectPtr SketchPlugin_Sketch::getAISObject(AISObjectPtr thePrevious)
{
  boost::shared_ptr<GeomDataAPI_Dir> aNorm = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  if (!aNorm || (aNorm->x() == 0 && aNorm->y() == 0 && aNorm->z() == 0)) {
    AISObjectPtr aAIS = thePrevious;
    if (!aAIS) {
      std::list<boost::shared_ptr<GeomAPI_Shape> > aFaces;

      addPlane(1, 0, 0, aFaces);  // YZ plane
      addPlane(0, 1, 0, aFaces);  // XZ plane
      addPlane(0, 0, 1, aFaces);  // XY plane
      boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aFaces);
      aAIS = AISObjectPtr(new GeomAPI_AISObject());
      aAIS->createShape(aCompound);

      std::vector<int> aRGB = Config_PropManager::color("Sketch planes", "planes_color",
      SKETCH_PLANE_COLOR);
      aAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);

      aAIS->setWidth(Config_PropManager::integer("Sketch planes", "planes_thickness",
      SKETCH_WIDTH));
    }
    return aAIS;
  }
  return AISObjectPtr();
}

void SketchPlugin_Sketch::erase()
{
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));
  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin();
  for (; anIt != aFeatures.end(); anIt++) {
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    if (aFeature) {
       // subs are referenced from sketch, but must be removed for sure, so not checkings
      document()->removeFeature(aFeature, false);
    }
  }
  ModelAPI_CompositeFeature::erase();
}

void SketchPlugin_Sketch::attributeChanged() {
  static bool myIsUpdated = false; // to avoid infinitive cycle on attrubtes change
  boost::shared_ptr<GeomAPI_Shape> aSelection = 
    data()->selection(SketchPlugin_Feature::EXTERNAL_ID())->value();
  if (aSelection && !myIsUpdated) { // update arguments due to the selection value
    myIsUpdated = true;
    // update the sketch plane
    boost::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aSelection);
    if (aPlane) {
      double anA, aB, aC, aD;
      aPlane->coefficients(anA, aB, aC, aD);

      // calculate attributes of the sketch
      boost::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
      boost::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
      boost::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
      aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
      boost::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
      // X axis is preferable to be dirX on the sketch
      static const double tol = 1.e-7;
      bool isX = fabs(anA - 1.0) < tol && fabs(aB) < tol && fabs(aC) < tol;
      boost::shared_ptr<GeomAPI_Dir> aTempDir(
        isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
      boost::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
      boost::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

      boost::shared_ptr<GeomDataAPI_Point> anOrigin = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
        data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
      anOrigin->setValue(anOrigPnt);
      boost::shared_ptr<GeomDataAPI_Dir> aNormal = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(SketchPlugin_Sketch::NORM_ID()));
      aNormal->setValue(aNormDir);
      boost::shared_ptr<GeomDataAPI_Dir> aDirX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
      aDirX->setValue(aXDir);
      boost::shared_ptr<GeomDataAPI_Dir> aDirY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
        data()->attribute(SketchPlugin_Sketch::DIRY_ID()));
      aDirY->setValue(aYDir);
      boost::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
    }
    myIsUpdated = false;
  }
}
