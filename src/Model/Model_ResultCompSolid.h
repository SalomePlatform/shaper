// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultCompSolid.h
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef Model_ResultCompSolid_H_
#define Model_ResultCompSolid_H_

#include "Model.h"
#include <ModelAPI_ResultCompSolid.h>
#include <vector>

/**\class Model_ResultCompSolid
* \ingroup DataModel
* \brief The compsolid (container of body results) result of a feature.
*
* Provides a container of shapes that may be displayed in the viewer.
*/
class Model_ResultCompSolid : public ModelAPI_ResultCompSolid
{
  /// Sub-bodies if this is compsolid: zero base index to subs
  std::vector<std::shared_ptr<ModelAPI_ResultBody> > mySubs;
  /// Flag that stores the previous state of "concealed": if it is changed,
  /// The event must be generated to redisplay this and all subs.
  bool myLastConcealed;


public:

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultCompSolid();

  /// Request for initialization of data model of the object: adding all attributes
  MODEL_EXPORT virtual void initAttributes();

  /// Stores the shape (called by the execution method). Creates sub-results for compsolid.
  MODEL_EXPORT virtual void store(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Stores the generated shape.  Creates sub-results for compsolid.
  MODEL_EXPORT virtual void storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
    const std::shared_ptr<GeomAPI_Shape>& theToShape);

  /// Stores the modified shape.  Creates sub-results for compsolid.
  MODEL_EXPORT virtual void storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const int theDecomposeSolidsTag = 0);

  /// Returns the number of sub-elements
  MODEL_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// Returns the sub-result by zero-base index
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultBody> subResult(const int theIndex,
    bool forTree = false) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  MODEL_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// Returns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Disables the result body: keeps the resulting shape as selection, but erases the underlaying
  /// naming data structure if theFlag if false. Or restores everything on theFlag is true.
  MODEL_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// The compsolid is concealed if at least one of the sub is concealed
  MODEL_EXPORT virtual bool isConcealed();

  /// Sets all subs as concealed in the data tree (referenced by other objects)
  MODEL_EXPORT virtual void setIsConcealed(const bool theValue);

protected:
  /// Makes a body on the given feature
  Model_ResultCompSolid();

  /// Updates the sub-bodies if shape of this object is composite-solid
  void updateSubs(const std::shared_ptr<GeomAPI_Shape>& theThisShape);

  friend class Model_Objects;
};

#endif
