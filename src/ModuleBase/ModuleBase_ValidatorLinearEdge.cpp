// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "ModuleBase_ValidatorLinearEdge.h"
#include "ModuleBase_WidgetShapeSelector.h"

#include <GeomAPI_Curve.h>

#include <Events_Error.h>

#include <StdSelect_TypeOfEdge.hxx>

#include <QString>
#include <QMap>


typedef QMap<QString, ModuleBase_ValidatorLinearEdge::TypeOfEdge> EdgeTypes;
static EdgeTypes MyEdgeTypes;

ModuleBase_ValidatorLinearEdge::TypeOfEdge ModuleBase_ValidatorLinearEdge::edgeType(const std::string& theType)
{
  if (MyEdgeTypes.count() == 0) {
    MyEdgeTypes["line"] = Line;
    MyEdgeTypes["circle"] = Circle;
  }
  QString aType = QString(theType.c_str()).toLower();
  if (MyEdgeTypes.contains(aType))
    return MyEdgeTypes[aType];
  
  Events_Error::send("Edge type defined in XML is not implemented!");
  return AnyEdge;
}

bool ModuleBase_ValidatorLinearEdge::isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const
{
  bool aValid = false;

  TypeOfEdge anEdgeType = AnyEdge;
  if (theArguments.size() == 1) {
    std::string anArgument = theArguments.front();
    anEdgeType = edgeType(anArgument);
  }

  ObjectPtr anObject = ModuleBase_WidgetShapeSelector::getObject(theAttribute);
  if (anObject.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aResult.get() != NULL) {
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get() != NULL && aShape->isEdge()) {
        aValid = anEdgeType == AnyEdge;
        if (!aValid) {
          bool isCircle = GeomAPI_Curve(aShape).isCircle();
          aValid = (isCircle && anEdgeType == Circle) ||
                   (!isCircle && anEdgeType == Line);
        }
      }
    }
  }
  else {
    //AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  }
  return aValid;
}
