// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.h
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#ifndef ModelAPI_Tools_HeaderFile
#define ModelAPI_Tools_HeaderFile

#include "ModelAPI.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultCompSolid.h>

#include <GeomAPI_Shape.h>

#include <vector>

namespace ModelAPI_Tools {
/// Returns shape from the given Result object
MODELAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult);

/*! Returns the feature error generated according to feature error and exec state
 * \param theFeature a feature
 * \return error value or empty string
 */
MODELAPI_EXPORT std::string getFeatureError(const FeaturePtr& theFeature);

/*!
 * Searches for variable with name \param theName in \param theDocument. 
 * If found, set it value in the \param outValue and returns true.
 */
MODELAPI_EXPORT bool findVariable(const DocumentPtr& theDocument, 
                                  const std::string& theName, double& outValue, ResultParameterPtr& theParam);

/*!
 * Searches for variable with name \param theName in the active document (Part), when
 * in the root document (PartSet). If found, set it value in the \param outValue
 * and returns true. If \param theDocument is empty active document is used.
 */
MODELAPI_EXPORT bool findVariable(const std::string& theName, double& outValue, ResultParameterPtr& theParam,
                                  const DocumentPtr& theDocument = DocumentPtr());

/*!
 * Returns the values of the next random color. The values are in range [0, 255]
 * \param theValues a container of component of RGB value: red, green, blue
 */
MODELAPI_EXPORT void findRandomColor(std::vector<int>& theValues);

/*!
 * Searches for Part result that contains the reference to the given document.
 * The result must be presented in the tree.
 * \param theMain document that contains the searched feature
 * \param theSub document that is searched, the resulting feature references to it
 * \returns null if not found
 */
MODELAPI_EXPORT ResultPtr findPartResult(const DocumentPtr& theMain, const DocumentPtr& theSub);

/*!
 * Searches for Part the feature that contains in result the reference to the given document.
 * The result may be disabled.
 * \param theMain document that contains the searched feature
 * \param theSub document that is searched, the resulting feature references to it
 * \returns null if not found
 */
MODELAPI_EXPORT FeaturePtr findPartFeature(const DocumentPtr& theMain, const DocumentPtr& theSub);

/*!
 * Returns the composite feature - parent of this feature.
 * \param theFeature the sub-element of composite
 * \returns null if it is not sub-element of composite
 */
MODELAPI_EXPORT CompositeFeaturePtr compositeOwner(const FeaturePtr& theFeature);

/*!
 * Returns the compsolid result - parent of this result.
 * \param theSub the sub-element of comp-solid
 * \returns null if it is not sub-element of composite
 */
MODELAPI_EXPORT ResultCompSolidPtr compSolidOwner(const ResultPtr& theSub);

/*!
* Returns true if the result contains a not empty list of sub results. It processes result compsolid.
* \param theResult a result object
* \returns boolean value
*/
MODELAPI_EXPORT bool hasSubResults(const ResultPtr& theResult);

/*!
* Adds the results of the given feature to theResults list: including disabled and sub-results
*/
MODELAPI_EXPORT void allResults(const FeaturePtr& theFeature, std::list<ResultPtr>& theResults);

}

#endif
