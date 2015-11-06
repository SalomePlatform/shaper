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

/// Changes the second attribute value to full or single angle. If it is corrected to full
/// value, the first angle is miltipied to the number of copies otherwise it is separated by it.
/// \param theFirstAngleAttribute the source attribute
/// \param theSecondAngleAttribute the changed attribute
/// \param theNumberOfCopies a value for modification
/// \param toFullAngle a type of modification
void updateAngleAttribute(const AttributePtr& theFirstAngleAttribute,
                          const AttributePtr& theSecondAngleAttribute,
                          const int& theNumberOfCopies,
                          const bool toFullAngle);

}; // namespace SketchPlugin_Tools

#endif // SKETCHPLUGIN_TOOLS_H_