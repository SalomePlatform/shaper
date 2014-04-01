// File:        Model_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Feature.h>

using namespace std;

Model_Feature::Model_Feature()
{
}

void Model_Feature::setLabel(TDF_Label& theLab)
{
  myLab = theLab;
}
