// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef SRC_SKETCHAPI_SKETCHAPI_SKETCHENTITY_H_
#define SRC_SKETCHAPI_SKETCHAPI_SKETCHENTITY_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_SketchEntity.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelAPI_Feature;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_SketchEntity
 * \ingroup CPPHighAPI
 * \brief Base class for Sketch feature interfaces
 */
class SketchAPI_SketchEntity : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_SketchEntity(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_SketchEntity();

  /// Auxiliary
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelAPI_AttributeBoolean> auxiliary() const;

  /// Set auxiliary
  SKETCHAPI_EXPORT
  void setAuxiliary(bool theAuxiliary);

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

  /// Convert list of features to list of appropriate wrappers
  SKETCHAPI_EXPORT
  static std::list<std::shared_ptr<SketchAPI_SketchEntity> >
  wrap(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures);

protected:
  std::shared_ptr<ModelAPI_AttributeBoolean> myAuxiliary;

  bool initialize();

  /// Check the entity is a copy of another feature
  bool isCopy() const;
};

//! Pointer on SketchEntity object
typedef std::shared_ptr<SketchAPI_SketchEntity> SketchEntityPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_SKETCHENTITY_H_ */
