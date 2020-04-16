// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef PlaneGCSSolver_EntityWrapper_H_
#define PlaneGCSSolver_EntityWrapper_H_

#include <PlaneGCSSolver_Defs.h>

#include <map>
#include <memory>

class ModelAPI_Attribute;

class PlaneGCSSolver_EntityWrapper;
typedef std::shared_ptr<PlaneGCSSolver_EntityWrapper> EntityWrapperPtr;

/**
 *  Wrapper providing operations with entities regardless the solver.
 */
class PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_EntityWrapper() : myExternal(false) {}
  virtual ~PlaneGCSSolver_EntityWrapper() {}

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const = 0;

  /// \brief Change flag indicating the entity cannot be changed in the solver
  void setExternal(bool theExternal) { myExternal = theExternal; }
  /// \brief Return the External flag
  bool isExternal() const { return myExternal; }

  /// \brief Store names of attributes and their values if necessary
  void setAdditionalAttributes(const std::map<std::string, EntityWrapperPtr>& theAttribues)
  { myAdditionalAttributes = theAttribues; }
  /// \brief Return the list of additional attributes
  const std::map<std::string, EntityWrapperPtr>& additionalAttributes() const
  { return myAdditionalAttributes; }

protected:
  /// \brief Update entity by the values of theAttribute
  /// \return \c true if any value of attribute is not equal to the stored in the entity
  virtual bool update(std::shared_ptr<ModelAPI_Attribute> theAttribute)
  { return false; }

  friend class PlaneGCSSolver_AttributeBuilder;

private:
  bool myExternal;
  std::map<std::string, EntityWrapperPtr> myAdditionalAttributes;
};

#endif
