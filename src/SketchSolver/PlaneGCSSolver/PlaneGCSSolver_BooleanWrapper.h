// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef PlaneGCSSolver_BooleanWrapper_H_
#define PlaneGCSSolver_BooleanWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing storage for boolean values.
 */
class PlaneGCSSolver_BooleanWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_BooleanWrapper(bool theValue);

  /// \brief Change value of parameter
  void setValue(bool theValue)
  { myValue = theValue; }
  /// \brief Return value of parameter
  bool value() const
  { return myValue; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_BOOLEAN; }

protected:
  /// \brief Update entity by the values of theAttribute
  /// \return \c true if any value of attribute is not equal to the stored in the entity
  virtual bool update(std::shared_ptr<ModelAPI_Attribute> theAttribute);

protected:
  bool myValue;
};

typedef std::shared_ptr<PlaneGCSSolver_BooleanWrapper> BooleanWrapperPtr;

#endif
