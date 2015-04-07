// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultParameter.cpp
// Created:     02 Apr 2015
// Author:      Sergey BELASH

#include "Model_ResultParameter.h"

Model_ResultParameter::~Model_ResultParameter()
{

}

void Model_ResultParameter::initAttributes()
{
  data()->addAttribute(ModelAPI_ResultParameter::VALUE(),
                       ModelAPI_AttributeDouble::typeId());
}


Model_ResultParameter::Model_ResultParameter()
{
  setIsConcealed(false);
}
