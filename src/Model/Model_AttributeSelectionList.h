// File:        Model_AttributeSelectionList.h
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeSelectionList_H_
#define Model_AttributeSelectionList_H_

#include "Model.h"
#include "Model_AttributeSelection.h"
#include <ModelAPI_AttributeSelectionList.h>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <vector>

/**\class Model_AttributeSelectionList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to the sub-shapes with
 * possibility to manage them.
 */

class Model_AttributeSelectionList : public ModelAPI_AttributeSelectionList
{
  Handle(TDataStd_Integer) mySize;  ///< Contains size of this list
  Handle(TDataStd_Real) mySelectionType;  ///< Contains current index, TODO: make it integer, not real
  std::vector<boost::shared_ptr<Model_AttributeSelection> > mySubs; /// the selection attributes
public:
  /// Adds the new reference to the end of the list
  MODEL_EXPORT virtual void append(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape);

  /// Returns the number ofselection attributes in the list
  MODEL_EXPORT virtual int size();

  MODEL_EXPORT virtual int selectionType();

  MODEL_EXPORT virtual void setSelectionType(int);

  /// Returns the attribute selection by the index (zero based)
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_AttributeSelection> value(const int theIndex);

  /// Returns all attributes
  MODEL_EXPORT virtual void clear();

  /// Sets the feature object
  MODEL_EXPORT virtual void setObject(const boost::shared_ptr<ModelAPI_Object>& theObject);

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeSelectionList(TDF_Label& theLabel);
  /// Updates the mySubs if needed since it is not persistent value
  void updateSubs();

  friend class Model_Data;
};

#endif
