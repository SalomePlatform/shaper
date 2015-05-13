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
class GEOMAPI_EXPORT GeomAPI_Ax1 : public GeomAPI_Interface
{
public:
  /// Default constructor.
  GeomAPI_Ax1();

  /** \brief Ñonstructor.
  *   \param[in] theOrigin point of origin.
  *   \param[in] theDir direction of axis.
  */
  GeomAPI_Ax1(std::shared_ptr<GeomAPI_Pnt> theOrigin,
              std::shared_ptr<GeomAPI_Dir> theDir);

  /// Sets origin point.
  void setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin);

  /// \return the plane origin point.
  std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Sets direction vector.
  void setDir(const std::shared_ptr<GeomAPI_Dir>& theDir);

  /// \return direction vector.
  std::shared_ptr<GeomAPI_Dir> dir() const;

  /// Reverses the unit vector of this axis and assigns the result to this axis.
  void reverse();

  /// \return reversed unit vector of this axis.
  std::shared_ptr<GeomAPI_Ax1> reversed();
};

#endif
