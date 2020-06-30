// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_CompositeBoolean_H_
#define FeaturesPlugin_CompositeBoolean_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_MakeShape.h>

/// \class FeaturesPlugin_CompositeBoolean
/// \ingroup Plugins
/// \brief Interface for the composite boolean feature.
class FeaturesPlugin_CompositeBoolean
{
public:
  enum OperationType {
    BOOL_CUT,
    BOOL_FUSE,
    BOOL_COMMON,
    BOOL_SMASH
  };

  /// Attribute name of main objects.
  inline static const std::string& OBJECTS_ID()
  {
    static const std::string MY_OBJECTS_ID("main_objects");
    return MY_OBJECTS_ID;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void executeCompositeBoolean();

  const OperationType& operationType() const { return myOperationType; }

protected:
  struct ResultBaseAlgo {
    ResultBodyPtr resultBody;
    GeomShapePtr baseShape;
    std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape;
  };

protected:
  FeaturesPlugin_CompositeBoolean(){};

  /// Initializes boolean attributes.
  void initBooleanAttributes();

  /// This function need to be defined for extrusion/revolution generation.
  virtual bool makeGeneration(ListOfShape& theBaseShapes,
                              ListOfMakeShape& theMakeShapes) = 0;

  /// Makes boolean operation.
  /// \param[in] theTools list of tools.
  /// \param[out] theObjects list of objects.
  /// \param[out] theMakeShapes list of according algos.
  /// \return false in failed.
  bool makeBoolean(const ListOfShape& theTools,
                   ListOfShape& theObjects,
                   ListOfMakeShape& theMakeShapes);

  /// Stores generation history.
  virtual void storeGenerationHistory(ResultBodyPtr theResultBody,
                                      const GeomShapePtr theBaseShape,
                                      const GeomMakeShapePtr theMakeShape) = 0;

  /// Stores modification history.
  void storeModificationHistory(ResultBodyPtr theResultBody,
                                const GeomShapePtr theObject,
                                const ListOfShape& theTools,
                                const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);

  /// Stores deleted shapes.
  void storeDeletedShapes(std::vector<ResultBaseAlgo>& theResultBaseAlgoList,
                          const ListOfShape& theTools,
                          const GeomShapePtr theResultShapesCompound);

protected:
  ModelAPI_Feature* myFeature;
  OperationType myOperationType;
};

#endif
