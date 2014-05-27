/*
 *
 */

#include <ModuleBase_MetaWidget.h>
#include <QMetaObject>

#ifdef _DEBUG
#include <iostream>
#endif

ModuleBase_MetaWidget::ModuleBase_MetaWidget(QWidget* theWrapped)
    : ModuleBase_ModelWidget(theWrapped->parent()),
      myWrappedWidget(theWrapped)
{

}

ModuleBase_MetaWidget::~ModuleBase_MetaWidget()
{

}

bool ModuleBase_MetaWidget::storeValue(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  #ifdef _DEBUG
  std::cout << "ModuleBase_MetaWidget::storeValue"
            << myWrappedWidget->metaObject()->className() << std::endl;
  #endif
  return true;
}

bool ModuleBase_MetaWidget::restoreValue(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  #ifdef _DEBUG
  std::cout << "ModuleBase_MetaWidget::restoreValue"
            << myWrappedWidget->metaObject()->className() << std::endl;
  #endif
  return true;
}

bool ModuleBase_MetaWidget::focusTo(const std::string& theAttributeName)
{
  #ifdef _DEBUG
  std::cout << "ModuleBase_MetaWidget::focusTo"
            << myWrappedWidget->metaObject()->className() << std::endl;
  #endif
  return true;
}

QList<QWidget*> ModuleBase_MetaWidget::getControls() const
{
  return QList<QWidget*>();
}
