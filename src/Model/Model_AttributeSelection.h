// File:        Model_AttributeSelection.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeSelection_H_
#define Model_AttributeSelection_H_

#include "Model.h"
#include "Model_AttributeReference.h"
#include <ModelAPI_AttributeSelection.h>

/**\class Model_AttributeSelection
 * \ingroup DataModel
 * \brief Attribute that contains reference to the sub-shape of some result, the selected shape.
 */

class Model_AttributeSelection : public ModelAPI_AttributeSelection
{
  Model_AttributeReference myRef;  ///< The reference functionality reusage
public:
  /// Defines the result and its selected sub-shape
  MODEL_EXPORT virtual void setValue(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape);

  /// Returns the selected subshape
  MODEL_EXPORT virtual boost::shared_ptr<GeomAPI_Shape> value();

  /// Returns the context of the selection (the whole shape owner)
  MODEL_EXPORT virtual ResultPtr context();

  /// Sets the feature object
  MODEL_EXPORT virtual void setObject(const boost::shared_ptr<ModelAPI_Object>& theObject);

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeSelection(TDF_Label& theLabel);
    /// Performs the selection for the body result (TNaming Selection)

  /// Performs the selection for the body result (TNaming selection)
  virtual void selectBody(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape);
  /// Performs the selection for the construction result (selection by index)
  virtual void selectConstruction(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape);

  friend class Model_Data;
  friend class Model_AttributeSelectionList;
};

#endif
