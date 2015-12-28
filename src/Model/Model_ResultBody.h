// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultBody.h
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultBody_H_
#define Model_ResultBody_H_

#include "Model.h"
#include <ModelAPI_ResultBody.h>
//#include <GeomAlgoAPI_MakeShape.h>
//#include <GeomAPI_DataMapOfShapeShape.h>
//#include <vector>

//class TNaming_Builder;

/**\class Model_ResultBody
 * \ingroup DataModel
 * \brief The body (shape) result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * May provide really huge results, so, working with this kind
 * of result must be optimized.
 */
class Model_ResultBody : public ModelAPI_ResultBody
{
  /// builders that tores the naming history: one per label to allow store several shapes to one 
  /// label; index in vector corresponds to the label tag
  //std::vector<TNaming_Builder*> myBuilders;

  /// Flag that stores the previous state of "concealed": if it is changed,
  /// The event is used to redisplay the body.
  bool myWasConcealed;
public:
  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes();

  /// Returns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Disables the result body: keeps the resulting shape as selection, but erases the underlaying
  /// naming data structure if theFlag if false. Or restores everything on theFlag is true.
  MODEL_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// The compsolid is concealed if at least one of the sub is concealed,
  /// so, sub is Concealed if at least one sub is concealed
  MODEL_EXPORT virtual bool isConcealed();

  /// Returns true if the latest modification of this body in the naming history
  // is equal to the given shape
  MODEL_EXPORT virtual bool isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultBody() {};

protected:
  /// Makes a body on the given feature
  Model_ResultBody();

  friend class Model_Objects;
};

#endif
