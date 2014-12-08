// File:        ModuleBase_Tools.cpp
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_Tools.h"
#include <QWidget>
#include <QLayout>
#include <QPainter>
#include <QBitmap>

namespace ModuleBase_Tools {

//******************************************************************

//******************************************************************

void adjustMargins(QWidget* theWidget)
{
  if(!theWidget)
    return;
  adjustMargins(theWidget->layout());
}

void adjustMargins(QLayout* theLayout)
{
  if(!theLayout)
    return;
  theLayout->setContentsMargins(2, 5, 5, 2);
  theLayout->setSpacing(4);
}

void zeroMargins(QWidget* theWidget)
{
  if(!theWidget)
    return;
  zeroMargins(theWidget->layout());
}

void zeroMargins(QLayout* theLayout)
{
  if(!theLayout)
    return;
  theLayout->setContentsMargins(0, 0, 0, 0);
  theLayout->setSpacing(5);
}

QPixmap composite(const QString& theAdditionalIcon, const int theXShift,
                  const int theYShift, const QString& theIcon)
{
  QImage anIcon(theIcon);
  QImage anAditional(theAdditionalIcon);

  if (anIcon.isNull())
    return QPixmap();

  int anAddWidth = anAditional.width();
  int anAddHeight = anAditional.height();

  int aWidth = anIcon.width();
  int aHeight = anIcon.height();

  int aStartWidthPos = aWidth-anAddWidth-1;
  int aStartHeightPos = aHeight-anAddHeight-1;

  for (int i = 0; i < anAddWidth; i++)
  {
    for (int j = 0; j < anAddHeight; j++)
    {
      if (qAlpha(anAditional.pixel(i, j)) > 0)
        anIcon.setPixel(i + aStartWidthPos, j + aStartHeightPos, anAditional.pixel(i, j));
    }
  }
  return QPixmap::fromImage(anIcon);
}

QPixmap lighter(const QString& theIcon, const int theLighterValue)
{
  QImage anIcon(theIcon);
  if (anIcon.isNull())
    return QPixmap();

  QImage aResult(theIcon);
  for ( int i = 0; i < anIcon.width(); i++ )
  {
    for ( int j = 0; j < anIcon.height(); j++ )
    {
      QRgb anRgb = anIcon.pixel( i, j );
      QColor aPixelColor(qRed(anRgb), qGreen(anRgb), qBlue(anRgb),
                         qAlpha( aResult.pixel( i, j ) ));

      QColor aLighterColor = aPixelColor.lighter(theLighterValue);
      aResult.setPixel(i, j, qRgba( aLighterColor.red(), aLighterColor.green(),
                                    aLighterColor.blue(), aLighterColor.alpha() ) );
    }
  }
  return QPixmap::fromImage(aResult);
}

}


