// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultCompSolid.cpp
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#include <Model_ResultCompSolid.h>

Model_ResultCompSolid::Model_ResultCompSolid()
{
}

Model_ResultCompSolid::~Model_ResultCompSolid()
{
}

std::shared_ptr<ModelAPI_ResultBody> Model_ResultCompSolid::addResult(std::string theID)
{
  std::shared_ptr<ModelAPI_ResultBody> aResult;
  return aResult;
}

int Model_ResultCompSolid::numberOfSubs(bool forTree) const
{
  return 0;
}

std::shared_ptr<ModelAPI_ResultBody> Model_ResultCompSolid::subResult(const int theIndex,
                                                                      bool forTree) const
{
  std::shared_ptr<ModelAPI_ResultBody> aBody;

  return aBody;
}

int Model_ResultCompSolid::subResultId(const int theIndex) const
{
  return 0;
}

bool Model_ResultCompSolid::isSub(ObjectPtr theObject) const
{
  return true;
}

void Model_ResultCompSolid::removeResult(std::shared_ptr<ModelAPI_ResultBody> theResult)
{
}
