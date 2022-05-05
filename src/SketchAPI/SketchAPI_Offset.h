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

#ifndef SRC_SKETCHAPI_SKETCHAPI_OFFSET_H_
#define SRC_SKETCHAPI_SKETCHAPI_OFFSET_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <list>

#include <SketchPlugin_Offset.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Object;
class ModelHighAPI_RefAttr;
class ModelHighAPI_Double;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Offset
 * \ingroup CPPHighAPI
 * \brief Interface for Offset feature
 */
class SketchAPI_Offset : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Offset(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Offset(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
                   const ModelHighAPI_Double & theOffsetValue,
                   bool theIsReversed);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Offset();

  INTERFACE_3(SketchPlugin_Offset::ID(),

              edgesList, SketchPlugin_Offset::EDGES_ID(),
              ModelAPI_AttributeRefList, /** Offset edges list */,

              value, SketchPlugin_Offset::VALUE_ID(),
              ModelAPI_AttributeDouble, /** Value */,

              reversed, SketchPlugin_Offset::REVERSED_ID(),
              ModelAPI_AttributeBoolean, /** Negative value */
              )

  /// List of created objects
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > offset() const;

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Offset object
typedef std::shared_ptr<SketchAPI_Offset> OffsetPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_OFFSET_H_ */
