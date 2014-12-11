// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Result.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Result.h"

bool ModelAPI_Result::isConcealed() {
  return myIsConcealed;
}

void ModelAPI_Result::setIsConcealed(const bool theValue) {
  myIsConcealed = theValue;
}

ModelAPI_Result::ModelAPI_Result() {
}

ModelAPI_Result::~ModelAPI_Result() {
}
