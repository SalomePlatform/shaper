// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Entity.h
// Created:     27 July 2015
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_Entity_H_
#define ModelAPI_Entity_H_

/**\class ModelAPI_Entity
 * \ingroup DataModel
 * \brief Represents a common parent class for Objects and documents.
 * Provided in order to make possible distinguishing of objects and documents
 * by downcasting of their pointers.
 */
class ModelAPI_Entity
{
public:
  /// Empty function which is added for virtualiation of the interface
  virtual void emptyFunction() const {}
};

#endif