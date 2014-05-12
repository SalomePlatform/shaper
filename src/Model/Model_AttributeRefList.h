// File:        Model_AttributeRefList.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeRefList_HeaderFile
#define Model_AttributeRefList_HeaderFile

#include "Model.h"
#include "ModelAPI_AttributeRefList.h"
#include <TDataStd_ReferenceList.hxx>

/**\class Model_AttributeRefList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features (located in the same document).
 */

class Model_AttributeRefList : public ModelAPI_AttributeRefList
{
  Handle_TDataStd_ReferenceList myRef; ///< references to the features labels
public:
  /// Appends the feature to the end of a list
  MODEL_EXPORT virtual void append(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Erases the first meet of the feature in the list
  MODEL_EXPORT virtual void remove(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Returns number of features in the list
  MODEL_EXPORT virtual int size();

  /// Returns the list of features
  MODEL_EXPORT virtual std::list<boost::shared_ptr<ModelAPI_Feature> > list();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefList(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
