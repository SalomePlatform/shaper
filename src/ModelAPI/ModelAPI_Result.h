// File:        ModelAPI_Result.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Result_H_
#define ModelAPI_Result_H_

#include "ModelAPI_Object.h"

class ModelAPI_Feature;

/**\class ModelAPI_Result
 * \ingroup DataModel
 * \brief The result of a feature.
 *
 * A generic class that .
 */
class ModelAPI_Result : public ModelAPI_Object
{
  bool myIsConcealed; ///< the result is concealed from the data tree (referenced by other objects)
 public:
   /// Returns true if the result is concealed from the data tree (referenced by other objects)
  inline bool isConcealed() {return myIsConcealed;}

  /// Returns true if the result is concealed from the data tree (referenced by other objects)
  inline void setIsConcealed(const bool theValue) {myIsConcealed = theValue;}

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Result()
  {
  }

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_Result> ResultPtr;

#endif
