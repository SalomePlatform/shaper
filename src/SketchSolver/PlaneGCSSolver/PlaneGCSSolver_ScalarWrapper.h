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

#ifndef PlaneGCSSolver_ScalarWrapper_H_
#define PlaneGCSSolver_ScalarWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS scalars.
 */
class PlaneGCSSolver_ScalarWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_ScalarWrapper(double *const theParam);

  /// \brief Return PlaneGCS parameter
  double* scalar() const
  { return myValue; }

  /// \brief Change value of parameter
  virtual void setValue(double theValue);
  /// \brief Return value of parameter
  virtual double value() const;

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_SCALAR; }

protected:
  double* myValue; ///< pointer to value provided by the storage
};

typedef std::shared_ptr<PlaneGCSSolver_ScalarWrapper> ScalarWrapperPtr;

#endif
