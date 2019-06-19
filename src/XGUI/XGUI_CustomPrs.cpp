// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <XGUI_CustomPrs.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_IModule.h>

#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_ShapeExplorer.h>

#include <Config_PropManager.h>

#include <Events_InfoMessage.h>

#include <vector>
#include <QColor>

double getDeflection(const ResultPtr& theResult)
{
  double aDeflection = -1;
  // get deflection from the attribute of the result
  if (theResult.get() != NULL &&
      theResult->data()->attribute(ModelAPI_Result::DEFLECTION_ID()).get() != NULL) {
    AttributeDoublePtr aDoubleAttr = theResult->data()->real(ModelAPI_Result::DEFLECTION_ID());
    if (aDoubleAttr.get() && aDoubleAttr->isInitialized()) {
      double aValue = aDoubleAttr->value();
      if (aValue > 0) /// zero value should not be used as a deflection(previous studies)
        aDeflection = aDoubleAttr->value();
    }
  }
  return aDeflection;
}

void getColor(const ResultPtr& theResult, std::vector<int>& theColor)
{
  theColor.clear();
  // get color from the attribute of the result
  if (theResult.get() != NULL &&
      theResult->data()->attribute(ModelAPI_Result::COLOR_ID()).get() != NULL) {
    AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
    if (aColorAttr.get() && aColorAttr->size()) {
      theColor.push_back(aColorAttr->value(0));
      theColor.push_back(aColorAttr->value(1));
      theColor.push_back(aColorAttr->value(2));
    }
  }
}

void XGUI_CustomPrs::getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
                                     std::vector<int>& theColor)
{
  theColor.clear();
  // get default color from the preferences manager for the given result
  if (theColor.empty()) {
    std::string aSection, aName, aDefault;
    theObject->colorConfigInfo(aSection, aName, aDefault);
    if (!aSection.empty() && !aName.empty()) {
      theColor = Config_PropManager::color(aSection, aName);
    }
  }
  if (!isEmptyColorValid && theColor.empty()) {
    // all AIS objects, where the color is not set, are in black.
    // The color should be defined in XML or set in the attribute
    theColor = Config_PropManager::color("Visualization", "object_default_color");
    Events_InfoMessage("XGUI_CustomPrs",
      "A default color is not defined in the preferences for this kind of result").send();
  }
}

double XGUI_CustomPrs::getDefaultDeflection(const ObjectPtr& theObject)
{
  double aDeflection = -1;
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aResult.get()) {
    bool isConstruction = false;

    std::string aResultGroup = aResult->groupName();
    if (aResultGroup == ModelAPI_ResultConstruction::group())
      isConstruction = true;
    else if (aResultGroup == ModelAPI_ResultBody::group()) {
      GeomShapePtr aGeomShape = aResult->shape();
      if (aGeomShape.get()) {
        // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
        // correction of deviation for them should not influence to the application performance
        GeomAPI_ShapeExplorer anExp(aGeomShape, GeomAPI_Shape::FACE);
        isConstruction = !anExp.more();
      }
    }
    if (isConstruction)
      aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
    else
      aDeflection = Config_PropManager::real("Visualization", "body_deflection");
  }
  return aDeflection;
}

double getTransparency(const ResultPtr& theResult)
{
  double aTransparency = -1;
  // get transparency from the attribute of the result
  if (theResult.get() != NULL &&
      theResult->data()->attribute(ModelAPI_Result::TRANSPARENCY_ID()).get() != NULL) {
    AttributeDoublePtr aDoubleAttr = theResult->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
    if (aDoubleAttr.get() && aDoubleAttr->isInitialized()) {
      aTransparency = aDoubleAttr->value();
    }
  }
  return aTransparency;
}

double getDefaultTransparency(const ResultPtr& theResult)
{
  return Config_PropManager::real("Visualization", "default_transparency");
}

XGUI_CustomPrs::XGUI_CustomPrs(XGUI_Workshop* theWorkshop)
: myWorkshop(theWorkshop)
{
}

void XGUI_CustomPrs::getResultColor(const ResultPtr& theResult, std::vector<int>& theColor)
{
  getColor(theResult, theColor);
  if (theColor.empty())
    getDefaultColor(theResult, false, theColor);
}

double XGUI_CustomPrs::getResultDeflection(const ResultPtr& theResult)
{
  double aDeflection = getDeflection(theResult);
  if (aDeflection < 0)
    aDeflection = getDefaultDeflection(theResult);
  return aDeflection;
}

double XGUI_CustomPrs::getResultTransparency(const ResultPtr& theResult)
{
  double aTransparency = getTransparency(theResult);
  if (aTransparency < 0)
    aTransparency = getDefaultTransparency(theResult);
  return aTransparency;
}

bool XGUI_CustomPrs::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                           std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs)
{
  bool aCustomized = false;
  if (theResult.get()) {
    std::vector<int> aColor;
    getResultColor(theResult, aColor);

    SessionPtr aMgr = ModelAPI_Session::get();
    if (aMgr->activeDocument() != theResult->document()) {
      QColor aQColor(aColor[0], aColor[1], aColor[2]);
      QColor aNewColor =
        QColor::fromHsvF(aQColor.hueF(), aQColor.saturationF()/3., aQColor.valueF());
      aColor[0] = aNewColor.red();
      aColor[1] = aNewColor.green();
      aColor[2] = aNewColor.blue();
    }
    aCustomized = !aColor.empty() && thePrs->setColor(aColor[0], aColor[1], aColor[2]);

    aCustomized = thePrs->setDeflection(getResultDeflection(theResult)) | aCustomized;

    aCustomized = thePrs->setTransparency(getResultTransparency(theResult)) | aCustomized;
  }
  ModuleBase_IModule* aModule = myWorkshop->module();
  aCustomized = aModule->customisePresentation(theResult, thePrs, theCustomPrs) || aCustomized;
  return aCustomized;
}
