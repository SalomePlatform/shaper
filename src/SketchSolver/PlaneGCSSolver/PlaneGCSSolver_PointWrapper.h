// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef PlaneGCSSolver_PointWrapper_H_
#define PlaneGCSSolver_PointWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS points.
 */
class PlaneGCSSolver_PointWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_PointWrapper(const GCSPointPtr thePoint);

  /// \brief Return PlaneGCS point
  const GCSPointPtr& point() const
  { return myPoint; }
  /// \brief Return PlaneGCS point to change
  GCSPointPtr& changeEntity()
  { return myPoint; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_POINT; }

private:
  GCSPointPtr myPoint;
};

typedef std::shared_ptr<PlaneGCSSolver_PointWrapper> PointWrapperPtr;

#endif
