// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ax3.hxx
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef GEOMAPI_AX3_H_
#define GEOMAPI_AX3_H_

#include "GeomAPI.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Dir.h"

/**
 * \ingroup DataModel
 * \brief The class represents a coordinate plane which is 2d plane with X and Y directions
 * and origin
 */ 
class GEOMAPI_EXPORT GeomAPI_Ax3 : public GeomAPI_Interface
{
public:
  /// Default constructor
  GeomAPI_Ax3();

  /// Ñonstructor
  /// \param theOrigin point of origin
  /// \param theDirX direction of X axis
  /// \param theNorm direction of normal vector
  GeomAPI_Ax3(std::shared_ptr<GeomAPI_Pnt> theOrigin,
              std::shared_ptr<GeomAPI_Dir> theDirX,
              std::shared_ptr<GeomAPI_Dir> theNorm);

  /// Sets origin point
  void setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin);

  /// Returns the plane origin point
  std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Sets X direction vector
  void setDirX(const std::shared_ptr<GeomAPI_Dir>& theDirX);

  /// Returns X direction vector
  std::shared_ptr<GeomAPI_Dir> dirX() const;

  /// Sets Y direction vector
  void setDirY(const std::shared_ptr<GeomAPI_Dir>& theDirY);

  /// Returns Y direction vector
  std::shared_ptr<GeomAPI_Dir> dirY() const;

  /// Sets Z direction vector
  void setNorm(const std::shared_ptr<GeomAPI_Dir>& theNorm);

  /// Returns Z direction vector
  std::shared_ptr<GeomAPI_Dir> norm() const;

  /// Converts 2d coordinates from the plane to 3d space point
  /// \param theX X coordinate
  /// \param theY Y coordinate
  std::shared_ptr<GeomAPI_Pnt> to3D(double theX, double theY) const;

  /// Converts 3d  to 2d coordinates of the plane
  /// \param theX X coordinate
  /// \param theY Y coordinate
  /// \param theZ Z coordinate
  std::shared_ptr<GeomAPI_Pnt2d> to2D(double theX, double theY, double theZ) const;

};


#endif