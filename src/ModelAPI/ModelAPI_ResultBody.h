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

#ifndef ModelAPI_ResultBody_H_
#define ModelAPI_ResultBody_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <string>
#include <map>
#include <vector>

class ModelAPI_BodyBuilder;
class GeomAlgoAPI_MakeShape;

/**\class ModelAPI_ResultBody
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
* Also provides a container of sub-body result in case it is compound or compsolid.
*/
class ModelAPI_ResultBody : public ModelAPI_Result
{
public:
  /// Internal enumeration for storage the information of connected topology flag
  enum ConnectedTopologyFlag {
    ConnectionNotComputed, ///< not yet computed
    IsConnected,           ///< the topology is connected
    IsNotConnected         ///< the topology is connected
  };

protected:
  /// Keeps (not persistently) the connected topology flag
  ConnectedTopologyFlag myConnect;

  ModelAPI_BodyBuilder* myBuilder; ///< provides the body processing in naming shape

public:
  MODELAPI_EXPORT virtual ~ModelAPI_ResultBody();

  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Bodies";
    return MY_GROUP;
  }

  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_BODY_COLOR("200,200,230");
    return RESULT_BODY_COLOR;
  }

  /// default deflection for a result body
  inline static const std::string DEFAULT_DEFLECTION()
  {
    return "0.0001";
  }

  /// Returns the number of sub-elements
  MODELAPI_EXPORT virtual int numberOfSubs(bool forTree = false) const = 0;

  /// Returns the sub-result by zero-base index
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_ResultBody> subResult(
    const int theIndex, bool forTree = false) const = 0;

  /// Returns true if theResult belong to this composite result as sub.
  /// Returns theIndex - zero based index of sub if found
  MODELAPI_EXPORT virtual bool isSub(ObjectPtr theResult, int& theIndex) const = 0;

  /// \brief Stores the shape (called by the execution method).
  /// param[in] theShape shape to store.
  /// param[in] theIsStoreSameShapes if false stores reference to the same shape
  ///                                if it is already in document.
  MODELAPI_EXPORT virtual void store(const GeomShapePtr& theShape,
                                     const bool theIsStoreSameShapes = true);

  /// Stores the generated shape (called by the execution method).
  MODELAPI_EXPORT virtual void storeGenerated(const GeomShapePtr& theFromShape,
                                              const GeomShapePtr& theToShape);

  /// Stores the root modified shapes (called by the execution method).
  MODELAPI_EXPORT virtual void storeGenerated(
    const std::list<GeomShapePtr>& theFromShapes, const GeomShapePtr& theToShape,
    const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);

  /// Stores the modified shape (called by the execution method).
  MODELAPI_EXPORT virtual void storeModified(const GeomShapePtr& theOldShape,
                                             const GeomShapePtr& theNewShape,
                                             const bool theIsCleanStored = true);

  /// Stores the root modified shapes (called by the execution method).
  MODELAPI_EXPORT virtual void storeModified(
    const std::list<GeomShapePtr>& theOldShapes, const GeomShapePtr& theNewShape,
    const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);

  /// Returns the shape-result produced by this feature
  MODELAPI_EXPORT virtual GeomShapePtr shape();

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  /// Returns true if it is stored correctly (the final shape contains this new sub-shape)
  MODELAPI_EXPORT virtual bool generated(const GeomShapePtr& theNewShape,
    const std::string& theName, const bool theCheckIsInResult = true) = 0;

  /// Records the shape newShape which was generated from the shape oldShape during a topological
  /// construction. As an example, consider the case of a face generated from an edge in
  /// construction of a prism.
  MODELAPI_EXPORT virtual void generated(const GeomShapePtr& theOldShape,
                                         const GeomShapePtr& theNewShape,
                                         const std::string& theName = "");

  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  MODELAPI_EXPORT virtual void modified(const GeomShapePtr& theOldShape,
                                        const GeomShapePtr& theNewShape,
                                        const std::string& theName = "");

  /// load deleted shapes
  MODELAPI_EXPORT
  virtual void loadDeletedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                 const GeomShapePtr& theOldShape,
                                 const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                 const GeomShapePtr& theShapesToExclude = GeomShapePtr());

  /// load and orient modified shapes
  MODELAPI_EXPORT
  virtual void loadModifiedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                  const GeomShapePtr& theOldShape,
                                  const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                  const std::string& theName = "") = 0;

  /// load and orient generated shapes
  MODELAPI_EXPORT
  virtual void loadGeneratedShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgo,
                                   const GeomShapePtr& theOldShape,
                                   const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                   const std::string& theName = "",
                                   const bool theSaveOldIfNotInTree = false) = 0;

  /// load shapes of the first level (to be used during shape import)
  MODELAPI_EXPORT virtual void loadFirstLevel(GeomShapePtr theShape,
                                              const std::string& theName) = 0;

  /// Returns true is the topology is connected.
  MODELAPI_EXPORT virtual bool isConnectedTopology() = 0;

  /// Set displayed flag to the result and all sub results
  /// \param theDisplay a boolean value
  MODELAPI_EXPORT virtual void setDisplayed(const bool theDisplay);

  /// Updates the sub-bodies if shape of this object is compsolid or compound
  MODELAPI_EXPORT virtual void updateSubs(const GeomShapePtr& theThisShape,
    const bool theShapeChanged = true) = 0;

  /// Updates the sub-bodies in accordance to the algorithm history information
  MODELAPI_EXPORT virtual void updateSubs(
    const GeomShapePtr& theThisShape, const std::list<GeomShapePtr>& theOlds,
    const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape, const bool isGenerated) = 0;

  /// Cleans cash related to the already stored elements
  MODELAPI_EXPORT virtual void cleanCash() = 0;

  /// Add shape Name for read shape in step file
  MODELAPI_EXPORT virtual std::wstring addShapeName
                              (std::shared_ptr<GeomAPI_Shape>,const std::wstring& theName) = 0;

  /// Add color for shape Name read shape in step file
  MODELAPI_EXPORT virtual void addShapeColor
                              (const std::wstring& theName,std::vector<int>& theColor) = 0;

  /// Set the map of name and color read shape in step file
  MODELAPI_EXPORT virtual void setShapeName
                          (std::map< std::wstring, std::shared_ptr<GeomAPI_Shape> > &theShapeName,
                           std::map< std::wstring, std::vector<int>> & theColorsShape) = 0;

  /// Clear the map of name and color read shape in step file
  MODELAPI_EXPORT virtual void clearShapeNameAndColor() = 0;

  /// find the name of shapp read in step file
  MODELAPI_EXPORT virtual std::wstring findShapeName(std::shared_ptr<GeomAPI_Shape> theShape) = 0;


protected:
  /// Default constructor accessible only from Model_Objects
  MODELAPI_EXPORT ModelAPI_ResultBody();

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultBody> ResultBodyPtr;

#endif
