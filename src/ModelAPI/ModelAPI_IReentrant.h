// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_IReentrant.hxx
// Created:     30 Mar 2017
// Author:      Natalia ERMOLAEVA

#ifndef ModelAPI_IReentrant_H
#define ModelAPI_IReentrant_H

#include "ModelAPI.h"

#include <vector>
#include <memory>

class Events_Message;

/** \class ModelAPI_IReentrant
 *  \ingroup DataModel
 *  \brief Interface of a class which can process specific messages
 */
class ModelAPI_IReentrant
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_IReentrant();

  /// Apply information of the message to current object.
  /// \param theMessage a container of information
  /// \return a next active attribute name
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage) = 0;
};

typedef std::shared_ptr<ModelAPI_IReentrant> ModelReentrantPtr;

#endif
