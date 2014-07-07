// File:        PartSet_WidgetSketchLabel.h
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_WidgetSketchLabel_H
#define PartSet_WidgetSketchLabel_H

#include "PartSet.h"

#include <ModuleBase_ModelWidget.h>

class QLabel;
class XGUI_OperationMgr; 

class PARTSET_EXPORT PartSet_WidgetSketchLabel : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  PartSet_WidgetSketchLabel(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetSketchLabel() {};

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const { return true;}

  virtual bool restoreValue(FeaturePtr theFeature) { return true;}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  QWidget* getControl() const;

  void setOperationsMgr(XGUI_OperationMgr* theMgr);

private slots:
  void onPlaneSelected();

private:
  QLabel* myLabel;
  QString myText;
  QString myTooltip;
};

#endif