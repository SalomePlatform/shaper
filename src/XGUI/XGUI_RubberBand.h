#ifndef XGUI_RubberBand_H
#define XGUI_RubberBand_H

#include "XGUI.h"
#include <QWidget>

/*!
 \class XGUI_AbstractRubberBand
 \ingroup GUI
 \brief Analog of class QRubberBand with possibility of creation non-rectangular contour for selection.
 
 Currently this class does not support Style functionality in full.
 */
class XGUI_EXPORT XGUI_AbstractRubberBand : public QWidget
{
Q_OBJECT
 protected:
  XGUI_AbstractRubberBand(QWidget*);

 public:
  virtual ~XGUI_AbstractRubberBand();

  virtual void clearGeometry();

  bool isClosed();

 protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void showEvent(QShowEvent*);
  virtual void moveEvent(QMoveEvent*);
  virtual void resizeEvent(QResizeEvent*);

  virtual bool eventFilter(QObject*, QEvent*);

  virtual void updateMask();

 protected:
  QPolygon myPoints;

  bool myIsClosed;
};

/*!
 \class XGUI_RectRubberBand
 \ingroup GUI
 \brief Analog of class QRubberBand with possibility of creation non-rectangular contour for selection.
 
 Redefinition for rectangular rubber band
 */
class XGUI_RectRubberBand : public XGUI_AbstractRubberBand
{
Q_OBJECT

 public:
  XGUI_RectRubberBand(QWidget*);
  virtual ~XGUI_RectRubberBand();

  void initGeometry(const QRect&);
  void setStartPoint(const QPoint&);
  void setEndPoint(const QPoint&);

  virtual void clearGeometry();
};

class XGUI_PolyRubberBand : public XGUI_AbstractRubberBand
{
Q_OBJECT

 public:
  XGUI_PolyRubberBand(QWidget*);
  virtual ~XGUI_PolyRubberBand();

  void initGeometry(const QPolygon&);
  void initGeometry(const QPoint&);

  void addNode(const QPoint&);
  void replaceLastNode(const QPoint&);
  void removeLastNode();

  void setClosed(bool);
};

#endif
