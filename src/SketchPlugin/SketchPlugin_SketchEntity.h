// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketchPlugin_SketchEntity_H_
#define SketchPlugin_SketchEntity_H_

#include "SketchPlugin.h"
#include "SketchPlugin_Feature.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Session.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeIntArray.h>
#include <GeomAPI_ICustomPrs.h>

#include <Config_PropManager.h>

/**\class SketchPlugin_SketchEntity
 * \ingroup Plugins
 * \brief Sketch Entity for creation of the new feature in PartSet. 
 * This is an abstract class to give
 * an interface to create the entity features such as line, circle, arc and point.
 */
class SketchPlugin_SketchEntity : public SketchPlugin_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Reference to the construction type of the feature
  inline static const std::string& AUXILIARY_ID()
  {
    static const std::string MY_AUXILIARY_ID("Auxiliary");
    return MY_AUXILIARY_ID;
  }

  /// Reference to the external edge or vertex as a AttributeSelection
  inline static const std::string& EXTERNAL_ID()
  {
    static const std::string MY_EXTERNAL_ID("External");
    return MY_EXTERNAL_ID;
  }

  /// Reference to the copy type of the feature
  inline static const std::string& COPY_ID()
  {
    static const std::string MY_COPY_ID("Copy");
    return MY_COPY_ID;
  }

  /// Width of the auxiliary line
  inline static const double SKETCH_LINE_WIDTH_AUXILIARY()
  {
    return 2;
  }

  /// Width of the line
  inline static const double SKETCH_LINE_WIDTH()
  {
    return 3;
  }

  /// Style of the auxiliary line
  inline static const int SKETCH_LINE_STYLE_AUXILIARY()
  {
    return  3;
  }

  /// Style of the line
  inline static const int SKETCH_LINE_STYLE()
  {
    return  0;
  }

  /// Request for initialization of data model of the feature: adding all attributes
  virtual void initAttributes();

  /// Returns true of the feature is created basing on the external shape of not-this-sketch object
  virtual bool isExternal() const
  {
    AttributeSelectionPtr aAttr = data()->selection(EXTERNAL_ID());
    if (aAttr)
      return aAttr->context().get() != NULL && !aAttr->isInvalid();
    return false;
  }

  /// Returns true of the feature is a copy of other feature
  virtual bool isCopy() const
  {
    AttributeBooleanPtr anAttr = data()->boolean(COPY_ID());
    if(anAttr.get()) {
      return anAttr->value();
    }
    return false;
  }

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
  {
    /// Store previous color values of the presentation to check it after setting specific entity
    /// color. Default color is set into presentation to have not modified isCustomized state
    /// after default customize prs is completed.
    std::vector<int> aPrevColor;
    aPrevColor.resize(3);
    thePrs->getColor(aPrevColor[0], aPrevColor[1], aPrevColor[2]);
    if (theResult.get()) {
      std::string aSection, aName, aDefault;
      theResult->colorConfigInfo(aSection, aName, aDefault);
      std::vector<int> aColor;
      aColor = Config_PropManager::color(aSection, aName);
      thePrs->setColor(aColor[0], aColor[1], aColor[2]);
    }

    bool isCustomized = theDefaultPrs.get() != NULL &&
                        theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);
    int aShapeType = thePrs->getShapeType();
    // a compound is processed like the edge because the
    // arc feature uses the compound for presentable AIS
    if (aShapeType != 6/*an edge*/ && aShapeType != 7/*a vertex*/ && aShapeType != 0/*compound*/)
      return false;

    // set color from preferences
    std::vector<int> aColor;
    std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr =
                                    data()->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID());
    bool isConstruction = anAuxiliaryAttr.get() != NULL && anAuxiliaryAttr->value();
    if (isConstruction) {
      aColor = Config_PropManager::color("Visualization", "sketch_auxiliary_color");
    }
    else if (isExternal()) {
      aColor = Config_PropManager::color("Visualization", "sketch_external_color");
    }
    else {
      aColor = Config_PropManager::color("Visualization", "sketch_entity_color");
    }
    if (!aColor.empty()) {
      if (theResult.get() && ModelAPI_Session::get()->isOperation()) {
        AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
        aColorAttr->setSize(3);
        // Set the color attribute in order do not use default colors in the perasentation object
        for (int i = 0; i < 3; i++)
          aColorAttr->setValue(i, aColor[i]);
      }
      thePrs->setColor(aColor[0], aColor[1], aColor[2]);
      for (int i = 0; i < 3 && !isCustomized; i++)
        isCustomized = aColor[i] != aPrevColor[i];
    }

    if (aShapeType == 6 || aShapeType == 0) { // if this is an edge or a compound
      if (isConstruction) {
        isCustomized = thePrs->setWidth(SKETCH_LINE_WIDTH_AUXILIARY()) || isCustomized;
        isCustomized = thePrs->setLineStyle(SKETCH_LINE_STYLE_AUXILIARY()) || isCustomized;
      }
      else {
        isCustomized = thePrs->setWidth(SKETCH_LINE_WIDTH()) || isCustomized;
        isCustomized = thePrs->setLineStyle(SKETCH_LINE_STYLE()) || isCustomized;
      }
    }
    else if (aShapeType == 7) { // otherwise this is a vertex
      // The width value do not have effect on the point presentation.
      // It is defined in order to extend selection area of the object.
      thePrs->setWidth(17);
    //  thePrs->setPointMarker(1, 1.); // Set point as a '+' symbol
    }
    if(isCopy()) {
      double aWidth = thePrs->width();
      isCustomized = thePrs->setWidth(aWidth / 2.5) || isCustomized;
    }

    if (!theResult.get()) {
      double aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
      thePrs->setDeflection(aDeflection);
    }
    return isCustomized;
  }

protected:
  /// initializes mySketch
  SketchPlugin_SketchEntity();

  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes(){};

};

#endif
