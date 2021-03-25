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

#include "GDMLAPI_ConeSegment.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
GDMLAPI_ConeSegment::GDMLAPI_ConeSegment(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
GDMLAPI_ConeSegment::GDMLAPI_ConeSegment(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                         const ModelHighAPI_Double& theRMin1,
                                         const ModelHighAPI_Double& theRMax1,
                                         const ModelHighAPI_Double& theRMin2,
                                         const ModelHighAPI_Double& theRMax2,
                                         const ModelHighAPI_Double& theZ,
                                         const ModelHighAPI_Double& theStartPhi,
                                         const ModelHighAPI_Double& theDeltaPhi)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setAttributes(theRMin1, theRMax1, theRMin2, theRMax2, theZ, theStartPhi, theDeltaPhi);
}

//==================================================================================================
GDMLAPI_ConeSegment::~GDMLAPI_ConeSegment()
{
}

//==================================================================================================
void GDMLAPI_ConeSegment::setAttributes(const ModelHighAPI_Double& theRMin1,
                                        const ModelHighAPI_Double& theRMax1,
                                        const ModelHighAPI_Double& theRMin2,
                                        const ModelHighAPI_Double& theRMax2,
                                        const ModelHighAPI_Double& theZ,
                                        const ModelHighAPI_Double& theStartPhi,
                                        const ModelHighAPI_Double& theDeltaPhi)
{
  fillAttribute(theRMin1, rmin1());
  fillAttribute(theRMax1, rmax1());
  fillAttribute(theRMin2, rmin2());
  fillAttribute(theRMax2, rmax2());
  fillAttribute(theZ, z());
  fillAttribute(theStartPhi, startphi());
  fillAttribute(theDeltaPhi, deltaphi());

  execute();
}

//==================================================================================================
void GDMLAPI_ConeSegment::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addConeSegment(" << aDocName;

  AttributeDoublePtr anAttrRMin1 = aBase->real(GDMLPlugin_ConeSegment::RMIN1_ID());
  AttributeDoublePtr anAttrRMax1 = aBase->real(GDMLPlugin_ConeSegment::RMAX1_ID());
  AttributeDoublePtr anAttrRMin2 = aBase->real(GDMLPlugin_ConeSegment::RMIN2_ID());
  AttributeDoublePtr anAttrRMax2 = aBase->real(GDMLPlugin_ConeSegment::RMAX2_ID());
  AttributeDoublePtr anAttrZ = aBase->real(GDMLPlugin_ConeSegment::Z_ID());
  AttributeDoublePtr anAttrStartPhi = aBase->real(GDMLPlugin_ConeSegment::STARTPHI_ID());
  AttributeDoublePtr anAttrDeltaPhi = aBase->real(GDMLPlugin_ConeSegment::DELTAPHI_ID());
  theDumper << ", " << anAttrRMin1 << ", " << anAttrRMax1;
  theDumper << ", " << anAttrRMin2 << ", " << anAttrRMax2;
  theDumper << ", " << anAttrZ << ", " << anAttrStartPhi << ", " << anAttrDeltaPhi;

  theDumper << ")" << std::endl;
}

//==================================================================================================
ConeSegmentPtr addConeSegment(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Double& theRMin1,
                              const ModelHighAPI_Double& theRMax1,
                              const ModelHighAPI_Double& theRMin2,
                              const ModelHighAPI_Double& theRMax2,
                              const ModelHighAPI_Double& theZ,
                              const ModelHighAPI_Double& theStartPhi,
                              const ModelHighAPI_Double& theDeltaPhi)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(GDMLAPI_ConeSegment::ID());
  return ConeSegmentPtr(new GDMLAPI_ConeSegment(aFeature, theRMin1, theRMax1, theRMin2, theRMax2,
                                                theZ, theStartPhi, theDeltaPhi));
}
