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

#ifndef GeomAlgoAPI_MakeShape_H_
#define GeomAlgoAPI_MakeShape_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_DataMapOfShapeShape.h>

#include <list>
#include <memory>
#include <map>
#include <string>

/// \class GeomAlgoAPI_MakeShape
/// \ingroup DataAlgo
/// \brief Interface to the root class of all topological shapes constructions
class GeomAlgoAPI_MakeShape : public GeomAPI_Interface
{
public:
  /// Builder type enum
  enum BuilderType {
    Unknown,
    OCCT_BRepBuilderAPI_MakeShape,
    OCCT_BOPAlgo_Builder
  };

public:
  /// \brief Empty constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape();

  /// Destructor for remove myHist
  GEOMALGOAPI_EXPORT ~GeomAlgoAPI_MakeShape();

  /// \brief Constructor by builder and builder type.
  /// \param[in] theBuilder pointer to the builder.
  /// \param[in] theBuilderType builder type.
  template<class T> explicit GeomAlgoAPI_MakeShape(T* theBuilder,
    const BuilderType theBuilderType = OCCT_BRepBuilderAPI_MakeShape)
  : GeomAPI_Interface(theBuilder),
    myBuilderType(theBuilderType)
  {
    initialize();
  }

  /// \brief Initializes internals.
  /// \param[in] theBuilder pointer to the builder.
  /// \param[in] theBuilderType builder type.
  template<class T> void initialize(T* theBuilder,
    const BuilderType theBuilderType = OCCT_BRepBuilderAPI_MakeShape)
  {
    setImpl(theBuilder);
    myBuilderType = theBuilderType;
    initialize();
  }

  /// \return status of builder.
  GEOMALGOAPI_EXPORT bool isDone() const;

  /// \return a shape built by the shape construction algorithm.
  GEOMALGOAPI_EXPORT virtual const std::shared_ptr<GeomAPI_Shape> shape() const;

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT bool isValid() const;

  /// \return true if resulting shape has volume.
  GEOMALGOAPI_EXPORT bool hasVolume() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfSubShapes() const;

  /// \return the list of shapes generated from the shape \a theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory generated shapes.
  GEOMALGOAPI_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                            ListOfShape& theHistory);

  /// \return the list of shapes modified from the shape \a theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory modified shapes. Does not cleared!
  GEOMALGOAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory);

  /// \return true if theShape was deleted.
  /// \param[in] theShape base shape.
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return true if the data were correct.
  GEOMALGOAPI_EXPORT virtual bool check() { return true; };

  ///  \return the list of created faces.
  GEOMALGOAPI_EXPORT std::map< std::string, std::shared_ptr<GeomAPI_Shape> > getCreatedFaces()
  {return myCreatedFaces;}

  /// \return the error.
  GEOMALGOAPI_EXPORT std::string getError() { return myError; }

  /// \brief Prepare the naming of faces.
  GEOMALGOAPI_EXPORT virtual void prepareNamingFaces();

  /// \brief Check the validity of the produced shape.
  GEOMALGOAPI_EXPORT bool checkValid(std::string theMessage);

  /// Optimization of access the new shapes by old shapes for the limited set of needed new shapes.
  /// \param theWholeOld the whole old shape
  /// \param theShapeType type of the sub-shapes that is used for optimization
  /// \returns true if optimization containers are already filled
  GEOMALGOAPI_EXPORT bool newShapesCollected(
    std::shared_ptr<GeomAPI_Shape> theWholeOld, const int theShapeType);

  /// Optimization of access the new shapes by old shapes for the limited set of needed new shapes.
  /// \param theWholeOld the whole old shape
  /// \param theShapeType type of the sub-shapes that is used for optimization
  /// \returns true if optimization containers are already filled
  GEOMALGOAPI_EXPORT void collectNewShapes(
    std::shared_ptr<GeomAPI_Shape> theWholeOld, const int theShapeType);

  /// Optimization of access the new shapes by old shapes for the limited set of needed new shapes.
  /// \param theWholeOld the whole old shape
  /// \param theNewShape the whole new shape
  /// \param theShapeType type of the old sub-shapes
  /// \returns compound of all old shapes that were used for creation of the given new
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Shape> oldShapesForNew(
    std::shared_ptr<GeomAPI_Shape> theWholeOld,
    std::shared_ptr<GeomAPI_Shape> theNewShape, const int theShapeType);

protected:
  /// \brief Sets builder type.
  /// \param[in] theBuilderType new builder type.
  void setBuilderType(const BuilderType theBuilderType);

  /// \brief Sets status of builder.
  /// \param[in] theFlag new status.
  void setDone(const bool theFlag);

  /// \brief Sets result shape.
  /// \param[in] theShape new shape.
  void setShape(const std::shared_ptr<GeomAPI_Shape> theShape);

protected:
   /// Data map to keep correct orientation of sub-shapes.
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  /// Error occurred during the execution of an algorithm.
  std::string myError;
  /// Map of created faces with their name for naming.
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > myCreatedFaces;

private:
  /// \brief Initializes internals.
  void initialize();

private:
  GeomAlgoAPI_MakeShape::BuilderType myBuilderType; ///< Type of make shape builder.
  bool myDone; ///< Builder status.
  std::shared_ptr<GeomAPI_Shape> myShape; ///< Resulting shape.

  /// map that is used to keep the optimization structure for access to the history
  /// kind of sub-shapes -> whole old shape -> new shape -> list of old shapes that create this new
  void* myHist;
};

typedef std::list<std::shared_ptr<GeomAlgoAPI_MakeShape> > ListOfMakeShape;

#endif
