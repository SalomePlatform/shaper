// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_BoundingBox.h"

#include <Config_PropManager.h>

#include <FeaturesPlugin_CreateBoundingBox.h>

#include <GeomAlgoAPI_BoundingBox.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <iomanip>
#include <sstream>

//=================================================================================================
FeaturesPlugin_BoundingBox::FeaturesPlugin_BoundingBox()
{
}

//=================================================================================================
void FeaturesPlugin_BoundingBox::initAttributes()
{
  // attribute for object selected
  data()->addAttribute(OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  // attributes for result message and values
  data()->addAttribute(X_MIN_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(Y_MIN_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(Z_MIN_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(X_MAX_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(Y_MAX_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(Z_MAX_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(CREATEBOX_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), X_MIN_COORD_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), Y_MIN_COORD_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), Z_MIN_COORD_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), X_MAX_COORD_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), Y_MAX_COORD_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), Z_MAX_COORD_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CREATEBOX_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), RESULT_VALUES_ID());

  data()->addAttribute(RESULT_VALUES_ID(), ModelAPI_AttributeDoubleArray::typeId());

  data()->realArray(RESULT_VALUES_ID())->setSize(6);

}

//=================================================================================================
void FeaturesPlugin_BoundingBox::execute()
{
  updateValues();
  createBoxByTwoPoints();

  if (boolean(CREATEBOX_ID())->value()) {
    if (!myCreateFeature.get())
      createBox();
    updateBox();
  } else {
    if (myCreateFeature.get()) {
      myCreateFeature->eraseResults();
      SessionPtr aSession = ModelAPI_Session::get();
      DocumentPtr aDoc =  aSession->activeDocument();
      aDoc->removeFeature(myCreateFeature);
      myCreateFeature.reset();
    }
  }
}

//=================================================================================================
void FeaturesPlugin_BoundingBox::attributeChanged(const std::string& theID)
{
  if (theID == OBJECT_ID()) {
    if (myCreateFeature.get())
      updateBox();
  }
}

//=================================================================================================
AttributePtr FeaturesPlugin_BoundingBox::attributResultValues()
{
   return attribute(RESULT_VALUES_ID());
}

//=================================================================================================
void FeaturesPlugin_BoundingBox::updateValues()
{
  AttributeSelectionPtr aSelection = selection(OBJECT_ID());
  if (aSelection->isInitialized()) {
    AttributeDoubleArrayPtr aValues =
      std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(attribute(RESULT_VALUES_ID()));
    std::stringstream streamxmin;
    std::stringstream streamymin;
    std::stringstream streamzmin;
    std::stringstream streamxmax;
    std::stringstream streamymax;
    std::stringstream streamzmax;

    GeomShapePtr aShape;
    if (aSelection && aSelection->isInitialized()) {
      aShape = aSelection->value();
      if (!aShape && aSelection->context())
        aShape = aSelection->context()->shape();
    }

    if (aShape && !aShape->isEqual(myShape)) {
      double aXmin, aXmax, aYmin,aYmax,aZmin,aZmax;
      std::string aError;
      if (!GetBoundingBox(aShape,
                          true,
                          aXmin, aXmax,
                          aYmin,aYmax,
                          aZmin,aZmax,
                          aError))
          setError("Error in bounding box calculation :" +  aError);

      myShape = aShape;
      streamxmin << std::setprecision(14) << aXmin;
      aValues->setValue(0, aXmin);
      streamxmax << std::setprecision(14) << aXmax;
      aValues->setValue(1, aXmax);
      streamymin << std::setprecision(14) << aYmin;
      aValues->setValue(2, aYmin);
      streamymax << std::setprecision(14) << aYmax;
      aValues->setValue(3, aYmax);
      streamzmin << std::setprecision(14) << aZmin;
      aValues->setValue(4, aZmin);
      streamzmax << std::setprecision(14) << aZmax;
      aValues->setValue(5, aZmax);
      string(X_MIN_COORD_ID() )->setValue( "X = " +  streamxmin.str() );
      string(Y_MIN_COORD_ID() )->setValue( "Y = " +  streamymin.str() );
      string(Z_MIN_COORD_ID() )->setValue( "Z = " +  streamzmin.str() );
      string(X_MAX_COORD_ID() )->setValue( "X = " +  streamxmax.str() );
      string(Y_MAX_COORD_ID() )->setValue( "Y = " +  streamymax.str() );
      string(Z_MAX_COORD_ID() )->setValue( "Z = " +  streamzmax.str() );
    }
  }
}

//=================================================================================================
void FeaturesPlugin_BoundingBox::createBox()
{
  SessionPtr aSession = ModelAPI_Session::get();

  DocumentPtr aDoc =  aSession->activeDocument();

  if (aDoc.get()) {
    myCreateFeature = aDoc->addFeature(FeaturesPlugin_CreateBoundingBox::ID());
  }
}

//=================================================================================================
void FeaturesPlugin_BoundingBox::updateBox()
{
  myCreateFeature->boolean(FeaturesPlugin_CreateBoundingBox::COMPUTE_ID())->setValue(false);
  myCreateFeature->selection(FeaturesPlugin_CreateBoundingBox::OBJECT_ID())
                        ->setValue(selection(OBJECT_ID())->context(),
                                   selection(OBJECT_ID())->value());

  AttributeDoubleArrayPtr aValuesFeatures =
    std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>
                              (myCreateFeature->attribute(RESULT_VALUES_ID()));
  AttributeDoubleArrayPtr aValues =
    std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(attribute(RESULT_VALUES_ID()));
  for (int anI=0; anI < 6; anI++)
    aValuesFeatures->setValue(anI,aValues->value(anI));

  myCreateFeature->execute();
  myCreateFeature->boolean(FeaturesPlugin_CreateBoundingBox::COMPUTE_ID())->setValue(true);
}
