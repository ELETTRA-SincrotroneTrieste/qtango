#ifndef QTANGO_CONFIG_H
#define QTANGO_CONFIG_H

#include <tango.h>
#include <QColor>
#include <QString>
#include <QMap>

#define ATTR_VALID_GREEN (Tango::AttrQuality) (Tango::ATTR_WARNING + 10)

/** \brief A class containing some configurations that might be useful to 
 *  several other objects.
 *
 * Call these methods before the creation of the QTango widgets, i.e. before calling
 * setupUi() method, if you use Qt Designer.
 * 
 * State colours and strings are mapped to the corresponding state as follows (you might
 * want to check configuration.cpp, where initStateColorsAndStrings is implemented.)
 * <ul>
*      <li>Tango::ON ->  "ON";</li>
*      <li>Tango::OFF ->  "OFF";</li>
*     <li>Tango::CLOSE ->  "CLOSED";</li>
*     <li>Tango::OPEN ->  "OPENED";</li>
*      <li>Tango::INSERT ->  "INSERT";</li>
*      <li>Tango::EXTRACT ->  "EXTRACT";</li>
*     <li>Tango::MOVING ->  "MOVING";</li>
*    <li>Tango::STANDBY ->  "STANDBY";</li>
*     <li>Tango::FAULT ->  "FAULT";</li>
*     <li>Tango::INIT -> "INIT";</li>
*      <li>Tango::RUNNING ->  "RUNNING";</li>
*      <li>Tango::ALARM ->  "ALARM";</li>
*      <li>Tango::DISABLE ->  "DISABLE";</li>
*      <li>Tango::UNKNOWN ->  "UNKNOWN";</li>
*
*     <li>Tango::ON ->  EColor(Elettra::green);</li>
*	<li>Tango::OFF ->  Qt::white;</li>
*      <li>Tango::CLOSE -> Qt::white;</li>
*      <li>Tango::OPEN ->  Qt::white;</li>
*      <li>Tango::INSERT ->  Qt::white;</li>
*      <li>Tango::EXTRACT ->  Qt::white;</li>
*      <li>Tango::MOVING ->  EColor(Elettra::blue);</li>
*     <li>Tango::STANDBY ->  EColor(Elettra::yellow);</li>
*     <li>Tango::FAULT -> EColor(Elettra::red);</li>
*     <li>Tango::INIT ->  EColor(Elettra::blue);</li>
*    <li>Tango::RUNNING ->  EColor(Elettra::green);</li>
*	<li>Tango::ALARM -> EColor(Elettra::orange);</li>
*	<li>Tango::DISABLE ->  EColor(Elettra::gray);</li>
*    <li>Tango::UNKNOWN ->  EColor(Elettra::darkGray);</li>
* </ul>
 */
class Config
{
  public:
      static Config* instance();
      
      /**
	 * sets background colors for DevState values, default is yellow
	 * Tango::ON is green, Tango::FAULT and Tango::ALARM are red
	 * @param vec a vector containing 14 colors, one for each state
	 *
	 * <strong>Note</strong>: call this method before creating any QTango widget. 
	 * This means before setupUi() if you are using Qt designer.
	 */
	void setStateColors(QVector<QColor>);
	/**
	 * sets background color for a given state
	 * @param state	the state you want to cofigure
	 * @param color the background color
	 *
	 * <strong>Note</strong>: call this method before creating any QTango widget. 
	 * This means before setupUi() if you are using Qt designer.
	 */
	void setStateColor(Tango::DevState, QColor);
	
	const QColor stateColor(Tango::DevState) const;
	int numStates() const { return d_stateColors.keys().size(); }
	
	const QColor qualityColor(Tango::AttrQuality) const;
	
	/** \brief associates a color to a Tango::AttrQuality.
	 *
	 * <em>Note</em>: a quality named <em>ATTR_VALID_GREEN</em> has been defined for
	 * convenience, to let widgets that want to display a colour different from the white
	 * for ATTR_VALID attributes personalize their ATTR_VALID color.
	 * For instance, QTango TLabel uses ATTR_VALID value in the color map, while gauges
	 * use ATTR_VALID_GREEN to display a green color for ATTR_VALID qualities.
	 * @see setQualityString
	 */
	void setQualityColor(Tango::AttrQuality, QColor);
	
	int numQualities() const { return d_qualityColors.keys().size(); }
	
	void	setStateString(Tango::DevState, QString);
	const   QString	stateString(Tango::DevState) const;
	
	/** \brief associates a string to a Tango AttrQuality
	 * 
	 * Currently, the quality string is not used by any QTango widget.
	 * @see setQualityColor
	 */
	void setQualityString(Tango::AttrQuality, QString);
	QString qualityString(Tango::AttrQuality) const;
	
	QMap<Tango::DevState, QString> stateStrings() { return d_stateStrings; }
	QMap<Tango::DevState, QColor>  stateColors() { return d_stateColors; }
	
	QMap<Tango::AttrQuality, QString> qualityStrings() { return d_qualityStrings; }
	QMap<Tango::AttrQuality, QColor> qualityColors() { return d_qualityColors; }
	
	
	
	/** \brief set the attribute property name for the QTangoAutoConfiguration "values" retrieval
	 *
	 * This is a <strong>work around</strong> to let QTangoAutoConfiguration provide the string list
	 * associated to the attribute property <em>values</em>, even if the attribute property hasn't
	 * got the <em>standard name <em>values</em></em>.
	 */
	void setOverrideValuesAttributePropertyName(const QString& name) { d_valueAttrPropName = name; }
	
	QString valuesAttributePropertyName() { return d_valueAttrPropName; }
          
  protected:
    
  private:
    Config();
    
    void initStateColorsAndStrings();
	void initQualityColorsAndStrings();
    
    QMap<Tango::DevState, QString> d_stateStrings;
    QMap<Tango::DevState, QColor>  d_stateColors;
	
	QMap<Tango::AttrQuality, QString> d_qualityStrings;
	QMap<Tango::AttrQuality, QColor> d_qualityColors;
    
	/* non standard device servers with non standard "values" attribute property name */
	QString d_valueAttrPropName;
	
    static Config* _instance;
};

#endif
