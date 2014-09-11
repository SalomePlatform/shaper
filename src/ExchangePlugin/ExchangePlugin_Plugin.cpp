/*
 *
 */

#include <ExchangePlugin_Plugin.h>
#include <ExchangePlugin_ImportFeature.h>

#include <ModelAPI_Session.h>

#include <boost/smart_ptr/shared_ptr.hpp>

using namespace std;

// the only created instance of this plugin
static ExchangePlugin_Plugin* MY_INSTANCE = new ExchangePlugin_Plugin();

ExchangePlugin_Plugin::ExchangePlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr ExchangePlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == ExchangePlugin_ImportFeature::ID()) {
    return FeaturePtr(new ExchangePlugin_ImportFeature);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
