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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SRC_SKETCHAPI_SKETCHAPI_MIRROR_H_
#define SRC_SKETCHAPI_SKETCHAPI_MIRROR_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <list>

#include <SketchPlugin_ConstraintMirror.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Object;
class ModelHighAPI_RefAttr;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Mirror
 * \ingroup CPPHighAPI
 * \brief Interface for Mirror feature
 */
class SketchAPI_Mirror : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Mirror(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Mirror(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const ModelHighAPI_RefAttr & theMirrorLine,
                   const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Mirror();

  INTERFACE_4(SketchPlugin_ConstraintMirror::ID(),
              mirrorLine, SketchPlugin_ConstraintMirror::ENTITY_A(),
              ModelAPI_AttributeRefAttr, /** Mirror line */,
              mirrorList, SketchPlugin_ConstraintMirror::MIRROR_LIST_ID(),
              ModelAPI_AttributeRefList, /** Mirror list */,
              referenceObjects, SketchPlugin_ConstraintMirror::ENTITY_B(),
              ModelAPI_AttributeRefList, /** Reference objects */,
              mirroredObjects, SketchPlugin_ConstraintMirror::ENTITY_C(),
              ModelAPI_AttributeRefList, /** Mirrored objects */
  )

  /// List of mirrored objects
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > mirrored() const;

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Mirror object
typedef std::shared_ptr<SketchAPI_Mirror> MirrorPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_MIRROR_H_ */
