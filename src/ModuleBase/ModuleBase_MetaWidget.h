/*
 * ModuleBase_IModelWidget.h
 *
 *  Created on: Apr 30, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_METAWIDGET_H_
#define MODULEBASE_METAWIDGET_H_

#include <ModuleBase_IModelWidget.h>
#include <ModuleBase.h>

#include <ModelAPI_Feature.h>

#include <QWidget>
#include <QString>

#include <boost/shared_ptr.hpp>

/*
 *
 */
class ModuleBase_MetaWidget : public ModuleBase_IModelWidget
{
public:
  MODULEBASE_EXPORT ModuleBase_MetaWidget(const QString& theId,
                                           QWidget* theWrapped,
                                           boost::shared_ptr<ModelAPI_Feature>);
  virtual ~ModuleBase_MetaWidget();
  //! Interface for saving widget's data into the data model
  MODULEBASE_EXPORT virtual bool storeValue();
  //! Interface for loading widget's data from the data model
  MODULEBASE_EXPORT virtual bool restoreValue();

private:
  QString  myId;
  QWidget* myWrappedWidget;
  boost::shared_ptr<ModelAPI_Feature> myFeature;
};

#endif /* MODULEBASE_METAWIDGET_H_ */
