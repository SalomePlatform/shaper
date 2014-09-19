// File:        SketchPlugin_Sketch.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeRefList.h>
#include <GeomAPI_AISObject.h>
#include <GeomAPI_XYZ.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <ModelAPI_ResultConstruction.h>

#include <Config_PropManager.h>

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
  std::list<boost::shared_ptr<GeomAPI_Shape> > aLoops;
  std::list<boost::shared_ptr<GeomAPI_Shape> > aWires;
  GeomAlgoAPI_SketchBuilder::createFaces(anOrigin->pnt(), aDirX->dir(), aDirY->dir(), aNorm->dir(),
                                         aFeaturesPreview, aLoops, aWires);

  aLoops.insert(aLoops.end(), aWires.begin(), aWires.end());
  boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aLoops);
  boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(aCompound);
  setResult(aConstr);
}

const void SketchPlugin_Sketch::addSub(const FeaturePtr& theFeature)
{
  boost::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature)->setSketch(this);
  data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->append(theFeature);
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

      aAIS->setWidth(Config_PropManager::integer("Sketch planes", "planes_thikness",
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
    if (aFeature)
      document()->removeFeature(aFeature);
  }
  SketchPlugin_Feature::erase();
}
