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

#include "SketcherPrs_DimensionStyle.h"
#include <Prs3d_DimensionAspect.hxx>
#include "SketcherPrs_Tools.h"

#include <Events_Loop.h>

#include <Locale_Convert.h>

#include <AIS_Dimension.hxx>
#include <TCollection_ExtendedString.hxx>

//#ifndef WNT
//  #define COMPILATION_CORRECTION
//#endif

// it is not possible to use 0x2211 as summ symbol because it is not supported by
// debian Linux platform
static const Standard_ExtCharacter MyEmptySymbol(' ');
static const Standard_ExtCharacter MySigmaSymbol('=');//0x03A3); // using equal instead of sigma


SketcherPrs_DimensionStyle::DimensionValue::DimensionValue(double theDoubleValue,
                                     bool theHasParameters, const std::string& theTextValue)
: myDoubleValue(theDoubleValue),
  myHasParameters(theHasParameters),
  myTextValue(theTextValue)
{
}

void SketcherPrs_DimensionStyle::DimensionValue::init(
                                                const AttributeDoublePtr& theAttributeValue)
{
  myDoubleValue = theAttributeValue->value();
  myHasParameters = theAttributeValue->usedParameters().size() > 0;
  myTextValue = Locale::Convert::toString(theAttributeValue->text());
}

SketcherPrs_DimensionStyle::SketcherPrs_DimensionStyle()
{
}

SketcherPrs_DimensionStyle::~SketcherPrs_DimensionStyle()
{
}

void SketcherPrs_DimensionStyle::updateDimensions(AIS_Dimension* theDimension,
          const SketcherPrs_DimensionStyle::DimensionValue& theDimensionValue)
{
  if (!theDimension)
    return;
  updateDimensions(theDimension, theDimensionValue.myHasParameters,
                   theDimensionValue.myTextValue, theDimensionValue.myDoubleValue);
}

void SketcherPrs_DimensionStyle::updateDimensions(AIS_Dimension* theDimension,
                                                          const bool theHasParameters,
                                                          const std::string& theTextValue,
                                                          const double theDoubleValue)
{
  if (!theDimension)
    return;

  /// do not show special symbols of dimension:
  ///   previous implementation did not allow to unite them
  theDimension->SetSpecialSymbol(MyEmptySymbol);
  theDimension->SetDisplaySpecialSymbol(AIS_DSS_No);

  TCollection_ExtendedString aCustomValue;
  if (theHasParameters) {
    //bool isParameterTextStyle = myStyle == SketcherPrs_ParameterStyleMessage::ParameterText;
    bool isParameterTextStyle =
      SketcherPrs_Tools::parameterStyle() == SketcherPrs_Tools::ParameterText;

    if (isParameterTextStyle)
      aCustomValue = theTextValue.c_str();
    else {
      // format value string using "sprintf"
      TCollection_AsciiString aFormatStr =
        theDimension->Attributes()->DimensionAspect()->ValueStringFormat();
      char aFmtBuffer[256];
      sprintf (aFmtBuffer, aFormatStr.ToCString(), theDoubleValue);
      aCustomValue = TCollection_ExtendedString (aFmtBuffer);

      aCustomValue.Insert (1, MySigmaSymbol);
    }
  }
  else {
    // format value string using "sprintf"
    TCollection_AsciiString aFormatStr =
      theDimension->Attributes()->DimensionAspect()->ValueStringFormat();
    char aFmtBuffer[256];
    sprintf (aFmtBuffer, aFormatStr.ToCString(), theDoubleValue);
    aCustomValue = TCollection_ExtendedString (aFmtBuffer);
  }
#ifdef COMPILATION_CORRECTION
  theDimension->SetCustomValue(theDoubleValue);
#else
  theDimension->SetCustomValue(aCustomValue);
#endif
}
