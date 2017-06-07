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

#ifndef PlaneGCSSolver_EdgeWrapper_H_
#define PlaneGCSSolver_EdgeWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS entities (lines, circles and arcs).
 */
class PlaneGCSSolver_EdgeWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_EdgeWrapper(const GCSCurvePtr theEntity);

  /// \brief Return PlaneGCS geometric entity
  const GCSCurvePtr& entity() const
  { return myEntity; }
  /// \brief Return PlaneGCS geometric entity to change
  GCSCurvePtr& changeEntity()
  { return myEntity; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return myType; }

private:
  SketchSolver_EntityType myType;
  GCSCurvePtr myEntity;
};

typedef std::shared_ptr<PlaneGCSSolver_EdgeWrapper> EdgeWrapperPtr;

#endif
