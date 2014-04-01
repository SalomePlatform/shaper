#include "XGUI_RubberBand.h"

#include <QBitmap>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QShowEvent>
#include <QVectorIterator>

/*!
 \class QtxAbstractRubberBand
 \brief Analog of class QRubberBand with possibility of creation non-rectangular contour for selection.
 
 Currently this class does not support Style functionality in full.
 */

/*!
 \brief Constructor
 \param theParent parent widget
 */

XGUI_AbstractRubberBand::XGUI_AbstractRubberBand(QWidget* theParent)
    : QWidget(theParent), myPoints(), myIsClosed(false)
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
#ifndef WIN32
  setAttribute(Qt::WA_NoSystemBackground);
#endif //WIN32  setAttribute(Qt::WA_WState_ExplicitShowHide);  setVisible(false);  theParent->installEventFilter(this);
  setGeometry(QRect(QPoint(0, 0), theParent->size()));
}

/*!
 \brief Destructor
 */
XGUI_AbstractRubberBand::~XGUI_AbstractRubberBand()
{
}

void XGUI_AbstractRubberBand::clearGeometry()
{
  myPoints.clear();
}

bool XGUI_AbstractRubberBand::isClosed()
{
  return myIsClosed;
}

void XGUI_AbstractRubberBand::paintEvent(QPaintEvent* theEvent)
{
  if (!myPoints.empty()) {
    QPixmap tiledPixmap(16, 16);

    QPainter pixmapPainter(&tiledPixmap);
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.setBrush(QBrush(Qt::black, Qt::Dense4Pattern));
    pixmapPainter.setBackground(QBrush(Qt::white));
    pixmapPainter.setBackgroundMode(Qt::OpaqueMode);
    pixmapPainter.drawRect(0, 0, tiledPixmap.width(), tiledPixmap.height());
    pixmapPainter.end();
    // ### workaround for borked XRENDER
    tiledPixmap = QPixmap::fromImage(tiledPixmap.toImage());

    QPainter aPainter(this);
    aPainter.setRenderHint(QPainter::Antialiasing);
    QRect r = myPoints.boundingRect();
    aPainter.setClipRegion(r.normalized().adjusted(-1, -1, 2, 2));
    aPainter.drawTiledPixmap(0, 0, width(), height(), tiledPixmap);

    aPainter.end();
  }
}

void XGUI_AbstractRubberBand::showEvent(QShowEvent* theEvent)
{
  raise();
  theEvent->ignore();
}

void XGUI_AbstractRubberBand::moveEvent(QMoveEvent*)
{
}

void XGUI_AbstractRubberBand::resizeEvent(QResizeEvent*)
{
}

bool XGUI_AbstractRubberBand::eventFilter(QObject* obj, QEvent* e)
{
  if (obj && obj == parent() && e->type() == QEvent::Resize) {
    QWidget* p = (QWidget*) parent();
    setGeometry(QRect(QPoint(0, 0), p->size()));
  }
  return QWidget::eventFilter(obj, e);
}

QRegion createRegion(const QPointF& p1, const QPointF& p2)
{
  if (p1 == p2)
    return QRegion();

  QLineF n = QLineF(p1, p2).normalVector(); //.unitVector();
  n.setLength(1);
  n.translate(p1 * -1);
  QPointF nPoint = n.p2();

  QPolygonF p;
  p << p1 + nPoint << p2 + nPoint << p2 - nPoint << p1 - nPoint << p1 + nPoint;

  return QRegion(p.toPolygon());
}

void XGUI_AbstractRubberBand::updateMask()
{
  QRegion r;

  QVectorIterator<QPoint> it(myPoints);
  while(it.hasNext()) {
    QPoint p = it.next();
    if (!it.hasNext())
      break;

    QPoint np = it.peekNext();

    if (p == np)
      continue;

    r += createRegion(p, np);
  }

  if (isClosed())
    r += createRegion(myPoints.last(), myPoints.first());

  setMask(r);
}

//**********************************************************
XGUI_RectRubberBand::XGUI_RectRubberBand(QWidget* parent)
    : XGUI_AbstractRubberBand(parent)
{
  myPoints.resize(4);
  myIsClosed = true;
}

XGUI_RectRubberBand::~XGUI_RectRubberBand()
{
}

void XGUI_RectRubberBand::initGeometry(const QRect& theRect)
{
  myPoints.clear();
  myPoints << theRect.topLeft() << theRect.topRight() << theRect.bottomRight()
      << theRect.bottomLeft();
  //setMask( QRegion( myPoints ) );
  updateMask();
}

void XGUI_RectRubberBand::setStartPoint(const QPoint& thePoint)
{
  myPoints[0] = thePoint;
  myPoints[1].setY(thePoint.y());
  myPoints[3].setX(thePoint.x());
  updateMask();
}

void XGUI_RectRubberBand::setEndPoint(const QPoint& thePoint)
{
  myPoints[2] = thePoint;
  myPoints[1].setX(thePoint.x());
  myPoints[3].setY(thePoint.y());
  updateMask();
}

void XGUI_RectRubberBand::clearGeometry()
{
  QMutableVectorIterator<QPoint> i(myPoints);
  while(i.hasNext()) {
    i.next();
    i.setValue(QPoint(-1, -1));
  }
}

//**********************************************************
XGUI_PolyRubberBand::XGUI_PolyRubberBand(QWidget* parent)
    : XGUI_AbstractRubberBand(parent)
{
}

XGUI_PolyRubberBand::~XGUI_PolyRubberBand()
{
}

void XGUI_PolyRubberBand::initGeometry(const QPolygon& thePoints)
{
  myPoints = thePoints;
  updateMask();
}

void XGUI_PolyRubberBand::initGeometry(const QPoint& thePoint)
{
  myPoints.clear();
  myPoints << thePoint;
  updateMask();
}

void XGUI_PolyRubberBand::addNode(const QPoint& thePoint)
{
  myPoints << thePoint;
  updateMask();
}

void XGUI_PolyRubberBand::replaceLastNode(const QPoint& thePoint)
{
  if (!myPoints.empty()) {
    myPoints.pop_back();
    myPoints << thePoint;
    updateMask();
  }
}

void XGUI_PolyRubberBand::removeLastNode()
{
  if (!myPoints.empty()) {
    myPoints.pop_back();
    updateMask();
  }
}

void XGUI_PolyRubberBand::setClosed(bool theFlag)
{
  if (myIsClosed != theFlag) {
    myIsClosed = theFlag;
    updateMask();
  }
}
