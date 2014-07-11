// File:        ModelAPI_ResultPart.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultPart.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>

boost::shared_ptr<ModelAPI_Document> Model_ResultPart::partDoc()
{
  return data()->docRef("PartDocument")->value();
}

boost::shared_ptr<ModelAPI_Feature> Model_ResultPart::owner()
{
  return boost::shared_ptr<ModelAPI_Feature>(); // return empty pointer
}

Model_ResultPart::Model_ResultPart(
const boost::shared_ptr<ModelAPI_Feature>& theFeature)
{
}
