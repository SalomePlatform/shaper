// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_CommonBoundingBox_H_
#define FeaturesPlugin_CommonBoundingBox_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Box.h>

#include <GeomAPI_IPresentable.h>
#include <GeomAPI_IScreenParams.h>

#include <ModelAPI_Attribute.h>

/// \class FeaturesPlugin_CommonBoundingBox
/// \ingroup Plugins
/// \brief Feature to view the Bounding Box.

class FeaturesPlugin_CommonBoundingBox : public ModelAPI_Feature
{
public:
  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute(){};

  /// Return Attribut values of result.
  virtual AttributePtr attributResultValues() = 0;

protected:
  FeaturesPlugin_CommonBoundingBox() {}

  /// Create box with two points
  void createBoxByTwoPoints();

  /// Create namming
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBox);

  GeomShapePtr myShape;
};

#endif
