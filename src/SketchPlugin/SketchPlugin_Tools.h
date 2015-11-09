// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Tools.h
// Created:     07 July 2015
// Author:      Sergey POKHODENKO

#ifndef SKETCHPLUGIN_TOOLS_H_
#define SKETCHPLUGIN_TOOLS_H_

#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>

namespace SketchPlugin_Tools {

/// Clears text expressions for all attributes of the feature 
void clearExpressions(FeaturePtr theFeature);

/// \return coincidence point
/// \param[in] theStartCoin coincidence feature
std::shared_ptr<GeomAPI_Pnt2d> getCoincidencePoint(const FeaturePtr theStartCoin);

/// Finds lines coincident at point
/// \param[in] theStartCoin coincidence feature
/// \param[in] theAttr attribute name
/// \param[out] theList list of lines
void findCoincidences(const FeaturePtr theStartCoin,
                      const std::string& theAttr,
                      std::set<FeaturePtr>& theList);

/// Changes the second attribute value to be multiplied or divided by the given value.
/// \param theFirstAngleAttribute the source attribute
/// \param theSecondAngleAttribute the changed attribute
/// \param theValue a value for modification
/// \param toMultiply a type of modification
void updateMultiAttribute(const AttributePtr& theFirstAngleAttribute,
                          const AttributePtr& theSecondAngleAttribute,
                          const int& theValue,
                          const bool toMultiply);

/// Changes the second attribute value to be multiplied or divided by the given value.
/// \param theFirstAngleAttribute the source attribute
/// \param theSecondAngleAttribute the changed attribute
/// \param theValue a value for modification
/// \param toMultiply a type of modification
void updateMultiAttribute(const AttributePtr& theFirstAttribute,
                          const AttributePtr& theSecondAttribute,
                          const AttributePtr& theModifiedAttribute,
                          const int& theValue,
                          const bool toMultiply);

}; // namespace SketchPlugin_Tools

#endif // SKETCHPLUGIN_TOOLS_H_