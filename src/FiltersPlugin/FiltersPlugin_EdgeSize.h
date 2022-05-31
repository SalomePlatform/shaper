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

#ifndef FILTERSPLUGIN_EDGESIZE_H_
#define FILTERSPLUGIN_EDGESIZE_H_

#include "FiltersPlugin.h"

#include <GeomAPI_Shape.h>
#include <ModelAPI_Filter.h>

/**\class FiltersPlugin_EdgeSize
* \ingroup DataModel
* \brief Filter for edges with specific size
*/
class FiltersPlugin_EdgeSize : public ModelAPI_Filter
{
public:
  FiltersPlugin_EdgeSize() : ModelAPI_Filter() {}

  virtual const std::string& name() const {
    static const std::string kName("Edge size");
    return kName;
  }

  /// Returns true for edge type
  virtual bool isSupported(GeomAPI_Shape::ShapeType theType) const override;

  /// This method should contain the filter logic. It returns true if the given shape
  /// is accepted by the filter.
  /// \param theShape the given shape
  /// \param theArgs arguments of the filter
  virtual bool isOk(const GeomShapePtr& theShape, const ResultPtr&,
                    const ModelAPI_FiltersArgs& theArgs) const override;

  /// Returns XML string which represents GUI of the filter
  virtual std::string xmlRepresentation() const override;

  /// Initializes arguments of a filter.
  virtual void initAttributes(ModelAPI_FiltersArgs& theArguments) override;

};

#endif
