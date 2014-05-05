/*
 * ModuleBase_IModelWidget.h
 *
 *  Created on: Apr 30, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_IMODELWIDGET_H_
#define MODULEBASE_IMODELWIDGET_H_

#include <ModuleBase.h>
#include <QWidget>
#include <QString>

/*
 * Common interface for widgets in the property panel.
 * Every widget are able to save/restore data from the
 * model and/or to contain other widgets.
 *
 * Also, there are some methods to simplify and accelerate
 * searching of children.
 */
class ModuleBase_IModelWidget
{
public:
  //! Interface for saving widget's data into the data model
  MODULEBASE_EXPORT virtual bool storeValue() = 0;
  //! Interface for loading widget's data from the data model
  MODULEBASE_EXPORT virtual bool restoreValue() = 0;
};

#endif /* MODULEBASE_IMODELWIDGET_H_ */
