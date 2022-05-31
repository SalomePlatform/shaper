// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_Document_H_
#define ModelAPI_Document_H_

#include "ModelAPI.h"
#include "ModelAPI_Entity.h"

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <set>

class ModelAPI_Feature;
class ModelAPI_Folder;
class ModelAPI_Object;
class ModelAPI_Result;
class ModelAPI_ResultConstruction;
class ModelAPI_ResultBody;
class ModelAPI_ResultPart;
class ModelAPI_ResultGroup;
class ModelAPI_ResultField;
class ModelAPI_ResultParameter;
class ModelAPI_Data;
class GeomAPI_Shape;

/**\class ModelAPI_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage.
 * Document contains all data that must be stored/retrieved in the file.
 * Also it provides access to this data: open/save, transactions management etc.
 */
class ModelAPI_Document: public ModelAPI_Entity
{
public:
  //! Returns the kind of the document: "PartSet", "Part", or something else.
  //! This kind is used for feature buttons enable/disable depending on active document
  //! (it uses workbench "document" identifier in XML configuration file for this)
  virtual const std::string& kind() const = 0;

  //! Removes document data
  //! \param theForever if it is false, document is just hidden
  //!                   (to keep possibility make it back on Undo/Redo)
  virtual void close(const bool theForever = false) = 0;

