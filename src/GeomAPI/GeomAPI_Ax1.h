// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ax1.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef GEOMAPI_AX1_H_
#define GEOMAPI_AX1_H_

#include <GeomAPI.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

/** \ingroup DataModel
 *  \brief The class represents an axis in 3D space.
 */
class GeomAPI_Ax1 : public GeomAPI_Interface
{
public:
  /// Default constructor.
  GEOMAPI_EXPORT 
  GeomAPI_Ax1();

  /** \brief Ñonstructor.
   *  \param[in] theOrigin point of origin.
   *  \param[in] theDir direction of axis.
   */
  GEOMAPI_EXPORT 
  GeomAPI_Ax1(std::shared_ptr<GeomAPI_Pnt> theOrigin,
              std::shared_ptr<GeomAPI_Dir> theDir);

  /// Sets origin point.
  GEOMAPI_EXPORT 
  void setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin);

  /// \return the plane origin point.
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Sets direction vector.
  GEOMAPI_EXPORT 
  void setDir(const std::shared_ptr<GeomAPI_Dir>& theDir);

  /// \return direction vector.
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Dir> dir() const;

  /// Reverses the unit vector of this axis and assigns the result to this axis.
  GEOMAPI_EXPORT 
  void reverse();

  /// \return reversed unit vector of this axis.
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Ax1> reversed();
};

#endif
