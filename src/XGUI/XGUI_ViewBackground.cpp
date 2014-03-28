#include "XGUI_ViewBackground.h"


/*!
  \brief Default constructor.
  Creates invalid background data.
*/
XGUI_ViewBackground::XGUI_ViewBackground()
  : myTextureMode( XGUI::CenterTexture ), 
  myGradientType( XGUI::NoGradient ), 
  myTextureShown( false )
{
  setMode( XGUI::NoBackground );
}

/*!
  \brief Constructor.
  Creates background data initialized with the specified color
  \param c color
*/
XGUI_ViewBackground::XGUI_ViewBackground( const QColor& theColor )
  : myTextureMode( XGUI::CenterTexture ), 
  myGradientType( XGUI::NoGradient ), 
  myTextureShown( false )
{
  setColor( theColor );
}

/*!
  \brief Constructor.
  Creates background data initialized with the specified two-color gradient
  \param type gradient type identifier
  \param c1 first gradient color
  \param c2 second gradient color
  \note the interpretation of the gradient identifier should be done in the calling code
*/
XGUI_ViewBackground::XGUI_ViewBackground( XGUI::GradientType type, const QColor& c1, const QColor& c2 )
  : myTextureMode( XGUI::CenterTexture ), 
  myGradientType( XGUI::NoGradient ), 
  myTextureShown( false )
{
  setGradient( type, c1, c2 );
}

/*!
  \brief Constructor.
  Creates background data initialized with the arbirtary gradient data
  \param grad gradient data
*/
XGUI_ViewBackground::XGUI_ViewBackground( const QGradient& grad )
  : myTextureMode( XGUI::CenterTexture ), 
  myGradientType( XGUI::NoGradient ), 
  myTextureShown( false )
{
  setGradient( grad );
}

/*!
  \brief Destructor.
*/
XGUI_ViewBackground::~XGUI_ViewBackground()
{
}

/*!
  \brief Compares two background data objects
*/
bool XGUI_ViewBackground::operator==( const XGUI_ViewBackground& other ) const
{
  return 
    ( myMode         == other.myMode )         && 
    ( myTextureMode  == other.myTextureMode )  &&
    ( myFileName     == other.myFileName )     &&
    ( myColors       == other.myColors )       &&
    ( myGradientType == other.myGradientType ) &&
    ( myGradient     == other.myGradient )     &&
    ( myTextureShown == other.myTextureShown );
}

/*!
  \brief Returns \c false if background data is not set (invalid)
  \return \c true if background data is valid or \c false otherwise
  \sa mode()
*/
bool XGUI_ViewBackground::isValid() const
{
  return myMode != XGUI::NoBackground;
}


/*!
  \brief Get file name used as a texture image
  \return path to the texture image file
  \sa setTexture(), setTextureShown()
*/
XGUI::TextureMode XGUI_ViewBackground::texture( QString& fileName ) const
{
  fileName = myFileName;
  return myTextureMode;
}

/*!
  \brief Set file name to be used as a texture image.

  \note To show texture image on the background it is necessary to call additionally
  setTextureShown() method.

  \param fileName path to the texture image file name
  \param m texture mode (CenterTexture by default)
  \sa texture(), setTextureShown()
*/
void XGUI_ViewBackground::setTexture( const QString& fileName, const XGUI::TextureMode m )
{
  myFileName = fileName;
  myTextureMode = m;
}

/*!
  \brief Check if "show texture" flag is switched on
  \return \c true if "show texture" flag is set or \c false otherwise
  \sa setTextureShown(), texture()
*/
bool XGUI_ViewBackground::isTextureShown() const
{
  return myTextureShown;
}

/*!
  \brief Specify if texture should be shown on the background or no.
  \param on \c true if texture should be shown or \c false otherwise
  \sa isTextureShown(), texture()
*/
void XGUI_ViewBackground::setTextureShown( bool on )
{
  myTextureShown = on;
}

/*!
  \brief Get background color. Returns null QColor if color is not set
  \return solid background color
  \sa setColor(), mode()
*/
QColor XGUI_ViewBackground::color() const
{
  return myColors.count() > 0 ? myColors[0] : QColor();
}

/*!
  \brief Set background color and switch to the ColorBackground mode
  \param c color
  \sa color(), mode()
*/
void XGUI_ViewBackground::setColor( const QColor& c )
{
  myColors.clear();
  myColors << c;
  setMode( XGUI::ColorBackground );
}

/*!
  \brief Get simple gradient data.
  Returns -1 and null QColor for \a c1 and \a c2 if gradient data is not set
  \param c1 first gradient color is returned via this parameter
  \param c2 second gradient color is returned via this parameter
  \return current two-colored gradient mode type identifier
  \note the interpretation of the gradient identifier should be done in the calling code
  \sa setGradient(int, const QColor&, const QColor&), mode()
*/
int XGUI_ViewBackground::gradient( QColor& c1, QColor& c2 ) const
{
  c1 = myColors.count() > 0 ? myColors[0] : QColor();
  c2 = myColors.count() > 1 ? myColors[1] : ( myColors.count() > 0 ? myColors[0] : QColor() );
  return myGradientType;
}

/*!
  \brief Set simple background gradient data and switch to the SimpleGradientBackground mode
  \param type two-colored gradient mode type identifier
  \param c1 first gradient color is returned via this parameter
  \param c2 second gradient color is returned via this parameter
  \note the interpretation of the gradient identifier should be done in the calling code
  \sa gradient(QColor&, QColor&), mode()
*/
void XGUI_ViewBackground::setGradient( XGUI::GradientType type, const QColor& c1, const QColor& c2 )
{
  myColors.clear();
  myColors << c1 << c2;
  myGradientType = type;
  setMode( XGUI::SimpleGradientBackground );
}

/*!
  \brief Get complex gradient data.
  Returns QGradient of QGradient::NoGradient if gradient data is not set
  \note This function does not transform simple gradient data set with 
  setGradient( const QString&, const QColor&, const QColor& ) to QGradient class
  \return gradient data
  \sa setGradient(const QGradient&), mode()
*/
const QGradient* XGUI_ViewBackground::gradient() const
{
  return &myGradient;
}

/*!
  \brief Set complex background gradient data and switch to the CustomGradientBackground mode
  \param grad gradient data (QLinearGradient, QRadialGradient or QConicalGradient)
  \sa gradient(), mode()
*/
void XGUI_ViewBackground::setGradient( const QGradient& grad )
{
  myGradient = grad;
  setMode( XGUI::CustomGradientBackground );
}
