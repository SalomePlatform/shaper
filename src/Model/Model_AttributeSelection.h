// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef Model_AttributeSelection_H_
#define Model_AttributeSelection_H_

#include "Model.h"
#include "Model_AttributeReference.h"
#include <ModelAPI_AttributeSelection.h>
#include <Selector_NameGenerator.h>
#include <TDF_LabelMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

class Model_AttributeSelectionList;
class Model_Document;

/**\class Model_AttributeSelection
 * \ingroup DataModel
 * \brief Attribute that contains reference to the sub-shape of some result, the selected shape.
 */
class Model_AttributeSelection : public ModelAPI_AttributeSelection,
                                 public Selector_NameGenerator
{
  Model_AttributeReference myRef;  ///< The reference functionality re-usage
  /// temporarily storages to avoid keeping in the data structure if not needed
  ObjectPtr myTmpContext;
  /// temporarily storages to avoid keeping in the data structure if not needed
  std::shared_ptr<GeomAPI_Shape> myTmpSubShape;
  /// temporarily storages to avoid keeping in the data structure if not needed
  CenterType myTmpCenterType;
  /// Reference to the parent attribute, if any (to split selection compounds in issue 1799)
  Model_AttributeSelectionList* myParent;

  /// current document to restore by name
  std::shared_ptr<Model_Document> myRestoreDocument;
  /// If true attribute selects geometry instead of shape.
  bool myIsGeometricalSelection;

public:
  /// Defines the result and its selected sub-shape
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immediately, without the following updates)
  /// \returns true if attribute was updated
  MODEL_EXPORT virtual bool setValue(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false);

  /// Same as SetValue, but it takes an edge (on circular or elliptic curve)
  /// and stores the vertex of the central point (for ellipse the first or the second focus point)
  MODEL_EXPORT virtual void setValueCenter(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Edge>& theEdge,
    const CenterType theCenterType,
    const bool theTemporarily = false);

  /// Makes this selection attribute selects the same as in theSource selection
  MODEL_EXPORT virtual void selectValue(
    const std::shared_ptr<ModelAPI_AttributeSelection>& theSource);

  /// Reset temporary stored values
  virtual void removeTemporaryValues();

  /// Returns the selected subshape
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> value();

  /// Returns the context of the selection (the whole shape owner)
  MODEL_EXPORT virtual ResultPtr context();

  /// Returns the context of the selection if the whole feature was selected
  MODEL_EXPORT virtual FeaturePtr contextFeature();
  /// Returns the context of the selection : result or feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Object> contextObject();

  /// Sets the feature object
  MODEL_EXPORT virtual void setObject(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Updates the selection due to the changes in the referenced objects
  /// \returns false if update is failed
  MODEL_EXPORT virtual bool update();

  /// Returns a textual string of the selection
  /// \param theDefaultValue a name, which is returned if the naming name can not be obtained
  MODEL_EXPORT virtual std::string namingName(const std::string& theDefaultValue = "");

  /// Defines the sub-shape by Id
  MODEL_EXPORT virtual void setId(int theID);

  /// Selects (i.e. creates Naming data structure) of sub-shape specified by textual name
  MODEL_EXPORT virtual void selectSubShape(const std::string& theType,
                                           const std::string& theSubShapeName);

  /// Selects sub-shape by its inner point
  MODEL_EXPORT virtual void selectSubShape(const std::string& theType,
                                           const std::shared_ptr<GeomAPI_Pnt>& thePoint);
  /// Selects sub-shape by weak naming index
  MODEL_EXPORT virtual void selectSubShape(const std::string& theType,
    const std::string& theContextName, const int theIndex);

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();

  /// Returns true if recompute of selection become impossible
  MODEL_EXPORT virtual bool isInvalid();

  /// Updates the arguments of selection if something was affected by creation
  /// or reorder of features upper in the history line (issue #1757)
  /// Returns theRemove true if this attribute must be removed (become deleted)
  MODEL_EXPORT virtual void updateInHistory(bool& theRemove);

  // Implementation of the name generator method from the Selector package
  // This method returns the context name by the label of the sub-selected shape
  MODEL_EXPORT virtual std::string contextName(const TDF_Label theSelectionLab) override;

  /// This method restores by the context and value name the context label and
  /// sub-label where the value is. Returns true if it is valid.
  MODEL_EXPORT virtual bool restoreContext(std::string theName,
    TDF_Label& theContext, TDF_Label& theValue) override;

  /// Returns true if the first result is newer than the second one in the tree of features
  MODEL_EXPORT virtual bool isLater(const TDF_Label theResult1, const TDF_Label theResult2)
    const override;

  /// Returns the name by context. Adds the part name if the context is located in other document
  MODEL_EXPORT virtual std::string contextName(const ResultPtr& theContext) const;

  /// Makes the current local selection becomes all sub-shapes with same base geometry.
  MODEL_EXPORT virtual void combineGeometrical();

  /// Resets attribute to deafult state
  MODEL_EXPORT virtual void reset();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeSelection(TDF_Label& theLabel);

  /// Returns the selected subshape, internal method that works without knowledge
  /// about special selection of circle and ellipse focuses (for that the public value
  /// method calls this and makes additional processing).
  /// Returns theType type of the center, or NOT_CENTER if it is not.
  std::shared_ptr<GeomAPI_Shape> internalValue(CenterType& theType);

  /// Performs the selection for the body result (TNaming selection)
  virtual void selectBody(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape);

  /// Performs the selection for the part result (selection by name of body result inside of part)
  /// \param theContext the result - owner of the selection
  /// \param theSubShape selected shape
  /// \param theUpdate flag that shows that it must be just update, theShape is null
  /// \returns true if everything is selected correctly
  virtual bool selectPart(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theUpdate = false);

  /// Returns the label where TNaming_Selection results are stored
  /// Note: there must be no attributes stored at the same label because Selector clears this lab
  TDF_Label selectionLabel();

  /// Sets the ID of the attribute in Data (called from Data): here it is used for myRef ID setting
  MODEL_EXPORT virtual void setID(const std::string theID);

  /// Sets the parent attribute
  void setParent(Model_AttributeSelectionList* theParent);

  /// Splits theNewShape into sub-shapes of theType type (for the list parent of this attribute)
  void split(ResultPtr theContext, TopoDS_Shape theNewShape, TopAbs_ShapeEnum theType);

  /// When group position is updated, searches the new context and new values
  bool searchNewContext(std::shared_ptr<Model_Document> theDoc, const TopoDS_Shape theContShape,
                        ResultPtr theContext, TopoDS_Shape theValShape, TDF_Label theAccessLabel,
                        std::list<ResultPtr>& theResults, TopTools_ListOfShape& theValShapes);

  /// Searches for the newest context, modification of the current, that contains theValue
  ResultPtr newestContext(
    const ResultPtr theCurrent, const std::shared_ptr<GeomAPI_Shape> theValue);

  /// computes theShapes list - shapes that were generated/modified/deleted the theValShape
  /// during creation from new to old context
  void computeValues(ResultPtr theOldContext, ResultPtr theNewContext, TopoDS_Shape theValShape,
    TopTools_ListOfShape& theShapes);

  /// Returns true if is geometrical selection.
  virtual bool isGeometricalSelection() const {
    return myIsGeometricalSelection;
  };

  /// Returns the module document label if this selection attribute is not in this document.
  /// Returns null label otherwise.
  TDF_Label baseDocumentLab();

  /// Returns features that conceals theFeature and located in history before theStop
  /// theResultOfFeature if not null defines exact referenced result of a feature
  void concealedFeature(
    const FeaturePtr theFeature, const FeaturePtr theStop, const bool theCheckCopy,
    std::list<FeaturePtr>& theConcealers, const ResultPtr theResultOfFeature);

  friend class Model_Data;
  friend class Model_AttributeSelectionList;
};

#endif
