// File:        Model_Update.hxx
// Created:     25 Jun 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Update_HeaderFile
#define Model_Update_HeaderFile

#include "Model.h"
#include "Events_Listener.h"
#include <boost/shared_ptr.hpp>
#include <set>
#include <map>

class ModelAPI_Object;
class ModelAPI_Feature;

/**\class Model_Update
 * \ingroup DataModel
 * \brief Updates the results of features when it is needed.
 */
class Model_Update : public Events_Listener
{
  ///< initial set of updated features that must be processed
  std::set<boost::shared_ptr<ModelAPI_Object> > myInitial;
  ///< already updated and processed features and modificated feature flag
  std::map<boost::shared_ptr<ModelAPI_Object>, bool> myUpdated;
public:
  /// Is called only once, on startup of the application
  Model_Update();

  /// Processes the feature argument update: executes the results
  MODEL_EXPORT virtual void processEvent(const Events_Message* theMessage);

protected:
  /// Recoursively checks and updates the object if needed
  /// Returns true if object was updated.
  bool updateObject(boost::shared_ptr<ModelAPI_Object> theObject);
};

#endif
