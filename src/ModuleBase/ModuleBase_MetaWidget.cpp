/*
 *
 */

#include <ModuleBase_MetaWidget.h>
#include <QMetaObject>

#ifdef _DEBUG
#include <iostream>
#endif

ModuleBase_MetaWidget::ModuleBase_MetaWidget(const QString& theId,
                                               QWidget* theWrapped,
                                               boost::shared_ptr<ModelAPI_Feature> theFeature)
    : myId(theId), myWrappedWidget(theWrapped), myFeature(theFeature)
{

}

ModuleBase_MetaWidget::~ModuleBase_MetaWidget()
{

}

bool ModuleBase_MetaWidget::storeValue()
{
  #ifdef _DEBUG
  std::cout << "ModuleBase_MetaWidget::storeValue"
            << myWrappedWidget->metaObject()->className() << std::endl;
  #endif
  return true;
}

bool ModuleBase_MetaWidget::restoreValue()
{
  #ifdef _DEBUG
  std::cout << "ModuleBase_MetaWidget::restoreValue"
            << myWrappedWidget->metaObject()->className() << std::endl;
  #endif
  return true;
}
