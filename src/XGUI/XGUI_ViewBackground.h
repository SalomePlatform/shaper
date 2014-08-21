#ifndef XGUI_ViewBackground_H
#define XGUI_ViewBackground_H

#include "XGUI.h"
#include "XGUI_Constants.h"

#include <QColor>
#include <QGradient>
#include <QString>
#include <QCompleter>

typedef QList<QColor> QColorList;     //!< list of colors

/*!
 \class XGUI_ViewBackground
 \brief Stores background data

 This class is used to store background data. Depending on the mode,
 the background can be specified by:
 - image (by assigning the file name to be used as background texture), see setTexture(), setTextureShown()
 - single color (by assigning any color), see setColor()
 - simple two-color gradient (with the gradient type id and two colors), see setGradient( int, const QColor&, const QColor& )
 - complex gradient (by assigning arbitrary gradient data), see setGradient( const QGradient& )

 The class stores all the data passed to it, so switching between different modes can be done
 just by calling setMode() function.

 \note Texture is used with combination of the background mode.

 \note Two-color gradient is specified by two colors and integer identifier. The interpretation of 
 this identifier should be done in the calling code.

 \code
 XGUI_ViewBackground bg;
 bg.setColor( QColor(100, 100, 100) );                  // bg is switched to ColorBackground mode
 bg.setGradient( Qt::Horizontal, Qt::gray, Qt::white ); // bg is switched to ColorBackground mode
 QLinearGradient grad( 0,0,1,1 ); 
 grad.setColorAt( 0.0, Qt::gray ); 
 grad.setColorAt( 0.5, Qt::white );
 grad.setColorAt( 1.0, Qt::green );
 grad.setSpread( QGradient::PadSpread );
 bg.setGradient( grad );                                // bg is switched to CustomGradientBackground mode
 bg.setMode( ColorBackground );                    // bg is switched back to ColorBackground mode
 bg.setTexture( "/data/images/background.png" );        // specify texture (in the centered mode by default)
 bg.setTextureShown( true );                            // draw texture on the solid color background
 \endcode
 */
class XGUI_EXPORT XGUI_ViewBackground
{
 public:
  XGUI_ViewBackground();
  XGUI_ViewBackground(const QColor& theColor);
  XGUI_ViewBackground(XGUI::GradientType type, const QColor& theColor1, const QColor& theColor2);
  XGUI_ViewBackground(const QGradient&);
  virtual ~XGUI_ViewBackground();

  bool operator==(const XGUI_ViewBackground&) const;
  inline bool operator!=(const XGUI_ViewBackground& other) const
  {
    return !operator==(other);
  }

  bool isValid() const;

  /*!
   \brief Get background mode
   \return current background mode
   \sa setMode()
   */
  XGUI::BackgroundMode mode() const
  {
    return myMode;
  }

  /*!
   \brief Set background mode
   \param m background mode being set
   \sa mode()
   */
  void setMode(const XGUI::BackgroundMode m)
  {
    myMode = m;
  }

  XGUI::TextureMode texture(QString&) const;
  void setTexture(const QString&, XGUI::TextureMode = XGUI::CenterTexture);
  bool isTextureShown() const;
  void setTextureShown(bool);

  QColor color() const;
  void setColor(const QColor&);

  int gradient(QColor&, QColor&) const;
  void setGradient(XGUI::GradientType, const QColor&, const QColor&);

  const QGradient* gradient() const;
  void setGradient(const QGradient&);

 private:
  XGUI::BackgroundMode myMode;
  XGUI::TextureMode myTextureMode;
  QString myFileName;
  QColorList myColors;
  XGUI::GradientType myGradientType;
  QGradient myGradient;
  bool myTextureShown;
};

#endif
