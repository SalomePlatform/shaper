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

#include "SketchPlugin_SketchEntity.h"

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

SketchPlugin_SketchEntity::SketchPlugin_SketchEntity()
: SketchPlugin_Feature()
{
}

void SketchPlugin_SketchEntity::initAttributes()
{
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  initDerivedClassAttributes();

  AttributePtr anAttr = data()->addAttribute(SketchPlugin_SketchEntity::COPY_ID(),
                                             ModelAPI_AttributeBoolean::typeId());
  anAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
    SketchPlugin_SketchEntity::COPY_ID());

  anAttr = data()->addAttribute(PARENT_ID(), ModelAPI_AttributeReference::typeId());
  anAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PARENT_ID());

  // initialize the rest of attributes
  initDerivedClassAttributes2();
}
