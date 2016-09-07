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
#include <map>
#include <set>

namespace ModelAPI_Tools {
/// Returns shape from the given Result object
MODELAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult);

/// Creates a container of shape of the feature results satisfied the given shape type
/// \param theFeature a source feature
/// \param theType shape type
/// \param an output container for shapes
MODELAPI_EXPORT void shapesOfType(const FeaturePtr& theFeature,
                                  const GeomAPI_Shape::ShapeType& theType,
                                  std::set<GeomShapePtr>& theShapes);

/*! Returns the feature error generated according to feature error and exec state
 * \param theFeature a feature
 * \return error value or empty string
 */
MODELAPI_EXPORT std::string getFeatureError(const FeaturePtr& theFeature);

/*!
 * Searches for variable with name \param theName in \param theDocument. 
 * If found, set it value in the \param outValue and returns true.
 * theSearcher must be located later in the history than the found variable.
 */
MODELAPI_EXPORT bool findVariable(const DocumentPtr& theDocument, FeaturePtr theSearcher,
  const std::string& theName, double& outValue, ResultParameterPtr& theParam);

/*!
 * Searches for variable with name \param theName in the active document (Part), when
 * in the root document (PartSet). If found, set it value in the \param outValue
 * and returns true. If \param theDocument is empty active document is used.
 * theSearcher must be located later in the history than the found variable.
 */
MODELAPI_EXPORT bool findVariable(FeaturePtr theSearcher, const std::string& theName,
  double& outValue, ResultParameterPtr& theParam,
  const DocumentPtr& theDocument = DocumentPtr());

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

/*!
 Returns true if there are no parts in the document, which are not activated
 \param theNotActivatedNames out string which contains not activated names
 \return a boolean value
 */
MODELAPI_EXPORT bool allDocumentsActivated(std::string& theNotActivatedNames);


/*! Removes features from the document
* \param theFeatures a list of features to be removed
* \param theFlushRedisplay a boolean value if the redisplay signal should be flushed
* \return true if at least one feature is removed
*/
MODELAPI_EXPORT bool removeFeaturesAndReferences(const std::set<FeaturePtr>& theFeatures,
                                                 const bool theFlushRedisplay = false,
                                                 const bool theUseComposite = false,
                                                 const bool theUseRecursion = true);

/*! Removes features from the document
* \param theFeatures a list of features to be removed
* \param theFlushRedisplay a boolean value if the redisplay signal should be flushed
* \return true if at least one feature is removed
*/
MODELAPI_EXPORT bool removeFeatures(const std::set<FeaturePtr>& theFeatures,
                                    const bool theFlushRedisplay);

/*! Build a map of references for the given set of features
* \param theFeatures a list of features
* \param theReferences a map of all references to the features
* \param theUseComposite state if the composite features of the reference should be in the map
* \param theUseRecursion state if references for features out of the sources feature
         list are put into the result container. E.g. if theFeatures contains "Sketch_2", map will
         contain references to this feature also.
*/
MODELAPI_EXPORT void findAllReferences(const std::set<FeaturePtr>& theFeatures,
                                       std::map<FeaturePtr, std::set<FeaturePtr> >& theReferences,
                                       const bool theUseComposite = true,
                                       const bool theUseRecursion = true);

/*! In the map of references find all features referenced to the source feature
* \param theFeatures a list of features to find references
* \param theReferences a map of all references to the features
* \param theFeaturesRefsTo an out list of referenced features
*/
MODELAPI_EXPORT void findRefsToFeatures(const std::set<FeaturePtr>& aFeatures,
                                        const std::map<FeaturePtr, std::set<FeaturePtr> >& aReferences,
                                        std::set<FeaturePtr>& aFeaturesRefsTo);

/*! Finds referenced of the feature to objects and collects concealed results.
* \param theFeatures a model feature
* \param theResults container for concealed results. It has no duplications
*/
MODELAPI_EXPORT void getConcealedResults(const FeaturePtr& theFeature,
                                         std::list<std::shared_ptr<ModelAPI_Result> >& theResults);
}

#endif
