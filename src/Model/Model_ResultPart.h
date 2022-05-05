// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
  /// shape of this part created from bodies (updated only of Part deactivation)
  TopoDS_Shape myShape;
  /// if it is just copy of original shape, keep just transformation
  std::shared_ptr<gp_Trsf> myTrsf;
 public:

  /// the reference to the base result document, may be null if this is the root,
  /// others make sequence of references
  inline static const std::string& BASE_REF_ID()
  {
    static const std::string MY_BASE_REF("BaseReference");
    return MY_BASE_REF;
  }
  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes();
  /// Returns the part-document of this result
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> partDoc();

  /// Returns the original part result: for transformation features results this is
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
  MODEL_EXPORT virtual std::wstring nameInPart(const std::shared_ptr<GeomAPI_Shape>& theShape,
    int& theIndex);

  /// Updates the selection inside of the part by the selection index
  MODEL_EXPORT virtual bool updateInPart(const int theIndex);
  /// Returns the shape by the name in the part
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shapeInPart(
    const std::wstring& theName, const std::string& theType, int& theIndex);
  /// Updates the selection inside of the part as a geometrical selection
  MODEL_EXPORT virtual bool combineGeometrical(const int theIndex, std::wstring& theNewName);
  /// Updates the shape-result of the part (called on Part feature execution)
  MODEL_EXPORT virtual void updateShape();
  /// Applies the additional transformation of the part
  MODEL_EXPORT virtual void setTrsf(std::shared_ptr<ModelAPI_Result> theThis,
    const std::shared_ptr<GeomAPI_Trsf>& theTransformation);
  /// Returns the summary transformations of all references to the origin
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Trsf> summaryTrsf();

  /// Returns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Returns the shape selected in the selection index
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> selectionValue(const int theIndex);

  /// Loading the part from file
  MODEL_EXPORT virtual void loadPart();

protected:
  /// makes a result on a temporary feature (an action)
  Model_ResultPart();

  /// returns sum of transformations of the whole sequence of transformation-parts
  gp_Trsf sumTrsf();

  /// Returns true if document is activated (loaded into the memory)
  virtual bool isActivated();

  friend class Model_Objects;
};

#endif
