// File:        PartSet_EditLine.h
// Created:     02 Jun 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_EditLine_H
#define PartSet_EditLine_H

#include "PartSet.h"

#include <QObject>
#include <QPoint>

class QLineEdit;
class QWidget;

/*!
 \class PartSet_EditLine
 * \brief The class to give an editor to modify a real value
*/
class PARTSET_EXPORT PartSet_EditLine : public QObject
{
  Q_OBJECT
public:
  /// Constructor
  PartSet_EditLine(QWidget* theParent);
  /// Destructor
  virtual ~PartSet_EditLine() {};

  /// Show the editor in the given global position
  /// \param thePoint a position
  /// \param theValue a value for the editor
  void start(const QPoint& thePoint, double theValue);

  /// Returns the editor value
  /// \return the real value
  double getValue() const;

signals:
  /// Signals about the editing stop
  /// \param theValue the editor value
  void stopped(double theValue);

protected slots:
  /// Slot to check the editing stop
  void onStopEditing();

protected:
  QLineEdit* myEditor; /// the value editor
};

#endif
