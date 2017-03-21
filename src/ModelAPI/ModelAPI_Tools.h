// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.h
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#ifndef ModelAPI_Tools_HeaderFile
#define ModelAPI_Tools_HeaderFile

#include "ModelAPI.h"

class ModelAPI_CompositeFeature;
class ModelAPI_Document;
class ModelAPI_Feature;
class ModelAPI_Result;
class ModelAPI_ResultParameter;
class ModelAPI_ResultCompSolid;

class GeomAPI_Shape;

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <list>

namespace ModelAPI_Tools {
/// Returns shape from the given Result object
MODELAPI_EXPORT std::shared_ptr<GeomAPI_Shape> shape(
                                   const std::shared_ptr<ModelAPI_Result>& theResult);

/*! Returns the feature error generated according to feature error and exec state
 * \param theFeature a feature
 * \return error value or empty string
 */
MODELAPI_EXPORT std::string getFeatureError(const std::shared_ptr<ModelAPI_Feature>& theFeature);

/*!
 * Searches for variable with name \param theName in \param theDocument. 
 * If found, set it value in the \param outValue and returns true.
 * theSearcher must be located later in the history than the found variable.
 */
MODELAPI_EXPORT bool findVariable(const std::shared_ptr<ModelAPI_Document>& theDocument,
                                  std::shared_ptr<ModelAPI_Feature> theSearcher,
                                  const std::string& theName, double& outValue,
                                  std::shared_ptr<ModelAPI_ResultParameter>& theParam);

/*!
 * Searches for variable with name \param theName in the active document (Part), when
 * in the root document (PartSet). If found, set it value in the \param outValue
 * and returns true. If \param theDocument is empty active document is used.
 * theSearcher must be located later in the history than the found variable.
 */
MODELAPI_EXPORT bool findVariable(std::shared_ptr<ModelAPI_Feature> theSearcher,
  const std::string& theName,
  double& outValue, std::shared_ptr<ModelAPI_ResultParameter>& theParam,
  const std::shared_ptr<ModelAPI_Document>& theDocument = std::shared_ptr<ModelAPI_Document>());

/*!
 * Searches for Part result that contains the reference to the given document.
 * The result must be presented in the tree.
 * \param theMain document that contains the searched feature
 * \param theSub document that is searched, the resulting feature references to it
 * \returns null if not found
 */
MODELAPI_EXPORT std::shared_ptr<ModelAPI_Result> findPartResult(
                                              const std::shared_ptr<ModelAPI_Document>& theMain,
                                              const std::shared_ptr<ModelAPI_Document>& theSub);

/*!
 * Searches for Part the feature that contains in result the reference to the given document.
 * The result may be disabled.
 * \param theMain document that contains the searched feature
 * \param theSub document that is searched, the resulting feature references to it
 * \returns null if not found
 */
MODELAPI_EXPORT std::shared_ptr<ModelAPI_Feature> findPartFeature(
                                           const std::shared_ptr<ModelAPI_Document>& theMain,
                                           const std::shared_ptr<ModelAPI_Document>& theSub);

/*!
 * Returns the composite feature - parent of this feature.
 * \param theFeature the sub-element of composite
 * \returns null if it is not sub-element of composite
 */
MODELAPI_EXPORT std::shared_ptr<ModelAPI_CompositeFeature> compositeOwner(
                                        const std::shared_ptr<ModelAPI_Feature>& theFeature);

/*!
 * Returns the compsolid result - parent of this result.
 * \param theSub the sub-element of comp-solid
 * \returns null if it is not sub-element of composite
 */
MODELAPI_EXPORT std::shared_ptr<ModelAPI_ResultCompSolid> compSolidOwner(
                                            const std::shared_ptr<ModelAPI_Result>& theSub);

/*!
* Returns true if the result contains a not empty list of sub results.
* It processes result compsolid.
* \param theResult a result object
* \returns boolean value
*/
MODELAPI_EXPORT bool hasSubResults(const std::shared_ptr<ModelAPI_Result>& theResult);

/*!
* Adds the results of the given feature to theResults list: including disabled and sub-results
*/
MODELAPI_EXPORT void allResults(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                std::list<std::shared_ptr<ModelAPI_Result> >& theResults);

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
MODELAPI_EXPORT bool removeFeaturesAndReferences(
                       const std::set<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                                                 const bool theFlushRedisplay = false,
                                                 const bool theUseComposite = false,
                                                 const bool theUseRecursion = true);

/*! Removes features from the document
* \param theFeatures a list of features to be removed
* \param theFlushRedisplay a boolean value if the redisplay signal should be flushed
* \return true if at least one feature is removed
*/
MODELAPI_EXPORT bool removeFeatures(
                            const std::set<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                            const bool theFlushRedisplay);

/*! Build a map of references for the given set of features
* \param theFeatures a list of features
* \param theReferences a map of all references to the features
* \param theUseComposite state if the composite features of the reference should be in the map
* \param theUseRecursion state if references for features out of the sources feature
         list are put into the result container. E.g. if theFeatures contains "Sketch_2", map will
         contain references to this feature also.
*/
MODELAPI_EXPORT void findAllReferences(
  const std::set<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
  std::map<std::shared_ptr<ModelAPI_Feature>,
  std::set<std::shared_ptr<ModelAPI_Feature> > >& theReferences,
  const bool theUseComposite = true,
  const bool theUseRecursion = true);

/*! In the map of references find all features referenced to the source feature
* \param theFeatures a list of features to find references
* \param theReferences a map of all references to the features
* \param theFeaturesRefsTo an out list of referenced features
*/
MODELAPI_EXPORT void findRefsToFeatures(
  const std::set<std::shared_ptr<ModelAPI_Feature> >& aFeatures,
  const std::map<std::shared_ptr<ModelAPI_Feature>,
                 std::set<std::shared_ptr<ModelAPI_Feature> > >& aReferences,
  std::set<std::shared_ptr<ModelAPI_Feature> >& aFeaturesRefsTo);

/*! Finds referenced of the feature to objects and collects concealed results.
* \param theFeatures a model feature
* \param theResults container for concealed results. It has no duplications
*/
MODELAPI_EXPORT void getConcealedResults(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   std::list<std::shared_ptr<ModelAPI_Result> >& theResults);
}

#endif
