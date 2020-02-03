// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_ResultPart_H_
#define ModelAPI_ResultPart_H_

#include "ModelAPI_Result.h"
class GeomAPI_Trsf;

#include <string>

/**\class ModelAPI_ResultPart
 * \ingroup DataModel
 * \brief The Part document, result of a creation of new part feature.
 *
 * This result leaves without feature: no parametricity for this element,
 * only add/remove, undo/redo.
 */
class ModelAPI_ResultPart : public ModelAPI_Result
{
 public:
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Parts";
    return MY_GROUP;
  }

  /// part document reference attribute
  inline static const std::string& DOC_REF()
  {
    static const std::string MY_DOC_REF("PartDocument");
    return MY_DOC_REF;
  }

  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_BODY_COLOR("96,157,255");
    return RESULT_BODY_COLOR;
  }

  /// Returns the part-document of this result
  virtual std::shared_ptr<ModelAPI_Document> partDoc() = 0;

  /// Returns the original part result: for transformation features results this is
  /// the original Part feature result
  virtual std::shared_ptr<ModelAPI_ResultPart> original() = 0;

  /// Sets this document as current and if it is not loaded yet, loads it
  virtual void activate() = 0;

  /// Returns true if document is activated (loaded into the memory)
  virtual bool isActivated() = 0;

  /// Returns the name of the shape inside of the part
  virtual std::string nameInPart(const std::shared_ptr<GeomAPI_Shape>& theShape,
    int& theIndex) = 0;
  /// Updates the selection inside of the part by the selection index
  virtual bool updateInPart(const int theIndex) = 0;

  /// Applies the additional transformation of the part
  virtual void setTrsf(std::shared_ptr<ModelAPI_Result> theThis,
    const std::shared_ptr<GeomAPI_Trsf>& theTransformation) = 0;

  /// Returns the summary transformations of all references to the origin
  virtual std::shared_ptr<GeomAPI_Trsf> summaryTrsf() = 0;

  /// Returns the shape by the name in the part
  virtual std::shared_ptr<GeomAPI_Shape> shapeInPart(
    const std::string& theName, const std::string& theType, int& theIndex) = 0;

  /// Updates the selection inside of the part as a geometrical selection
  virtual bool combineGeometrical(const int theIndex, std::string& theNewName) = 0;

  /// Returns the shape selected in the selection index
  virtual std::shared_ptr<GeomAPI_Shape> selectionValue(const int theIndex) = 0;

  /// Updates the shape-result of the part (called on Part feature execution)
  virtual void updateShape() = 0;

  /// Loading the part from file
  virtual void loadPart() = 0;
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultPart> ResultPartPtr;

#endif
