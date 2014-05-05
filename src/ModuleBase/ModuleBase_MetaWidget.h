/*
 *
 */

#ifndef MODULEBASE_METAWIDGET_H_
#define MODULEBASE_METAWIDGET_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <ModelAPI_Feature.h>

#include <QWidget>

#include <boost/shared_ptr.hpp>

/*
 *
 */
class ModuleBase_MetaWidget : public ModuleBase_ModelWidget
{
public:
  MODULEBASE_EXPORT ModuleBase_MetaWidget(QWidget* theWrapped);
  virtual ~ModuleBase_MetaWidget();
  //! Interface for saving widget's data into the data model
  MODULEBASE_EXPORT virtual bool storeValue(boost::shared_ptr<ModelAPI_Feature> theFeature);
  //! Interface for loading widget's data from the data model
  MODULEBASE_EXPORT virtual bool restoreValue(boost::shared_ptr<ModelAPI_Feature> theFeature);

private:
  QWidget* myWrappedWidget;
};

#endif /* MODULEBASE_METAWIDGET_H_ */
