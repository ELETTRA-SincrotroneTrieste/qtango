#ifndef TPROPERTYLABEL_H
#define TPROPERTYLABEL_H

#include <QLabel>

class TPropertyLabelPrivate;
class PropertyReader;

/** \brief A QLabel which sets its text according to a device or class property value
  *
  * This widget is a QLabel which sets its text according to a device or class property value.
  *
  * \par Description
  * This class is a QLabel. It is configurable by means of the source property, which defines
  * a device or class property to be read from the Tango database.
  * According to the form of the source, the type of property (device or class) to be
  * retrieved is automatically detected.
  *
  * \par Reading device properties
  * If the source is of type tango/device/name/devicePropertyName, then the devicePropertyName
  * device property is obtained from the device tango/device/name and its value is set as
  * text on the label.
  *
  * \par Reading class properties
  * If the source is of type ClassName/propertyName, then the class property having propertyName
  * as name is read from the tango database and its value is set as text in the label.
  *
  * @see source
  * @see setSource
  *
  * A designer plugin is provided in order to drag and drop a TPropertyLabel into a
  * designer widget
  *
  */
class TPropertyLabel : public QLabel
{
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE false)

    Q_OBJECT

public:

    virtual ~TPropertyLabel();

    explicit TPropertyLabel(QWidget *parent);
    
    QString source() const;

    PropertyReader *propertyReader() const;

signals:
    
public slots:
    void setSource(const QString& src);

private slots:
    void setError(const QString& mess);

private:
    TPropertyLabelPrivate *d_ptr;
    
    Q_DECLARE_PRIVATE(TPropertyLabel)
};

#endif // PROPERTYLABEL_H
