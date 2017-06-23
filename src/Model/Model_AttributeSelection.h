// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_AttributeSelection_H_
#define Model_AttributeSelection_H_

#include "Model.h"
#include "Model_AttributeReference.h"
#include <ModelAPI_AttributeSelection.h>
#include <TDF_LabelMap.hxx>
#include <TopoDS_Shape.hxx>

class Model_AttributeSelectionList;

/**\class Model_AttributeSelection
 * \ingroup DataModel
 * \brief Attribute that contains reference to the sub-shape of some result, the selected shape.
 */
class Model_AttributeSelection : public ModelAPI_AttributeSelection
{
  Model_AttributeReference myRef;  ///< The reference functionality reusage
  TDF_LabelMap myScope; ///< the map of valid labels for naming selection solving
  /// temporarily storages to avoid keeping in the data structure if not needed
  ResultPtr myTmpContext;
  /// temporarily storages to avoid keeping in the data structure if not needed
  std::shared_ptr<GeomAPI_Shape> myTmpSubShape;
  /// Reference to the partent attribute, if any (to split selection compounds in issue 1799)
  Model_AttributeSelectionList* myParent;
public:
  /// Defines the result and its selected sub-shape
  /// \param theContext object where the sub-shape was selected
  /// \param theSubShape selected sub-shape (if null, the whole context is selected)
  /// \param theTemporarily if it is true, do not store and name the added in the data framework
  ///           (used to remove immideately, without the following updates)
  MODEL_EXPORT virtual void setValue(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily = false);

  /// Reset temporary stored values
  virtual void removeTemporaryValues();

  /// Returns the selected subshape
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> value();

  /// Returns the context of the selection (the whole shape owner)
  MODEL_EXPORT virtual ResultPtr context();

  /// Sets the feature object
  MODEL_EXPORT virtual void setObject(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Updates the underlied selection due to the changes in the referenced objects
  /// \returns false if update is failed
  MODEL_EXPORT virtual bool update();

  /// Returns a textual string of the selection
  /// \param theDefaultValue a name, which is returned if the naming name can not be obtained
  MODEL_EXPORT virtual std::string namingName(const std::string& theDefaultValue = "");

  /// Returns an Id of the selection
  /// NOTE: This method has been added for temporary export of groups towards old GEOM
  /// It should then be removed when a direct use of objects from SHAPER
  /// will be possible from SMESH module of SALOME.
  MODEL_EXPORT virtual int Id();

  /// Defines the sub-shape by Id
  /// NOTE: This method is opposite to Id() method.
  MODEL_EXPORT virtual void setId(int theID);

  /// Selects (i.e. creates Naming data structure) of sub-shape specifed by textual name
  MODEL_EXPORT virtual void selectSubShape(const std::string& theType,
                                           const std::string& theSubShapeName);

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();

  /// Returns true if recomute of selection become impossible
  MODEL_EXPORT virtual bool isInvalid();

  /// Updates the arguments of selection if something was affected by creation
  /// or reorder of features upper in the history line (issue #1757)
  MODEL_EXPORT virtual void updateInHistory();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeSelection(TDF_Label& theLabel);

  /// Performs the selection for the body result (TNaming Selection)

  /// Performs the selection for the body result (TNaming selection)
  virtual void selectBody(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape);

  /// Performs the selection for the part result (selection by name of body result inside of part)
  /// \param theContext the result - owner of the selection
  /// \param theSubShape selected shape
  /// \param theUpdate flag that shows that it must be just update, theShape is null
  /// \returns true if eveything is selected correctly
  virtual bool selectPart(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theUpdate = false);

  /// Returns the label where TNaming_Selection results are stored
  /// Note: there must be no attributes stored at the same label because Selector clears this lab
  TDF_Label selectionLabel();

  /// Returns the prepared map of valid labels for naming selection solving (creates if not exists)
  TDF_LabelMap& scope();

  /// Sets the ID of the attribute in Data (called from Data): here it is used for myRef ID setting
  MODEL_EXPORT virtual void setID(const std::string theID);

  /// Returns the name by context. Adds the part name if the context is located in other document
  std::string contextName(const ResultPtr& theContext) const;

  /// Sets the parent attribute
  void setParent(Model_AttributeSelectionList* theParent);

  /// Splits theNewShape into sub-shapes of theType type (for the list parent of this attribute)
  void split(ResultPtr theContext, TopoDS_Shape theNewShape, TopAbs_ShapeEnum theType);

  friend class Model_Data;
  friend class Model_AttributeSelectionList;
};

#endif
