// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Face.h
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Face_H_
#define GeomAPI_Face_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pln;

/**\class GeomAPI_Face
* \ingroup DataModel
 * \brief Interface to the face object
 */
class GeomAPI_Face : public GeomAPI_Shape
{
public:
  /// Creation of empty (null) shape
  GEOMAPI_EXPORT 
  GeomAPI_Face();

  /// Creation of face by the face-shape
  GEOMAPI_EXPORT 
  GeomAPI_Face(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns true if the current face is geometrically equal to the given face
  GEOMAPI_EXPORT 
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theFace) const;

  /// Returns true if the face is a planar face
  GEOMAPI_EXPORT 
  bool isPlanar() const;

  /// Returns true if the face is a cylindrical face
  GEOMAPI_EXPORT 
  bool isCylindrical() const;

  /// Returns the base plane of the face (if it is planar) with location in the center of the face
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pln> getPlane() const;
};

#endif

