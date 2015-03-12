// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_SketchEntity.h
// Created:     05 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SketchPlugin_SketchEntity_H_
#define SketchPlugin_SketchEntity_H_

#include "SketchPlugin.h"
#include "SketchPlugin_Feature.h"

#include <ModelAPI_CompositeFeature.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <GeomAPI_ICustomPrs.h>

#include <Config_PropManager.h>

#define SKETCH_ENTITY_COLOR "#ff0000"
#define SKETCH_EXTERNAL_COLOR "#00ff00"
#define SKETCH_CONSTRUCTION_COLOR "#000000"

/**\class SketchPlugin_SketchEntity
 * \ingroup Plugins
 * \brief Sketch Entity for creation of the new feature in PartSet. This is an abstract class to give
 * an interface to create the entity features such as line, circle, arc and point.
 */
class SketchPlugin_SketchEntity : public SketchPlugin_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Reference to the construction type of the feature
  inline static const std::string& CONSTRUCTION_ID()
  {
    static const std::string MY_CONSTRUCTION_ID("Construction");
    return MY_CONSTRUCTION_ID;
  }

  /// Reference to the external edge or vertex as a AttributeSelection
  inline static const std::string& EXTERNAL_ID()
  {
    static const std::string MY_EXTERNAL_ID("External");
    return MY_EXTERNAL_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes
  virtual void initAttributes();

  /// Returns true of the feature is created basing on the external shape of not-this-sketch object
  virtual bool isExternal() const
  {
    AttributeSelectionPtr aAttr = data()->selection(EXTERNAL_ID());
    if (aAttr)
      return aAttr->context().get() != NULL;
    return false;
  }

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
  {
    bool isCustomized = false;
    int aShapeType = thePrs->getShapeType();
    // a compound is processed like the edge because the arc feature uses the compound for presentable AIS
    if (aShapeType != 6/*an edge*/ && aShapeType != 7/*a vertex*/ && aShapeType != 0/*compound*/)
      return false;

    // set color from preferences
    std::vector<int> aColor;
    std::shared_ptr<ModelAPI_AttributeBoolean> aConstructionAttr =
                                    data()->boolean(SketchPlugin_SketchEntity::CONSTRUCTION_ID());
    bool isConstruction = aConstructionAttr.get() != NULL && aConstructionAttr->value();
    if (isConstruction) {
      aColor = Config_PropManager::color("Visualization", "sketch_construction_color",
                                         SKETCH_CONSTRUCTION_COLOR);
    }
    else if (isExternal()) {
      aColor = Config_PropManager::color("Visualization", "sketch_external_color",
                                        SKETCH_EXTERNAL_COLOR);
    }
    else {
      aColor = Config_PropManager::color("Visualization", "sketch_entity_color",
                                          SKETCH_ENTITY_COLOR);
    }
    if (!aColor.empty())
      isCustomized = thePrs->setColor(aColor[0], aColor[1], aColor[2]) || isCustomized;

    if (aShapeType == 6 || aShapeType == 0) { // if this is an edge or a compound
      if (isConstruction) {
        isCustomized = thePrs->setWidth(1) || isCustomized;
        isCustomized = thePrs->setLineStyle(3) || isCustomized;
      }
      else {
        isCustomized = thePrs->setWidth(3) || isCustomized;
        isCustomized = thePrs->setLineStyle(0) || isCustomized;
      }
    }
    else if (aShapeType == 7) { // otherwise this is a vertex
      //  thePrs->setPointMarker(6, 2.);
    }
    return isCustomized;
  }

protected:
  /// initializes mySketch
  SketchPlugin_SketchEntity();
};

#endif
