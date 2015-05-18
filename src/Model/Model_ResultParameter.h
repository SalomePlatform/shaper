// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultParameter.h
// Created:     02 Apr 2015
// Author:      Sergey BELASH

#ifndef MODEL_RESULTPARAMETER_H_
#define MODEL_RESULTPARAMETER_H_

#include "Model.h"
#include <Model_Document.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultParameter.h>

/**\class Model_ResultParameter
 * \ingroup DataModel
 * \brief
 */
class Model_ResultParameter : public ModelAPI_ResultParameter
{
 public:
  MODEL_EXPORT virtual ~Model_ResultParameter();
  MODEL_EXPORT virtual void initAttributes();

 protected:
  Model_ResultParameter();

  friend class Model_Objects;
};

#endif