  //! Adds to the document the new feature of the given feature id
  //! \param theID creates feature and puts it in the document (if it is not action)
  //! \param theMakeCurrent to make current this new feature in this document
  virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID,
    const bool theMakeCurrent = true) = 0;

  //! Return a list of features, which refers to the feature
  //! \param theFeature a feature
  //! \param theRefs a list of features
  //! \param isSendError a flag whether the error message should be send
  virtual void refsToFeature(std::shared_ptr<ModelAPI_Feature> theFeature,
                             std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs,
                             const bool isSendError = true) = 0;

  //! Removes the feature from the document
  //! \param theFeature a feature to be removed
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature) = 0;

  //! Moves the feature to make it after the given one in the history.
  virtual void moveFeature(std::shared_ptr<ModelAPI_Feature> theMoved,
                           std::shared_ptr<ModelAPI_Feature> theAfterThis,
                           const bool theSplit = false) = 0;

  ///! Returns the id of the document
  virtual const int id() const = 0;

  //! Returns the object in the group by the index (started from zero)
  //! \param theGroupID group that contains an object
  //! \param theIndex zero-based index of feature in the group
  //! \param theAllowFolder take into account grouping feature by folders
  virtual std::shared_ptr<ModelAPI_Object> object(const std::string& theGroupID,
                                                  const int theIndex,
                                                  const bool theAllowFolder = false) = 0;

  //! Returns the first found object in the group by the object name
  //! \param theGroupID group that contains an object
  //! \param theName name of the object to search
  //! \returns null if such object is not found
  virtual std::shared_ptr<ModelAPI_Object> objectByName(const std::string& theGroupID,
                                                    const std::wstring& theName) = 0;

  //! Returns the object index in the group. Object must be visible. Otherwise returns -1.
  //! \param theObject object of this document
  //! \param theAllowFolder take into account grouping feature by folders
  //! \returns index started from zero, or -1 if object is invisible or belongs to another document
  virtual const int index(std::shared_ptr<ModelAPI_Object> theObject,
                          const bool theAllowFolder = false) = 0;

  //! Returns the number of objects in the group of objects
  //! \param theGroupID group of objects
  //! \param theAllowFolder take into account grouping feature by folders
  virtual int size(const std::string& theGroupID, const bool theAllowFolder = false) = 0;

  //! Returns the parent object of this child. This may be result or feature, parent of a
  //! top result. Fast method, that uses internal data structure specifics.
  virtual std::shared_ptr<ModelAPI_Object> parent(
    const std::shared_ptr<ModelAPI_Object> theChild) = 0;

  //! Returns the feature that is currently edited in this document, normally
  //! this is the latest created feature
  //! \param theVisible use visible features only: flag is true for Object Browser functionality
  //! \returns null if next created feature must be the first
  virtual std::shared_ptr<ModelAPI_Feature> currentFeature(const bool theVisible) = 0;

  //! Sets the current feature: all features below will be disabled, new features
  //! will be appended after this one. This method does not flushes the events appeared:
  //! it will be done by the finishOperation, or direct flushes
  //! \param theCurrent the selected feature as current: blow it everything become disabled
  //! \param theVisible use visible features only: flag is true for Object Browser functionality
  virtual void setCurrentFeature(std::shared_ptr<ModelAPI_Feature> theCurrent,
    const bool theVisible) = 0;
  //! Makes the current feature one feature upper
  virtual void setCurrentFeatureUp() = 0;

  //! Returns the number of all features: in the history or not
  virtual int numInternalFeatures() = 0;
  //! Returns the feature by zero-based index: features in the history or not
  virtual std::shared_ptr<ModelAPI_Feature> internalFeature(const int theIndex) = 0;
  //! Performs synchronization of transactions with the module document:
  //! If some document is not active (by undo of activation) but in memory,
  //! on activation the transactions must be synchronized because all redo-s performed
  //! without this participation
  virtual void synchronizeTransactions() = 0;

  //! To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Document();

  //! Creates a construction result
  virtual std::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  //! Creates a body result
  virtual std::shared_ptr<ModelAPI_ResultBody> createBody(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  //! Creates a part result
  virtual std::shared_ptr<ModelAPI_ResultPart> createPart(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  //! Copies a part result, keeping the reference to the origin
  virtual std::shared_ptr<ModelAPI_ResultPart> copyPart(
      const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  //! Creates a group result
  virtual std::shared_ptr<ModelAPI_ResultGroup> createGroup(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  //! Creates a field result
  virtual std::shared_ptr<ModelAPI_ResultField> createField(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  //! Creates a parameter result
  virtual std::shared_ptr<ModelAPI_ResultParameter> createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;

  //! Returns a feature by result (owner of result)
  virtual std::shared_ptr<ModelAPI_Feature> feature(
      const std::shared_ptr<ModelAPI_Result>& theResult) = 0;

  //! Returns all features of the document including the hidden features which are not in
  //! history. Not very fast method, for calling once, not in big cycles.
  virtual std::list<std::shared_ptr<ModelAPI_Feature> > allFeatures() = 0;

  //! Returns all objects of the document including the hidden features which are not in
  //! history. Not very fast method, for calling once, not in big cycles.
  virtual std::list<std::shared_ptr<ModelAPI_Object> > allObjects() = 0;

  //! Creates a folder (group of the features in the object browser)
  //! \param theAddBefore a feature, the folder is added before
  //!                     (if empty, the folder is added after the last feature)
  virtual std::shared_ptr<ModelAPI_Folder> addFolder(
      std::shared_ptr<ModelAPI_Feature> theAddBefore = std::shared_ptr<ModelAPI_Feature>()) = 0;
  //! Removes the folder from the document (all features in the folder will be kept).
  virtual void removeFolder(std::shared_ptr<ModelAPI_Folder> theFolder) = 0;
  //! Search a folder above the list of features applicable to store them
  //! (it means the list of features stored in the folder should be consequential)
  //! \return Empty pointer if there is no applicable folder
  virtual std::shared_ptr<ModelAPI_Folder> findFolderAbove(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures) = 0;
  //! Search a folder below the list of features applicable to store them
  //! (it means the list of features stored in the folder should be consequential)
  //! \return Empty pointer if there is no applicable folder
  virtual std::shared_ptr<ModelAPI_Folder> findFolderBelow(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures) = 0;
  //! Search a folder containing the given feature.
  //! Additionally calculates a zero-based index of the feature in this folder.
  //! \param theFeature feature to search
  //! \param theIndexInFolder zero-based index in the folder or -1 if the feature is top-level.
  //! \return the folder containing the feature or empty pointer if the feature is top-level.
  virtual std::shared_ptr<ModelAPI_Folder> findContainingFolder(
      const std::shared_ptr<ModelAPI_Feature>& theFeature,
      int& theIndexInFolder) = 0;
  //! Add a list of features to the folder. The correctness of the adding is not performed
  //! (such checks have been done in corresponding find.. method).
  //! \return \c true if the movement is successful
  virtual bool moveToFolder(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                            const std::shared_ptr<ModelAPI_Folder>& theFolder) = 0;
  //! Remove features from the folder
  //! \param theFeatures list of features to be removed
  //! \param theBefore   extract features before the folder (this parameter is applicable only
  //!                    when all features in the folder are taking out,
  //!                    in other cases the direction is taken automatically)
  //! \return \c true if the features have been moved out
  virtual bool removeFromFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const bool theBefore = true) = 0;

  //! Informs the document that it becomes active and some actions must be performed
  virtual void setActive(const bool theFlag) = 0;
  //! Returns true if this document is currently active
  virtual bool isActive() const = 0;

  /// Returns true if document is opened and valid
  virtual bool isOpened() = 0;

  /// Returns the feature that produced the given face of the given result.
  virtual std::shared_ptr<ModelAPI_Feature> producedByFeature(
    std::shared_ptr<ModelAPI_Result> theResult,
    const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;

  /// Returns true if theLater is in history of features creation later than theCurrent
  virtual bool isLater(std::shared_ptr<ModelAPI_Feature> theLater,
                       std::shared_ptr<ModelAPI_Feature> theCurrent) const = 0;

  //! Internally makes document know that feature was removed or added in history after creation
  MODELAPI_EXPORT virtual void updateHistory(const std::string theGroup) = 0;

  /// Stores in the document boolean flags: states of the nodes in the object browser.
  /// Normally is called outside of the transaction, just before "save".
  MODELAPI_EXPORT virtual void storeNodesState(const std::list<bool>& theStates) = 0;

  /// Returns the stored nodes states. Normally it is calls just after "open".
  /// Appends the values to theStates list.
  MODELAPI_EXPORT virtual void restoreNodesState(std::list<bool>& theStates) const = 0;

  /// Just removes all features without touching the document data (to be able undo)
  MODELAPI_EXPORT virtual void eraseAllFeatures() = 0;

  /// Returns the next (from the history point of view) feature, any: invisible or disabled
  /// \param theCurrent previous to the resulting feature
  /// \param theReverse if it is true, iterates in reversed order (next becomes previous)
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Feature> nextFeature(
    std::shared_ptr<ModelAPI_Feature> theCurrent, const bool theReverse = false) const = 0;

  /// Loads the OCAF document from the file into the current document.
  /// All the features are added after the active feature.
  /// \param theFileName name of the file to import
  /// \param theImported list of features imported from the file
  /// \param theCheckOnly verify the document does not contain unappropriate features
  ///                     (useful for import to PartSet), but do not import it
  /// \returns true if file was loaded successfully
  MODELAPI_EXPORT virtual bool importPart(const char* theFileName,
                                      std::list<std::shared_ptr<ModelAPI_Feature> >& theImported,
                                      bool theCheckOnly = false) = 0;

  /// Export the list of features to the file
  /// \param theFilename path to save the file
  /// \param theExportFeatures list of features to export
  MODELAPI_EXPORT virtual bool save(const char* theFilename,
    const std::list<std::shared_ptr<ModelAPI_Feature> >& theExportFeatures) const = 0;

protected:
  //! Only for SWIG wrapping it is here
  MODELAPI_EXPORT ModelAPI_Document();

  //! Internally makes document know that feature was removed or added in history after creation
  MODELAPI_EXPORT virtual void updateHistory(const std::shared_ptr<ModelAPI_Object> theObject) = 0;

  friend class ModelAPI_Object; // to add or remove from the history
  friend class ModelAPI_Result; // to add or remove from the history
};

//! Pointer on document object
typedef std::shared_ptr<ModelAPI_Document> DocumentPtr;

#endif
