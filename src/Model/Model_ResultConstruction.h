// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef Model_ResultConstruction_H_
#define Model_ResultConstruction_H_

#include "Model.h"
#include <ModelAPI_ResultConstruction.h>
#include <vector>

class ModelAPI_Document;
class TDF_Label;

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
 public:
  /// Retuns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// By default object is displayed in the object browser.
  MODEL_EXPORT virtual bool isInHistory();
  /// Sets the result
  MODEL_EXPORT virtual void setShape(std::shared_ptr<GeomAPI_Shape> theShape);
  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Sets the flag that it must be displayed in history (default is true)
  MODEL_EXPORT virtual void setIsInHistory(const bool myIsInHistory);

  /// if the construction result may be used as faces, this method returns not zero number of faces
  /// \param theUpdateNaming is false of keeping the naming structure untouched (on load)
  MODEL_EXPORT virtual int facesNum(const bool theUpdateNaming = true);
  /// if the construction result may be used as faces, this method returns face by zero based index
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Face> face(const int theIndex);
  /// Change the order of faces
  MODEL_EXPORT
  virtual void setFacesOrder(const std::list<std::shared_ptr<GeomAPI_Face> >& theFaces);

  /// By default object is not infinite.
  MODEL_EXPORT virtual bool isInfinite();
  /// Sets the flag that it is infinite
  MODEL_EXPORT virtual void setInfinite(const bool theInfinite);
  /// The construction element may be concealed only by "delete" feature
  MODEL_EXPORT virtual void setIsConcealed(const bool theValue, const bool theForced = false);

  /// Updates the shape taking the current value from the data structure, returns true
  /// if update has been correctly done
  MODEL_EXPORT virtual bool updateShape();

 protected:
  /// Makes a body on the given feature
  Model_ResultConstruction();

  /// Stores shape in the data structure to allow identification it by the naming
  void storeShape(std::shared_ptr<GeomAPI_Shape> theShape);

  friend class Model_Objects;
};

#endif
