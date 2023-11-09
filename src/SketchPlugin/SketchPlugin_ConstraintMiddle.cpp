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

#include "SketchPlugin_ConstraintMiddle.h"

#include "SketcherPrs_Factory.h"

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeString.h>

#include <ModelAPI_Events.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Tools.h>


class RaiiSetBoolFlag {
public:
  RaiiSetBoolFlag() = delete;
  RaiiSetBoolFlag(bool &theFlag)
  {
    myBoolPtr = &theFlag;
    *myBoolPtr = true;
  }
  ~RaiiSetBoolFlag()
  {
    *myBoolPtr = false;
  }
private:
  bool *myBoolPtr;
};


SketchPlugin_ConstraintMiddle::SketchPlugin_ConstraintMiddle()
{
  myBlockAttribInit = false;
}

// Create new point for Middle constraint
void SketchPlugin_ConstraintMiddle::CreatePoint()
{
  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  auto aTrPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(POINT_REF_ID()));

  if (!myPoint)
  {
    // Get last subfeature (constraintMiddle) for set as parent
    FeaturePtr aCurrentFeature = sketch()->subFeature(sketch()->numberOfSubs() - 1);
    keepCurrentFeature();
    myPoint = sketch()->addFeature(SketchPlugin_Point::ID());
    myPoint->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
    restoreCurrentFeature();

    myPoint->reference(SketchPlugin_Point::PARENT_ID())->setValue(aCurrentFeature);
  }

  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    myPoint->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(aTrPnt->pnt());

  myPoint->execute();

  // Init second attr for constraint
  refattr(SketchPlugin_Constraint::ENTITY_B())->setObject(myPoint);


  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

void SketchPlugin_ConstraintMiddle::initAttributes()
{
  // To maintain compatibility with older study versions, keep the order of all previously existing attributes:
  //  1) ENTITY_A
  //  2) ENTITY_B
  //  ----------- new attributes start here:
  //  3) MIDDLE_TYPE
  //  4) POINT_REF_ID
  AttributeRefAttrPtr aAttrEntA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId()));

  AttributeRefAttrPtr aAttrEntB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId()));
  
  AttributeStringPtr aMethodAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
    data()->addAttribute(SketchPlugin_ConstraintMiddle::MIDDLE_TYPE(), ModelAPI_AttributeString::typeId()));
  
  // When loading studies from older versions (<9.12), the MIDDLE_TYPE attribute did not exist
  // and is therefore not initialized!
  // BUT: in case of a valid middle point constraint, the ENTITY_A and ENTITY_B attributes must be initialized
  bool useDefaultMethod = !aMethodAttr->isInitialized();
  if (aAttrEntA->isInitialized() && aAttrEntB->isInitialized() && useDefaultMethod)
  {
    // Initialize the creation method attribute (MIDDLE_TYPE) with the previously default
    // creation method: "mid point from line and point".
    // NOTE: ensure that the other attributes are NOT reset, as we want to keep the current inputs
    RaiiSetBoolFlag blockEntityReset(myBlockAttribInit);
    aMethodAttr->setValue(MIDDLE_TYPE_BY_LINE_AND_POINT());
  }

  data()->addAttribute(POINT_REF_ID(), GeomDataAPI_Point2D::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), POINT_REF_ID());
}

void SketchPlugin_ConstraintMiddle::execute()
{
  if (string(MIDDLE_TYPE())->value() == MIDDLE_TYPE_BY_LINE())
  {
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(POINT_REF_ID()))->setValue(1., 1.);
    AttributeRefAttrPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_B()));

    auto aRefAttr = data()->refattr(SketchPlugin_Constraint::ENTITY_A())->object();
    if (!aRefAttr.get())
      return;


    if (!attribute(ENTITY_B())->isInitialized())
      CreatePoint(); // Create new point
  }
}

void SketchPlugin_ConstraintMiddle::attributeChanged(const std::string& theID)
{
  if (theID == MIDDLE_TYPE())
  {
    if (!myBlockAttribInit) {
      SketchPlugin_Tools::resetAttribute(this, ENTITY_A());
      SketchPlugin_Tools::resetAttribute(this, ENTITY_B());
    }
  }
  else if (theID == POINT_REF_ID())
  {
    if (!myPoint)
      return;

    auto aTrPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(POINT_REF_ID()));
    AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      myPoint->attribute(SketchPlugin_Point::COORD_ID()));
    aCoord->setValue(aTrPnt->pnt());

    myPoint->execute();
  }
}

AISObjectPtr SketchPlugin_ConstraintMiddle::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;
  AISObjectPtr anAIS = SketcherPrs_Factory::middleConstraint(this, sketch(),
                                                             thePrevious);

  return anAIS;
}
