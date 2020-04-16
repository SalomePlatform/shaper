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

#ifndef FeaturesPlugin_CompositeSketch_H_
#define FeaturesPlugin_CompositeSketch_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_CompositeFeature.h>

#include <ModelAPI_ResultBody.h>

/// \class FeaturesPlugin_CompositeSketch
/// \ingroup Plugins
/// \brief Interface for the composite sketch feature.
class FeaturesPlugin_CompositeSketch : public ModelAPI_CompositeFeature
{
public:
  /// Attribute name of sketch feature.
  inline static const std::string& SKETCH_ID()
  {
    static const std::string MY_SKETCH_ID("sketch");
    return MY_SKETCH_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base");
    return MY_BASE_OBJECTS_ID;
  }

  /// Appends a feature to the sketch sub-elements container.
  FEATURESPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// \return the number of sub-elements.
  FEATURESPLUGIN_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// \return the sub-feature by zero-base index.
  FEATURESPLUGIN_EXPORT virtual
    std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex, bool forTree = false);

  /// \return the sub-feature unique identifier in this composite feature by zero-base index.
  FEATURESPLUGIN_EXPORT virtual int subFeatureId(const int theIndex) const;

  /// \return true if feature or result belong to this composite feature as subs.
  FEATURESPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after).
  FEATURESPLUGIN_EXPORT virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

protected:
  enum InitFlags {
    InitSketchLauncher   = 1 << 0,
    InitBaseObjectsList  = 1 << 1
  };

  FeaturesPlugin_CompositeSketch(){};

  /// Initializes composite sketch attributes.
  void initCompositeSketchAttribtues(const int theInitFlags);

  /// \brief Returns list of base shapes.
  /// \param[out] theBaseShapesList list of base shapes (warning: list not cleared).
  /// \param[in] theIsMakeShells if true make shells from faces with shared edges.
  void getBaseShapes(ListOfShape& theBaseShapesList, const bool theIsMakeShells = true);

  /// Stores result of generation.
  void storeResult(const GeomShapePtr theBaseShape,
                   const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                   const int theIndex = 0);

  /// Stores generation history.
  void storeGenerationHistory(ResultBodyPtr theResultBody,
                              const GeomShapePtr theBaseShape,
                              const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);

  /// Used to store from and to shapes: generated, or in common modified tag
  void storeShapes(const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                   ResultBodyPtr theResultBody,
                   const GeomAPI_Shape::ShapeType theBaseShapeType,
                   const ListOfShape& theShapes,
                   const std::string theName);

};

#endif
