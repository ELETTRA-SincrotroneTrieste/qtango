#ifndef STYLE_LOADER_H
#define STYLE_LOADER_H

#include <QString>

/** \brief Class to load a css stylesheet to a qtango application.
 *
 * The stylesheet, a css file, is loaded by default from the <em>$SHAREDIR/qtango/stylesheets</em>.
 * The <em> environment variable</em> <strong>QTANGO_STYLESHEET</strong> can be used to override 
 * this path.
 * Moreover, the constructor of EStyleLoader can accept a string containing the path of the 
 * stylesheet. In this case, the string passed to the constructor rules the choice.
 * \par Example
 * \code
 * // main.cpp
 * //
 * QApplication a(argc, argv);
 * EStyleLoader styleLoader;
 * a.setStyleSheet(styleLoader.styleSheet());
 * return a.exec();
 */
class EStyleLoader
{
  public:
	/** \brief the class constructor
	 *
	 * @param filename An optional parameter to specify a custom file name.
	 * 
	 */
	EStyleLoader(const QString& filename = QString());
	
	/** \brief returns the stylesheet loaded after the object has been constructed.
	 *
	 * @return the stylesheet read from the css file or an empty string if no style was specified or
	 *         if the application fails to load the file in read only mode.
	 */
	QString styleSheet() { return d_styleSheet; }
	
  private:
	QString d_styleSheet;
};


#endif
