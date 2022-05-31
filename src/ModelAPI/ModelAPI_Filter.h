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

#ifndef ModelAPI_Filter_H_
#define ModelAPI_Filter_H_

#include "ModelAPI_FiltersArgs.h"
#include "ModelAPI_ResultBody.h"

#include <GeomAPI_Shape.h>


/**\class ModelAPI_ViewFilter
* \ingroup DataModel
* \brief A general interface class filters definition
*/
class ModelAPI_Filter
{
public:
  virtual ~ModelAPI_Filter() {}

  /// Returns name of the filter to represent it in GUI
  virtual const std::string& name() const = 0;

  /// Returns true if the given shape type is supported
  virtual bool isSupported(GeomAPI_Shape::ShapeType theType) const = 0;

  /// This method should contain the filter logic. It returns true if the given shape
  /// is accepted by the filter.
  /// \param theShape the given shape
  /// \param theResult parent result of the shape to be checked
  /// \param theArgs arguments of the filter
  virtual bool isOk(const GeomShapePtr& theShape,
                    const ResultPtr& theResult,
                    const ModelAPI_FiltersArgs& theArgs) const = 0;

  /// Returns True if the filter can be used several times within one filtering
  virtual bool isMultiple() const { return false; }

  /// Returns XML string which represents GUI of the filter
  /// By default it returns nothing (no GUI)
  virtual std::string xmlRepresentation() const { return ""; }

  /// Initializes arguments of a filter. If a filter has no arguments, this method may be
  /// not redefined.
  virtual void initAttributes(ModelAPI_FiltersArgs& /*theArguments*/) {}

  /// Returns XML string which represents GUI of the filter
  /// by reading corresponding XML file.
  MODELAPI_EXPORT virtual std::string xmlFromFile(const std::string& theConfigFile) const;

private:
  bool myIsReverse;
};

typedef std::shared_ptr<ModelAPI_Filter> FilterPtr;

#endif