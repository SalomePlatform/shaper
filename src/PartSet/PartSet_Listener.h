// File:        PartSet_Listener.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_Listener_H
#define PartSet_Listener_H

#include "PartSet.h"

#include <Events_Listener.h>

class PartSet_Module;

/*!
 \class PartSet_Listener
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_Listener : public Events_Listener
{
public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  PartSet_Listener(PartSet_Module* theModule);
  /// Destructor
  virtual ~PartSet_Listener();

  /// This method is called by loop when the event is started to process.
  virtual void processEvent(const Events_Message* theMessage);

private:
  PartSet_Module* myModule; ///< the current module
};

#endif
