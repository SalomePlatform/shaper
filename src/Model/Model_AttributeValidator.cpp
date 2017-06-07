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

#include "Model_AttributeValidator.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

bool Model_AttributeValidator::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments,
                                       Events_InfoMessage& theError) const
{
  if (theAttribute->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr anAttribue =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    if (!anAttribue->expressionError().empty()) {
      theError = "Expression error: %1";
      theError.arg(anAttribue->expressionError());
      return false;
    }
  } else
  if (theAttribute->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttribue =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (!anAttribue->expressionError().empty()) {
      theError = "Expression error: %1";
      theError.arg(anAttribue->expressionError());
      return false;
    }
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttribue =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);

    const char* aComponent[] = {"X", "Y", "Z"};
    std::string anErrorMessage;
    for (int i = 0; i < 3; ++i) {
      if (!anAttribue->expressionError(i).empty())
        anErrorMessage.append("\n").append(aComponent[i])
          .append(": ").append(anAttribue->expressionError(i));
    }
    if (!anErrorMessage.empty()) {
      theError = "Expression error: %1";
      theError.arg(anErrorMessage);
      return false;
    }
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr anAttribue =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);

    const char* aComponent[] = {"X", "Y"};
    std::string anErrorMessage;
    for (int i = 0; i < 2; ++i) {
      if (!anAttribue->expressionError(i).empty())
        anErrorMessage.append("\n").append(aComponent[i])
          .append(": ").append(anAttribue->expressionError(i));
    }
    if (!anErrorMessage.empty()) {
      theError = "Expression error: %1";
      theError.arg(anErrorMessage);
      return false;
    }
  }
  return true;
}
