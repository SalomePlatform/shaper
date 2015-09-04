// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShapeCustom.h
// Created:     4 September 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_MakeShapeCustom_H_
#define GeomAlgoAPI_MakeShapeCustom_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_DataMapOfShapeMapOfShapes.h>
#include <GeomAPI_DataMapOfShapeShape.h>

/** \class GeomAlgoAPI_MakeShapeCustom
 *  \ingroup DataAlgo
 *  \brief Interface to the root class of all topological shapes constructions
 */
class GeomAlgoAPI_MakeShapeCustom : public GeomAlgoAPI_MakeShape
{
public:
  /// Default constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeCustom();

  /// Sets result shape.
  GEOMALGOAPI_EXPORT void setResult(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// Adds modified shape.
  GEOMALGOAPI_EXPORT bool addModified(const std::shared_ptr<GeomAPI_Shape> theBase,
                                      const std::shared_ptr<GeomAPI_Shape> theResult);

  /// Adds generated shape.
  GEOMALGOAPI_EXPORT bool addGenerated(const std::shared_ptr<GeomAPI_Shape> theBase,
                                       const std::shared_ptr<GeomAPI_Shape> theResult);

  /// Adds deleted shape.
  GEOMALGOAPI_EXPORT bool addDeleted(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return a shape built by the shape construction algorithms
  GEOMALGOAPI_EXPORT virtual const std::shared_ptr<GeomAPI_Shape> shape() const;

  /// \return the list of shapes generated from the shape theShape
  GEOMALGOAPI_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                            ListOfShape& theHistory);

  /// \return the list of shapes modified from the shape theShape
  GEOMALGOAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory);

  /// \return whether the shape is deleted
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape);

private:
  std::shared_ptr<GeomAPI_Shape>    myShape;
  GeomAPI_DataMapOfShapeMapOfShapes myGenerated;
  GeomAPI_DataMapOfShapeMapOfShapes myModified;
  GeomAPI_DataMapOfShapeShape       myDeleted;
};

#endif