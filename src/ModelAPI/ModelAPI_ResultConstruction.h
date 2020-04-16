// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_ResultConstruction_H_
#define ModelAPI_ResultConstruction_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_Face.h>

#include <string>

/**\class ModelAPI_ResultConstruction
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class ModelAPI_ResultConstruction : public ModelAPI_Result
{
 public:
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Construction";
    return MY_GROUP;
  }

  /// default color for a result construction
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_CONSTRUCTION_COLOR("120,120,120");
    return RESULT_CONSTRUCTION_COLOR;
  }

  /// default deflection for a result construction
  inline static const std::string DEFAULT_DEFLECTION()
  {
    return "0.00001";
  }

  /// Sets the result
  virtual void setShape(std::shared_ptr<GeomAPI_Shape> theShape) = 0;

  /// Updates the shape taking the current value from the data structure, returns true
  /// if update has been correctly done
  virtual bool updateShape() = 0;

  /// Sets the flag that it must be displayed in history (default is true)
  virtual void setIsInHistory(const bool isInHistory) = 0;

  /// if the construction result may be used as faces, this method returns not zero number of faces
  /// \param theUpdateNaming is false of keeping the naming structure untouched (on load)
  virtual int facesNum(const bool theUpdateNaming = true) = 0;
  /// if the construction result may be used as faces, this method returns face by zero based index
  virtual std::shared_ptr<GeomAPI_Face> face(const int theIndex) = 0;
  /// Change the order of faces
  virtual void setFacesOrder(const std::list<std::shared_ptr<GeomAPI_Face> >& theFaces) = 0;

  /// By default object is not infinite.
  virtual bool isInfinite() = 0;
  /// Sets the flag that it is infinite
  virtual void setInfinite(const bool theInfinite) = 0;
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultConstruction> ResultConstructionPtr;

#endif
