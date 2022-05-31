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

#ifndef Model_ResultBody_H_
#define Model_ResultBody_H_

#include "Model.h"
#include <ModelAPI_ResultBody.h>
#include <vector>
#include <map>

#include <TopTools_DataMapOfShapeListOfShape.hxx>

/**\class Model_ResultBody
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
* Also provides a container of sub-body result in case it is compound or compsolid.
*/
class Model_ResultBody : public ModelAPI_ResultBody
{
  /// Sub-bodies if this is compsolid or compound: zero-based index to subs
  std::vector<ResultBodyPtr> mySubs;
  /// Also keep map of result to index in mySubs to facilitate speed of access from OB
  std::map<ObjectPtr, int> mySubsMap;
  /// Keeps the last state of the concealment flag in order to update it when needed.
  bool myLastConcealed;
  /// History information for update subs
  std::shared_ptr<GeomAlgoAPI_MakeShape> myAlgo;
  /// All old shapes used for the root result construction
  std::list<GeomShapePtr> myOlds;
  /// Information about the kind of the history information: modified or generated
  bool myIsGenerated;
  /// Map from old shape to list of new shapes, cash for computeOldForSub method
  TopTools_DataMapOfShapeListOfShape myHistoryCash;

public:

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultBody();

  /// Request for initialization of data model of the result body: adding all attributes
  virtual void initAttributes();

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  MODEL_EXPORT virtual bool generated(const GeomShapePtr& theNewShape,
    const std::string& theName, const bool theCheckIsInResult = true) override;

  /// load generated shapes
  MODEL_EXPORT
  virtual void loadGeneratedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                   const GeomShapePtr& theOldShape,
                                   const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                   const std::string& theName = "",
                                   const bool theSaveOldIfNotInTree = false) override;

  /// load modified shapes for sub-objects
  MODEL_EXPORT
  virtual void loadModifiedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                  const GeomShapePtr& theOldShape,
                                  const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                  const std::string& theName = "") override;

  /// load shapes of the first level (to be used during shape import)
  MODEL_EXPORT virtual void loadFirstLevel(GeomShapePtr theShape, const std::string& theName);

  /// Returns the number of sub-elements
  MODEL_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// Returns the sub-result by zero-base index
  MODEL_EXPORT virtual ResultBodyPtr subResult(const int theIndex,
    bool forTree = false) const;

  /// Returns true if feature or result belong to this composite feature as subs
  /// Returns theIndex - zero based index of sub if found
  MODEL_EXPORT virtual bool isSub(ObjectPtr theObject, int& theIndex) const;

  /// Returns the parameters of color definition in the resources configuration manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Disables the result body: keeps the resulting shape as selection, but erases the underlaying
  /// naming data structure if theFlag if false. Or restores every  thing on theFlag is true.
  MODEL_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// The compsolid is concealed if at least one of the sub is concealed
  MODEL_EXPORT virtual bool isConcealed();

  /// Sets all subs as concealed in the data tree (referenced by other objects)
  MODEL_EXPORT virtual void setIsConcealed(const bool theValue, const bool theForced = false);

  /// Returns true is the topology is connected.
  MODEL_EXPORT virtual bool isConnectedTopology();

  /// Cleans cash related to the already stored elements
  MODEL_EXPORT virtual void cleanCash() override;

  /// sets the texture file
  MODEL_EXPORT virtual bool hasTexture() override;


protected:
  /// Makes a body on the given feature
  Model_ResultBody();

  /// Updates the sub-bodies if shape of this object is composite-solid
  void updateSubs(const std::shared_ptr<GeomAPI_Shape>& theThisShape,
                  const bool theShapeChanged = true);

  /// Updates the sub-bodies in accordance to the algorithm history information
  void updateSubs(
    const GeomShapePtr& theThisShape, const std::list<GeomShapePtr>& theOlds,
    const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape, const bool isGenerated);

  /// Checks the state of children and parents to send events of creation/erase when needed
  void updateConcealment();

  /// Adds to theOldForSub only old shapes that where used for theSub creation
  void computeOldForSub(const GeomShapePtr& theSub,
    const std::list<GeomShapePtr>& theAllOlds, std::list<GeomShapePtr>& theOldForSub);

  friend class Model_Objects;

  /// Add shape Name for read shape in step file
  std::wstring addShapeName(std::shared_ptr<GeomAPI_Shape>,const std::wstring& theName) override;

  /// Add color for shape Name read shape in step file
  void addShapeColor( const std::wstring& theName,std::vector<int>& color) override;

  /// Set the map of name and color read shape in step file
  void setShapeName(std::map< std::wstring,
                              std::shared_ptr<GeomAPI_Shape>>& theShapeName,
                              std::map< std::wstring,
                              std::vector<int>>& theColorsShape) override;

  /// find the name of shapp read in step file
  std::wstring findShapeName(std::shared_ptr<GeomAPI_Shape> theShape) override;

  /// Clear the map of name and color read shape in step file
  void clearShapeNameAndColor() override;

  /// map with the name read in step file and shape
  std::map< std::wstring, std::shared_ptr<GeomAPI_Shape> > myNamesShape;

  /// map with the name contruct and color read
  std::map< std::wstring, std::vector<int>> myColorsShape;

};

#endif
