// File:    SketchPlugin_Constraint.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Constraint.h"

#include <ModelAPI_Data.h>
#include <SketchPlugin_Sketch.h>

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Constraint::preview()
{
  return getPreview();
}

void SketchPlugin_Constraint::addConstrainedObject(
          const std::string&                                    theAttrID,
          const boost::shared_ptr<ModelAPI_AttributeReference>& theReference)
{
  if (!data()->attribute(theAttrID).get())
    data()->addAttribute(theAttrID, theReference->type());
  boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(
    data()->attribute(theAttrID))->setValue(theReference->value());
}

void SketchPlugin_Constraint::addConstrainedObject(
          const std::string&                                  theAttrID,
          const boost::shared_ptr<ModelAPI_AttributeRefAttr>& theReference)
{
  if (!data()->attribute(theAttrID).get())
    data()->addAttribute(theAttrID, theReference->type());
  //boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
  //  data()->attribute(theAttrID))->setValue(theReference->value());
}

void SketchPlugin_Constraint::getSketchParameters(
          std::list< boost::shared_ptr<ModelAPI_Attribute> >& theParams)
{
  theParams.push_back(sketch()->data()->attribute(SKETCH_ATTR_ORIGIN));
  theParams.push_back(sketch()->data()->attribute(SKETCH_ATTR_DIRX));
  theParams.push_back(sketch()->data()->attribute(SKETCH_ATTR_DIRY));
  theParams.push_back(sketch()->data()->attribute(SKETCH_ATTR_NORM));
}
