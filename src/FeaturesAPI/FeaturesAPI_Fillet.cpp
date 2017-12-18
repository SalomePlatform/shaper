// Copyright (C) 2017  CEA/DEN, EDF R&D
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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesAPI_Fillet.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
////#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

FeaturesAPI_Fillet::FeaturesAPI_Fillet(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FeaturesAPI_Fillet::FeaturesAPI_Fillet(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                       const ModelHighAPI_Double& theRadius)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_SINGLE_RADIUS(), mycreationMethod);
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theRadius, myradius);

    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Fillet::FeaturesAPI_Fillet(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                       const ModelHighAPI_Double& theRadius1,
                                       const ModelHighAPI_Double& theRadius2)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_VARYING_RADIUS(), mycreationMethod);
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theRadius1, mystartRadius);
    fillAttribute(theRadius2, myendRadius);

    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Fillet::~FeaturesAPI_Fillet()
{
}

//==================================================================================================
void FeaturesAPI_Fillet::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet::setRadius(const ModelHighAPI_Double& theRadius)
{
  fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_SINGLE_RADIUS(), mycreationMethod);
  fillAttribute(theRadius, myradius);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet::setRadius(const ModelHighAPI_Double& theRadius1,
                                   const ModelHighAPI_Double& theRadius2)
{
  fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_VARYING_RADIUS(), mycreationMethod);
  fillAttribute(theRadius1, mystartRadius);
  fillAttribute(theRadius2, myendRadius);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Fillet::OBJECT_LIST_ID());

  theDumper << aBase << " = model.addFillet(" << aDocName << ", " << anAttrObjects;

  std::string aCreationMethod = aBase->string(FeaturesPlugin_Fillet::CREATION_METHOD())->value();

  if(aCreationMethod == FeaturesPlugin_Fillet::CREATION_METHOD_SINGLE_RADIUS()) {
    AttributeDoublePtr anAttrRadius = aBase->real(FeaturesPlugin_Fillet::RADIUS_ID());
    theDumper << ", " << anAttrRadius;
  } else if(aCreationMethod == FeaturesPlugin_Fillet::CREATION_METHOD_VARYING_RADIUS()) {
    AttributeDoublePtr anAttrRadius1 = aBase->real(FeaturesPlugin_Fillet::START_RADIUS_ID());
    AttributeDoublePtr anAttrRadius2 = aBase->real(FeaturesPlugin_Fillet::END_RADIUS_ID());
    theDumper << ", " << anAttrRadius1 << ", " << anAttrRadius2;
  }

  theDumper << ")" << std::endl;
}

void FeaturesAPI_Fillet::execIfBaseNotEmpty()
{
  if (mybaseObjects->size() > 0)
    execute();
}


//==================================================================================================

FilletPtr addFillet(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const ModelHighAPI_Double& theRadius)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Fillet::ID());
  return FilletPtr(new FeaturesAPI_Fillet(aFeature, theBaseObjects, theRadius));
}

FilletPtr addFillet(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const ModelHighAPI_Double& theRadius1,
                    const ModelHighAPI_Double& theRadius2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Fillet::ID());
  return FilletPtr(new FeaturesAPI_Fillet(aFeature, theBaseObjects, theRadius1, theRadius2));
}
