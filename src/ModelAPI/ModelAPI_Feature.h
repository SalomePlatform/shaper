// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Feature_H_
#define ModelAPI_Feature_H_

#include <ModelAPI.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <memory>

#include <list>
#include <string>

/**\class ModelAPI_Feature
 * \ingroup DataModel
 * \brief Feature function that represents the particular functionality
 * of this operation. Produces results by the arguments.
 */
class ModelAPI_Feature : public ModelAPI_Object
{
  ///< list of current results of this feature
  std::list<std::shared_ptr<ModelAPI_Result> > myResults;
  ///< is feature disabled or not
  bool myIsDisabled;
 public:
  /// Returns the unique kind of a feature (like "Point")
  virtual const std::string& getKind() = 0;

  /// Returns the group identifier of all features
  inline static std::string group()
  {
    static std::string MY_GROUP = "Features";
    return MY_GROUP;
  }

  /// Returns document this feature belongs to
  virtual std::shared_ptr<ModelAPI_Document> document() const
  {
    return ModelAPI_Object::document();
  }

  /// Returns the group identifier of this result
  virtual std::string groupName()
  {
    return group();
  }

  /// Computes or recomputes the results
  virtual void execute() = 0;

  /// Computes the attribute value on the base of other attributes if the value can be computed
  /// \param theAttributeId an attribute index to be computed
  /// \return a boolean value about it is computed
  virtual bool compute(const std::string& theAttributeId) { return false; };

  /// Registers error during the execution, causes the ExecutionFailed state
  virtual void setError(const std::string& theError, bool isSend = true) {
    data()->setError(theError, isSend);
  }

  /// Returns error, arose during the execution
  virtual std::string error() const {
    return data()->error();
  }

  /// returns the current results of the feature
  MODELAPI_EXPORT const std::list<std::shared_ptr<ModelAPI_Result> >& results();
  /// returns the first result in the list or NULL reference
  MODELAPI_EXPORT std::shared_ptr<ModelAPI_Result> firstResult();
  /// returns the last result in the list or NULL reference
  MODELAPI_EXPORT std::shared_ptr<ModelAPI_Result> lastResult();
  /// sets the alone result
  MODELAPI_EXPORT void setResult(const std::shared_ptr<ModelAPI_Result>& theResult);
  /// sets the result by index (zero based), results before this must be set before
  MODELAPI_EXPORT void setResult(const std::shared_ptr<ModelAPI_Result>& theResult,
                                 const int theIndex);
  /// removes the result from the feature
  MODELAPI_EXPORT void removeResult(const std::shared_ptr<ModelAPI_Result>& theResult);
  /// removes all results starting from the gived index (zero-based)
  MODELAPI_EXPORT void removeResults(const int theSinceIndex);
  /// removes all results from the feature
  MODELAPI_EXPORT void eraseResults();
  /// removes all fields from this feature: results, data, etc
  MODELAPI_EXPORT virtual void erase();

  /// Returns true if result is persistent (stored in document) and on undo-redo, save-open
  /// it is not needed to recompute it.
  virtual bool isPersistentResult() {return true;}

  /// Returns true if this feature must not be created: this is just an action
  /// that is not stored in the features history and data model (like "delete part").
  virtual bool isAction()
  {
    return false;
  }

  /// Returns true if this feature is used as macro: creates other features and then removed.
  /// \returns false by default
  MODELAPI_EXPORT virtual bool isMacro() const;

  /// Returns true if preview update during the edition needed. Otherwise the update-mechanism
  /// calls the \a execute function only on "apply" of the operation
  /// \returns true by default
  MODELAPI_EXPORT virtual bool isPreviewNeeded() const;

  /// Must return document where the new feature must be added to
  /// By default it is empty: it is added to the document this method is called to
  MODELAPI_EXPORT virtual const std::string& documentToAdd();

  /// Enables/disables the feature. The disabled feature has no results and does not participate in
  /// any calculation.
  /// \returns true if state is really changed
  MODELAPI_EXPORT virtual bool setDisabled(const bool theFlag);

  /// Returns the feature is disabled or not.
  MODELAPI_EXPORT virtual bool isDisabled() const;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Feature();

  /// Returns the feature by the object (result).
  MODELAPI_EXPORT static std::shared_ptr<ModelAPI_Feature> feature(ObjectPtr theObject);

 //
 // Helper methods, aliases for data()->method()
 // -----------------------------------------------------------------------------------------------
  /// Returns the name stored in the attribute
  inline std::string name()
  {
    return data()->name();
  }
  /// Returns the Boolean attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeBoolean> boolean(const std::string& theID)
  {
    return data()->boolean(theID);
  }
  /// Returns the document reference attribute
  inline std::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID)
  {
    return data()->document(theID);
  }
  /// Returns the real attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID)
  {
    return data()->real(theID);
  }
  /// Returns the integer attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeInteger> integer(const std::string& theID)
  {
    return data()->integer(theID);
  }
  /// Returns the reference attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeRefAttr> refattr(const std::string& theID)
  {
    return data()->refattr(theID);
  }
  /// Returns the reference attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeReference> reference(const std::string& theID)
  {
    return data()->reference(theID);
  }
  /// Returns the list of references attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeRefList> reflist(const std::string& theID)
  {
    return data()->reflist(theID);
  }
  /// Returns the shape selection attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeSelection> selection(const std::string& theID)
  {
    return data()->selection(theID);
  }
  /// Returns the list of shape selections attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeSelectionList> selectionList(const std::string& theID)
  {
    return data()->selectionList(theID);
  }
  /// Returns the string attribute by the identifier
  inline std::shared_ptr<ModelAPI_AttributeString> string(const std::string& theID)
  {
    return data()->string(theID);
  }
  /// Returns the attribute by the identifier
  inline std::shared_ptr<ModelAPI_Attribute> attribute(const std::string& theID)
  {
    return data()->attribute(theID);
  }
 // -----------------------------------------------------------------------------------------------
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_Feature> FeaturePtr;

#endif

