// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Different.cpp
// Created:     08 July 2015
// Author:      Sergey POKHODENKO

#include <GeomValidators_Different.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

#include <algorithm>
#include <map>
#include <list>

const double tolerance = 1e-7;

//=================================================================================================
/* Help
To extend GeomValidators_Different validator with new attribute types:
1. Modify function isEqualAttributes
2. Create new implementation of isEqual() for the new type
*/
//=================================================================================================

bool isEqual(const AttributePoint2DPtr& theLeft, const AttributePoint2DPtr& theRight)
{
  return theLeft->pnt()->distance(theRight->pnt()) < tolerance;
}

bool isEqualAttributes(const AttributePtr& theLeft, const AttributePtr& theRight)
{
  if (theLeft->attributeType() == GeomDataAPI_Point2D::typeId() && 
      theRight->attributeType() == GeomDataAPI_Point2D::typeId())
    return isEqual(std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theLeft),
                   std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theRight));
  return false;
}

/** \class IsEqual
 *  \ingroup Validators
 *  \brief Auxiliary class used in std::find_if
 */
class IsEqual {
  AttributePtr myAttribute;
public:
  /// Constructor
  IsEqual(const AttributePtr& theAttribute) : myAttribute(theAttribute) {}
  /// \return true in case if AttributePtr is equal with myAttribute
  bool operator()(const AttributePtr& theAttribute) {
    return isEqualAttributes(myAttribute, theAttribute);
  }
};

bool GeomValidators_Different::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::list<std::string>& theArguments,
                                       std::string& theError) const
{
  std::map<std::string, std::list<AttributePtr> > anAttributesMap;
  // For all attributes referred by theArguments 
  // sort it using attributeType() and store into anAttributesMap 
  std::list<std::string>::const_iterator anArgumentIt = theArguments.begin();
  for (; anArgumentIt != theArguments.end(); ++anArgumentIt) {
    AttributePtr anAttribute = theFeature->attribute(*anArgumentIt);
    anAttributesMap[anAttribute->attributeType()].push_back(anAttribute);
  }

  // Search differences inside each attribute list
  std::map<std::string, std::list<AttributePtr> >::const_iterator anAttributesMapIt = anAttributesMap.begin();
  for (; anAttributesMapIt != anAttributesMap.end(); ++anAttributesMapIt) {
    const std::list<AttributePtr>& anAttributes = anAttributesMapIt->second;
    // for the list of attributes check that all elements are unique
    std::list<AttributePtr>::const_iterator anAttributeIt = anAttributes.begin();
    if (anAttributeIt != anAttributes.end()) {
      std::list<AttributePtr>::const_iterator aNextIt = anAttributeIt; ++aNextIt;
      while (aNextIt != anAttributes.end()) {
        // if equal attribute is found then all attributes are not different
        std::list<AttributePtr>::const_iterator aFindIt =
            std::find_if(aNextIt, anAttributes.end(), IsEqual(*anAttributeIt));
        if (aFindIt != anAttributes.end()) {
          theError = "Attributes " + (*anAttributeIt)->id() + " and " + (*aFindIt)->id() + " are equal." ;
          return false;
        }
        ++anAttributeIt;
        ++aNextIt; 
      }
    }
  }

  return true;
}

bool GeomValidators_Different::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return true;
}
