
#ifndef PYINTERP_WATCHER_H
#define PYINTERP_WATCHER_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "PyInterp_Dispatcher.h"

#include <QObject>

// Private class that keeps track of destructions of request listeners
class PYINTERP_EXPORT PyInterp_Watcher : public QObject
{                                           
  Q_OBJECT

public:
  PyInterp_Watcher() : QObject( 0 ) {}
  virtual ~PyInterp_Watcher() {}

public slots:
  void onDestroyed( QObject* o ) { PyInterp_Dispatcher::Get()->objectDestroyed( o ); }
};

#endif // PYINTERP_WATCHER_H
