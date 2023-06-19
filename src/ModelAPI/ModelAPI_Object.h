// Copyright (C) 2014-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModelAPI_Object_H_
#define ModelAPI_Object_H_

#include "ModelAPI.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_Entity.h"

#include <memory>

class ModelAPI_Data;
class ModelAPI_Document;

// sometimes it is useful for debug to see name of each object (bad for memory and performance)
//#define DEBUG_NAMES

/**\class ModelAPI_Object
 * \ingroup DataModel
 * \brief Represents any object in the data model and in the object browser.
 *
 * It may be feature or result of the feature. User just may set name for it
 * or change this name later. Generic class for Feature, Body, Parameter and other
 * objects related to the features and their results. Contains attributes of this 
 * object in the "Data".
 */
class ModelAPI_Object: public ModelAPI_Entity
{
  std::shared_ptr<ModelAPI_Data> myData;  ///< manager of the data model of a feature
  std::shared_ptr<ModelAPI_Document> myDoc;  ///< document this object belongs to
 public:
#ifdef DEBUG_NAMES
  std::wstring myName; // name of this object
#endif
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
  virtual bool isDisabled() = 0;

  /// Returns the parameters of color definition in the resources config manager
  virtual void colorConfigInfo(std::string& /*theSection*/,
                               std::string& /*theName*/,
                               std::string& /*theDefault*/) {}

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  MODELAPI_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Initializes the default states of the object
  MODELAPI_EXPORT ModelAPI_Object();

  /// To use virtuality for destructors
  MODELAPI_EXPORT virtual ~ModelAPI_Object();

  /// Returns true if object must be displayed in the viewer: flag is stored in the
  /// data model, so on undo/redo, open/save or recreation of object by history-playing it keeps
  /// the original state in the current transaction.
  MODELAPI_EXPORT virtual bool isDisplayed();

  /// Sets the displayed/hidden state of the object. If it is changed, sends the "redisplay"
  /// signal.
  MODELAPI_EXPORT virtual void setDisplayed(const bool theDisplay);

  MODELAPI_EXPORT virtual bool hasTexture()
  {
    return false;
  }

 protected:
  /// This method is called just after creation of the object: it must initialize
  /// all fields, normally initialized in the constructor
  MODELAPI_EXPORT virtual void init() = 0;

  /// Sets the data manager of an object (document does)
  MODELAPI_EXPORT virtual void setData(std::shared_ptr<ModelAPI_Data> theData);

  /// Sets the data manager of an object (document does)
  MODELAPI_EXPORT virtual void setDoc(std::shared_ptr<ModelAPI_Document> theDoc);

  /// removes all fields from this feature
  MODELAPI_EXPORT virtual void erase();

  friend class Model_Objects;
  friend class ModelAPI_Feature;
  friend class Model_Document;

};

typedef std::shared_ptr<ModelAPI_Object> ObjectPtr;

#endif
