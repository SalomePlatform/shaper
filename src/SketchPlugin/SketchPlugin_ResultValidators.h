// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        Model_ResultValidators.h
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Model_ResultValidators_H
#define Model_ResultValidators_H

#include <SketchPlugin.h>
#include <ModelAPI_ResultValidator.h>
#include <ModelAPI_Object.h>

class SketchPlugin_ResultPointValidator : public ModelAPI_ResultValidator
{
 public:
  SKETCHPLUGIN_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

class SketchPlugin_ResultLineValidator : public ModelAPI_ResultValidator
{
 public:
  SKETCHPLUGIN_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

class SketchPlugin_ResultArcValidator : public ModelAPI_ResultValidator
{
 public:
  SKETCHPLUGIN_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

#endif
