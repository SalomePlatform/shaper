// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultConstruction.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultConstruction_H_
#define Model_ResultConstruction_H_

#include "Model.h"
#include <ModelAPI_ResultConstruction.h>
#include <vector>

/**\class Model_ResultConstruction
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class Model_ResultConstruction : public ModelAPI_ResultConstruction
{
  std::shared_ptr<ModelAPI_Feature> myOwner;  ///< owner of this result
  std::shared_ptr<GeomAPI_Shape> myShape;  ///< shape of this result created "on the fly"
  bool myFacesUpToDate; ///< is true if faces in myuFaces are computed and up to date
  std::vector<std::shared_ptr<GeomAPI_Face> > myFaces; ///< stores the up to date faces if they exist
  bool myIsInHistory;
  bool myIsInfinite;
 public:
  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes();

  // Retuns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// By default object is displayed in the object browser.
  MODEL_EXPORT virtual bool isInHistory()
  {
    return myIsInHistory;
  }

  /// Sets the result
  MODEL_EXPORT virtual void setShape(std::shared_ptr<GeomAPI_Shape> theShape);
  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Sets the flag that it must be displayed in history (default is true)
  MODEL_EXPORT virtual void setIsInHistory(const bool myIsInHistory);

  /// if the construction result may be used as faces, this method returns not zero number of faces
  MODEL_EXPORT virtual int facesNum();
  /// if the construction result may be used as faces, this method returns face by zero based index
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Face> face(const int theIndex);

  /// By default object is not infinite.
  MODEL_EXPORT virtual bool isInfinite();
  /// Sets the flag that it is infinite
  MODEL_EXPORT virtual void setInfinite(const bool theInfinite);

 protected:
  /// Makes a body on the given feature
  Model_ResultConstruction();

  friend class Model_Objects;
};

#endif
