// File:        Model_Object.hxx
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Object_HeaderFile
#define Model_Object_HeaderFile

#include <Model.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Document.h>

#include <TDataStd_Name.hxx>

/**\class ModelAPI_Object
 * \ingroup DataModel
 * \brief Represents the result of some feature in the object browser
 * under the specific folder. Just a reference to specific feature-operation
 * with possibility to rename it.
 */
class Model_Object : public ModelAPI_Object
{
  FeaturePtr myRef; ///< the referenced feature
  Handle_TDataStd_Name myName; ///< the name of this object that may be changed
public:
  /// Reference to the feature-operation that produces this object
  MODEL_EXPORT virtual FeaturePtr featureRef();

  /// Returns the name of this object (by default equal to the name of feature)
  MODEL_EXPORT virtual std::string getName();

  /// Defines the name of the object
  MODEL_EXPORT virtual void setName(std::string theName);

  /// Returns the kind of a feature (like "Point")
  MODEL_EXPORT virtual const std::string& getKind() {return myRef->getKind();}

  /// Returns to which group in the document must be added feature
  MODEL_EXPORT virtual const std::string& getGroup() {return FEATURES_GROUP;}

  /// Returns document this feature belongs to
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> document()
  {return myRef->document();}

  /// Returns true if feature refers to the same model data instance
  MODEL_EXPORT virtual bool isSame(const FeaturePtr& theFeature)
  {
    boost::shared_ptr<Model_Object> anObj = boost::dynamic_pointer_cast<Model_Object>(theFeature);
    return anObj && myRef == anObj->myRef;
  }

  /// It is just a reference: don't init attributes
  MODEL_EXPORT virtual void initAttributes() {}

  /// It is just a reference: don't execute
  MODEL_EXPORT virtual void execute() {}

private:

  /// Constructor fully defines this object
  Model_Object(FeaturePtr theRef, Handle_TDataStd_Name theName);

  friend class Model_Document;  
};

#endif
