// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
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
  bool myFacesUpToDate; ///< is true if faces in myuFaces are computed and up to date
  /// stores the up to date faces if they exist
  std::vector<std::shared_ptr<GeomAPI_Face> > myFaces;
  bool myIsInHistory;
  bool myIsInfinite;
 public:
  /// Retuns the parameters of color definition in the resources config manager
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
  /// The construction element is never concealed
  MODEL_EXPORT virtual void setIsConcealed(const bool theValue);

  // methods related to selection of sub-shapes in construction, used by SelectionAttribute

  /// Selects theSubShape in the construction. Returns an index of the selected sub-shape.
  /// Puts the selected shape with a needed BRepNaming sub-structure to the data tree of result.
  /// If theSubShape is null, it selects the whole construction and returns zero index.
  /// If theIndex is not -1, it re-selects over the existing data (used for update selection).
  /// If theExtDoc is document where this selection is needed, if it differs from this,
  /// naming structures will be located there.
  int select(const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const std::shared_ptr<ModelAPI_Document> theExtDoc, const int theIndex = -1);

  /// Returns already selected shape by the given index. Zero index means the whole construction,
  /// so, the returned shape in this case is null.
  /// If theExtDoc is document where this selection is needed, if it differs from this,
  /// naming structures will be located there.
  std::shared_ptr<GeomAPI_Shape> shape(const int theIndex,
    const std::shared_ptr<ModelAPI_Document> theExtDoc);

  /// Updates the existing selection by the index.
  /// Returns false if update is failed. Returns theModified true if the selection was updated.
  /// If theExtDoc is document where this selection is needed, if it differs from this,
  /// naming structures will be updated there.
  bool update(const int theIndex, const std::shared_ptr<ModelAPI_Document> theExtDoc,
    bool& theModified);

 protected:
  /// Makes a body on the given feature
  Model_ResultConstruction();

  /// Searchies for the working label selection/update will start from
  /// Returns true if this is label of the external document.
  /// theExtDoc is document where this selection is required
  TDF_Label startLabel(const std::shared_ptr<ModelAPI_Document> theExtDoc, bool& theExternal);

  friend class Model_Objects;
};

#endif
