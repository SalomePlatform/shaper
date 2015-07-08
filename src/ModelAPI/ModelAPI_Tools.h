// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.h
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#ifndef ModelAPI_Tools_HeaderFile
#define ModelAPI_Tools_HeaderFile

#include "ModelAPI.h"
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>
#include <ModelAPI_CompositeFeature.h>

#include <vector>

namespace ModelAPI_Tools {
/// Returns shape from the given Result object
MODELAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult);

/*!
 * Searches for variable with name \param theName in the active document (Part), when
 * in the root document (PartSet). If found, set it value in the \param outValue
 * and returns true.
 */
MODELAPI_EXPORT bool findVariable(const std::string& theName, double& outValue, 
  ResultParameterPtr& theParam);

/*!
 * Returns the values of the next random color. The values are in range [0, 255]
 * \param theValues a container of component of RGB value: red, green, blue
 */
MODELAPI_EXPORT void findRandomColor(std::vector<int>& theValues);

/*!
 * Searches for Part result that contains the reference to the given document.
 * \param theMain document that contains the searched feature
 * \param theSub document that is searched, the resulting feature references to it
 * \returns numm if not found
 */
MODELAPI_EXPORT ResultPtr findPartResult(const DocumentPtr& theMain, const DocumentPtr& theSub);

/*!
 * Returns the cpomposite feature - parent of this feature.
 * \param theFeature the sub-element of composite
 * \returns null if it is not sub-element of composite
 */
MODELAPI_EXPORT CompositeFeaturePtr compositeOwner(const FeaturePtr& theFeature);

}

#endif
