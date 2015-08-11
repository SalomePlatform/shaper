// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultPart.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultPart_H_
#define Model_ResultPart_H_

#include "Model.h"
#include <ModelAPI_ResultPart.h>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>

/**\class Model_ResultPart
 * \ingroup DataModel
 * \brief The Part document, result of a creation of new part feature.
 *
 * This result leaves without feature: no parametricity for this element,
 * only add/remove, undo/redo.
 */
class Model_ResultPart : public ModelAPI_ResultPart
{
  TopoDS_Shape myShape; ///< shape of this part created from bodies (updated only of Part deactivation)
  std::shared_ptr<gp_Trsf> myTrsf; ///< if it is just copy of original shape, keep just transformation
 public:

  /// the reference to the base result document, may be null if this is the root, others make sequence of references
  inline static const std::string& BASE_REF_ID()
  {
    static const std::string MY_BASE_REF("BaseReference");
    return MY_BASE_REF;
  }
  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes();
  /// Returns the part-document of this result
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> partDoc();

  /// Returns the original part result: for transfomration features results this is 
  /// the original Part feature result
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultPart> original();

  /// Sets this document as current and if it is not loaded yet, loads it
  MODEL_EXPORT virtual void activate();

  /// disable all feature of the part on disable of the part result
  MODEL_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// Result shape of part document is compound of bodies inside of this part
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Returns the name of the shape inside of the part
  /// \param theShape selected shape in this document
  /// \param theIndex is returned as one-based index if selection was required, "0" otherwise
  /// \returns empty name is selection is not correct
  MODEL_EXPORT virtual std::string nameInPart(const std::shared_ptr<GeomAPI_Shape>& theShape,
    int& theIndex);

  /// Updates the selection inside of the part by the selection index
  MODEL_EXPORT virtual bool updateInPart(const int theIndex);
  /// Returns the shape by the name in the part
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shapeInPart(const std::string& theName);
  /// Updates the shape-result of the part (called on Part feature execution)
  MODEL_EXPORT virtual void updateShape();
  /// Applies the additional transformation of the part
  MODEL_EXPORT virtual void setTrsf(std::shared_ptr<ModelAPI_Result> theThis, 
    const std::shared_ptr<GeomAPI_Trsf>& theTransformation);

  /// Returns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

protected:
  /// makes a result on a temporary feature (an action)
  Model_ResultPart();

  /// Returns true if document is activated (loaded into the memory)
  virtual bool isActivated();

  friend class Model_Objects;
};

#endif
