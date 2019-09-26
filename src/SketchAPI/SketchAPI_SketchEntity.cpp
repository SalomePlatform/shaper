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

#include "SketchAPI_SketchEntity.h"
#include <SketchAPI_Arc.h>
#include <SketchAPI_Circle.h>
#include <SketchAPI_Ellipse.h>
#include <SketchAPI_EllipticArc.h>
#include <SketchAPI_IntersectionPoint.h>
#include <SketchAPI_Line.h>
#include <SketchAPI_Point.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
//--------------------------------------------------------------------------------------
SketchAPI_SketchEntity::SketchAPI_SketchEntity(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_SketchEntity::~SketchAPI_SketchEntity()
{

}

//--------------------------------------------------------------------------------------
bool SketchAPI_SketchEntity::initialize()
{
  SET_ATTRIBUTE(Auxiliary, ModelAPI_AttributeBoolean, SketchPlugin_SketchEntity::AUXILIARY_ID())

  return true;
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_AttributeBoolean> SketchAPI_SketchEntity::auxiliary() const
{
  return myAuxiliary;
}

void SketchAPI_SketchEntity::setAuxiliary(bool theAuxiliary)
{
  fillAttribute(theAuxiliary, myAuxiliary);

  execute();
}

//--------------------------------------------------------------------------------------
void SketchAPI_SketchEntity::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  AttributeBooleanPtr anAux = aBase->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID());
  if (anAux->value()) {
    const std::string& aName = theDumper.name(aBase);
    theDumper << aName << ".setAuxiliary(" << anAux << ")" <<std::endl;
  }
}

bool SketchAPI_SketchEntity::isCopy() const
{
  // check the feature is a copy of another entity
  AttributeBooleanPtr isCopy = feature()->boolean(SketchPlugin_SketchEntity::COPY_ID());
  return isCopy.get() && isCopy->value();
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> >
SketchAPI_SketchEntity::wrap(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures)
{
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > aResult;
  std::list<std::shared_ptr<ModelAPI_Feature> >::const_iterator anIt = theFeatures.begin();
  for (; anIt != theFeatures.end(); ++anIt) {
    if ((*anIt)->getKind() == SketchPlugin_Line::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(new SketchAPI_Line(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Arc::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(new SketchAPI_Arc(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Circle::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(new SketchAPI_Circle(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Ellipse::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(new SketchAPI_Ellipse(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_EllipticArc::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(new SketchAPI_EllipticArc(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_Point::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(new SketchAPI_Point(*anIt)));
    else if ((*anIt)->getKind() == SketchPlugin_IntersectionPoint::ID())
      aResult.push_back(std::shared_ptr<SketchAPI_SketchEntity>(
                                                    new SketchAPI_IntersectionPoint(*anIt)));
  }
  return aResult;
}
