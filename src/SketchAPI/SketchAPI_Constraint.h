// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef SRC_SKETCHAPI_SKETCHAPI_CONSTRAINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_CONSTRAINT_H_

#include "SketchAPI.h"

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_RefAttr.h>

#include <SketchPlugin_Constraint.h>

class ModelHighAPI_Double;
class SketchAPI_SketchEntity;

/**\class SketchAPI_Constraint
 * \ingroup CPPHighAPI
 * \brief Interface for Constraint feature
 */
class SketchAPI_Constraint : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Constraint(const std::shared_ptr<ModelAPI_Feature> & theFeature);

  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Constraint();

  static std::string ID()
  {
    static const std::string MY_SKETCH_CONSTRAINT_ID = "SketchConstraint";
    return MY_SKETCH_CONSTRAINT_ID;
  }
  virtual std::string getID() { return ID(); }

  SKETCHAPI_EXPORT
  bool initialize();

  SKETCHAPI_EXPORT void setEntityA(const ModelHighAPI_RefAttr& theEntity);
  SKETCHAPI_EXPORT void setEntityB(const ModelHighAPI_RefAttr& theEntity);
  SKETCHAPI_EXPORT void setEntityC(const ModelHighAPI_RefAttr& theEntity);
  SKETCHAPI_EXPORT void setEntityD(const ModelHighAPI_RefAttr& theEntity);

  SKETCHAPI_EXPORT void setValue(const ModelHighAPI_Double& theValue);

  SKETCHAPI_EXPORT double value() const;

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

protected:
  // Check all attributes of constraint are already dumped.
  // If not, store the constraint as postponed.
  bool areAllAttributesDumped(ModelHighAPI_Dumper& theDumper) const;
};

#endif
