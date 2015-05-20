// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Object.hxx
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Object_H_
#define ModelAPI_Object_H_

#include "ModelAPI.h"
#include "ModelAPI_Data.h"

#include <memory>

class ModelAPI_Data;
class ModelAPI_Document;

/**\class ModelAPI_Object
 * \ingroup DataModel
 * \brief Represents any object in the data model and in the object browser.
 *
 * It may be feature or result of the feature. User just may set name for it
 * or change this name later. Generic class for Feature, Body, Parameter and other
 * objects related to the features and their results. Contains attributes of this 
 * object in the "Data".
 */
class ModelAPI_Object
{
  std::shared_ptr<ModelAPI_Data> myData;  ///< manager of the data model of a feature
  std::shared_ptr<ModelAPI_Document> myDoc;  ///< document this object belongs to
 public:
  /// By default object is displayed in the object browser.
  MODELAPI_EXPORT virtual bool isInHistory();

  /// Makes object presented or not in the history of the created objects
  /// \param theObject is shared pointer to "this"
  /// \param theFlag is boolean value: to add or remove from the history
  MODELAPI_EXPORT virtual void setInHistory(
    const std::shared_ptr<ModelAPI_Object> theObject, const bool theFlag);

  /// Returns the data manager of this object: attributes
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Data> data() const;

  /// Returns true if object refers to the same data model instance
  MODELAPI_EXPORT virtual bool isSame(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Returns document this feature belongs to
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Document> document() const;

  /// Returns the group identifier of this object
  virtual std::string groupName() = 0;

  /// Request for initialization of data model of the object: adding all attributes
  virtual void initAttributes() = 0;

  /// Returns the feature is disabled or not.
  virtual bool isDisabled() const = 0;

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  MODELAPI_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Initializes the default states of the object
  MODELAPI_EXPORT ModelAPI_Object();

  /// To use virtuality for destructors
  MODELAPI_EXPORT virtual ~ModelAPI_Object();

  /// Returns true if object must be displayed in the viewer: flag is stored in the
  /// data model, so on undo/redo, open/save or recreation of object by history-playing it keeps
  /// the original state i nthe current transaction.
  MODELAPI_EXPORT virtual bool isDisplayed();

  /// Sets the displayed/hidden state of the object. If it is changed, sends the "redisplay"
  /// signal.
  MODELAPI_EXPORT virtual void setDisplayed(const bool theDisplay);

 protected:
  /// Sets the data manager of an object (document does)
  MODELAPI_EXPORT virtual void setData(std::shared_ptr<ModelAPI_Data> theData);

  /// Sets the data manager of an object (document does)
  MODELAPI_EXPORT virtual void setDoc(std::shared_ptr<ModelAPI_Document> theDoc);

  /// removes all fields from this feature
  MODELAPI_EXPORT virtual void erase();

  friend class Model_Objects;

};

typedef std::shared_ptr<ModelAPI_Object> ObjectPtr;

#endif
