// File:        Model_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Feature.hxx>

using namespace std;

Model_Feature::Model_Feature()
{
}

string Model_Feature::GetKind()
{
  return "Point";
}
