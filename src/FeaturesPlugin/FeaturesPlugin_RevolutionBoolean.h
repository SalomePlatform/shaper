// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_RevolutionBoolean_H_
#define FeaturesPlugin_RevolutionBoolean_H_

#include "FeaturesPlugin_Revolution.h"
#include "FeaturesPlugin_CompositeBoolean.h"

/// \class FeaturesPlugin_RevolutionBoolean
/// \ingroup Plugins
/// \brief Interface for the composite revolution feature.
class FeaturesPlugin_RevolutionBoolean : public FeaturesPlugin_Revolution,
                                         public FeaturesPlugin_CompositeBoolean
{
public:
  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

protected:
  FeaturesPlugin_RevolutionBoolean(){};

  // Creates revolutions.
  bool makeGeneration(ListOfShape& theBaseShapes,
                      ListOfMakeShape& theMakeShapes);

  /// Stores generation history.
  void storeGenerationHistory(ResultBodyPtr theResultBody,
                              const GeomShapePtr theBaseShape,
                              const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);
};

#endif
