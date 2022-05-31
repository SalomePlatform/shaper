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

#include "GDMLAPI_Ellipsoid.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

GDMLAPI_Ellipsoid::GDMLAPI_Ellipsoid(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

GDMLAPI_Ellipsoid::GDMLAPI_Ellipsoid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Double& theAX,
                                     const ModelHighAPI_Double& theBY,
                                     const ModelHighAPI_Double& theCZ)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setSizes(theAX, theBY, theCZ);
    fillAttribute("", useZCut1());
    fillAttribute("", useZCut2());
  }
}

GDMLAPI_Ellipsoid::GDMLAPI_Ellipsoid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Double& theAX,
                                     const ModelHighAPI_Double& theBY,
                                     const ModelHighAPI_Double& theCZ,
                                     const ModelHighAPI_Double& theZCut1,
                                     const ModelHighAPI_Double& theZCut2)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setSizes(theAX, theBY, theCZ);
    setZCut1(theZCut1);
    setZCut2(theZCut2);
  }
}

GDMLAPI_Ellipsoid::~GDMLAPI_Ellipsoid()
{
}

void GDMLAPI_Ellipsoid::setSizes(const ModelHighAPI_Double& theAX,
                                 const ModelHighAPI_Double& theBY,
                                 const ModelHighAPI_Double& theCZ)
{
  fillAttribute(theAX, ax());
  fillAttribute(theBY, by());
  fillAttribute(theCZ, cz());

  execute();
}

void GDMLAPI_Ellipsoid::setZCut1(const ModelHighAPI_Double& theZCut1)
{
  fillAttribute("true", useZCut1());
  fillAttribute(theZCut1, zCut1());

  execute();
}

void GDMLAPI_Ellipsoid::setZCut2(const ModelHighAPI_Double& theZCut2)
{
  fillAttribute("true", useZCut2());
  fillAttribute(theZCut2, zCut2());

  execute();
}


//==================================================================================================
void GDMLAPI_Ellipsoid::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeDoublePtr anAttrAX = aBase->real(GDMLPlugin_Ellipsoid::AX_ID());
  AttributeDoublePtr anAttrBY = aBase->real(GDMLPlugin_Ellipsoid::BY_ID());
  AttributeDoublePtr anAttrCZ = aBase->real(GDMLPlugin_Ellipsoid::CZ_ID());

  theDumper << aBase << " = model.addEllipsoid(" << aDocName << ", "
            << anAttrAX << ", " << anAttrBY << ", " << anAttrCZ;

  AttributeStringPtr anAttrUseZCut1 = aBase->string(GDMLPlugin_Ellipsoid::USE_ZCUT1_ID());
  AttributeDoublePtr anAttrZCut1 = aBase->real(GDMLPlugin_Ellipsoid::ZCUT1_ID());
  AttributeStringPtr anAttrUseZCut2 = aBase->string(GDMLPlugin_Ellipsoid::USE_ZCUT2_ID());
  AttributeDoublePtr anAttrZCut2 = aBase->real(GDMLPlugin_Ellipsoid::ZCUT2_ID());

  bool isZCut1 = !anAttrUseZCut1->value().empty();
  bool isZCut2 = !anAttrUseZCut2->value().empty();
  if (isZCut1 && isZCut2)
    theDumper << ", " << anAttrZCut1 << ", " << anAttrZCut2 << ")" << std::endl;
  else {
    theDumper << ")" << std::endl;

    if (isZCut1 && !isZCut2)
      theDumper << theDumper.name(aBase) << ".setZCut1(" << anAttrZCut1 << ")" << std::endl;
    else if (!isZCut1 && isZCut2)
      theDumper << theDumper.name(aBase) << ".setZCut2(" << anAttrZCut2 << ")" << std::endl;
  }
}


//==================================================================================================
EllipsoidPtr addEllipsoid(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Double& theAX,
                          const ModelHighAPI_Double& theBY,
                          const ModelHighAPI_Double& theCZ)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(GDMLAPI_Ellipsoid::ID());
  return EllipsoidPtr(new GDMLAPI_Ellipsoid(aFeature, theAX, theBY, theCZ));
}

EllipsoidPtr addEllipsoid(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Double& theAX,
                          const ModelHighAPI_Double& theBY,
                          const ModelHighAPI_Double& theCZ,
                          const ModelHighAPI_Double& theZCut1,
                          const ModelHighAPI_Double& theZCut2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(GDMLAPI_Ellipsoid::ID());
  return EllipsoidPtr(new GDMLAPI_Ellipsoid(aFeature, theAX, theBY, theCZ, theZCut1, theZCut2));
}
