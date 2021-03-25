// Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

#ifndef PlaneGCSSolver_ScalarArrayWrapper_H_
#define PlaneGCSSolver_ScalarArrayWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with array of PlaneGCS scalars.
 */
class PlaneGCSSolver_ScalarArrayWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_ScalarArrayWrapper(const GCS::VEC_pD& theParam);

  /// \breif Size of array
  int size() const { return (int)myValue.size(); }

  /// \brief Return array of PlaneGCS parameters
  const GCS::VEC_pD& array() const { return myValue; }
  /// \breif Set array of parameters
  void setArray(const GCS::VEC_pD& theParams) { myValue = theParams; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_SCALAR_ARRAY; }

protected:
  /// \brief Update entity by the values of theAttribute
  /// \return \c true if any value of attribute is not equal to the stored in the entity
  virtual bool update(std::shared_ptr<ModelAPI_Attribute> theAttribute);

protected:
  GCS::VEC_pD myValue; ///< list of pointers to values provided by the storage
};

typedef std::shared_ptr<PlaneGCSSolver_ScalarArrayWrapper> ScalarArrayWrapperPtr;

#endif
