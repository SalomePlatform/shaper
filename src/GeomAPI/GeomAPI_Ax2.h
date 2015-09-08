// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ax2.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAPI_Ax2_H_
#define GeomAPI_Ax2_H_

#include <GeomAPI.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

/** \ingroup DataModel
 *  \brief The class represents an axis in 3D space.
 */
class GeomAPI_Ax2 : public GeomAPI_Interface
{
public:
  /// Default constructor.
  GEOMAPI_EXPORT 
  GeomAPI_Ax2();

  /** \brief Ñonstructor.
   *  \param[in] theOrigin point of origin.
   *  \param[in] theN direction of axis.
   *  \param[in] theVX x direction of axis.
   */
  GEOMAPI_EXPORT 
  GeomAPI_Ax2(std::shared_ptr<GeomAPI_Pnt> theOrigin,
              std::shared_ptr<GeomAPI_Dir> theN,
              std::shared_ptr<GeomAPI_Dir> theVX);

  /** \brief Ñonstructor.
   *  \param[in] theOrigin point of origin.
   *  \param[in] theDir direction of axis.
   */
  GEOMAPI_EXPORT 
  GeomAPI_Ax2(std::shared_ptr<GeomAPI_Pnt> theOrigin,
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
};

#endif
