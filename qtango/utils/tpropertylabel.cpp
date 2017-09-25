#include "tpropertylabel.h"
#include <utils/ttextdbproperty.h>
#include <propertyreader.h>
#include <macros.h>

class TPropertyLabelPrivate
{
public:
    TPropertyLabelPrivate() {}

    QString source;

    TTextDbProperty *tTextDbProperty;
};


TPropertyLabel::~TPropertyLabel()
{
    if(d_ptr->tTextDbProperty)
        delete d_ptr->tTextDbProperty;
}

/** \brief The constructor of a TPropertyLabel
  *
  * @param parent a QWidget which is the parent of the TPropertyLabel
  *
  * @see setSource
  * @see source
  *
  * If it is not possible to correctly retrieve the device or class property
  * specified by source, then this widget is disabled and its tooltip describes
  * the problem in brief, just as each other QTango widget.
  *
  * \par Object implementation.
  * The TPropertyLabel uses TTextDbProperty and PropertyReader as underlying
  * objects in order to retrieve the property values from the database.
  * You might want to read their documentation as well.
  * To retrieve a reference to the PropertyReader just call propertyReader.
  *
  * \par Note
  * TPropertyReader always read device or class properties in a different thread.
  *
  * @see propertyReader
  * @see PropertyReader
  * @see TTextDbProperty
  *
  */
TPropertyLabel::TPropertyLabel(QWidget *parent) :
    QLabel(parent)
{
    d_ptr = new TPropertyLabelPrivate();
    d_ptr->tTextDbProperty = NULL;
    setText("No Link");
    setToolTip("Property Label");
}

void TPropertyLabel::setError(const QString& mess)
{
    this->setDisabled(true);
    this->setToolTip(mess);
}

/** \brief returns the source of the TPropertyLabel
  *
  * @return the string set with setSource
  *
  * @see setSource
  */
QString TPropertyLabel::source() const
{
    return d_ptr->source;
}

/** \brief Obtain a reference to the underlying PropertyReader.
  *
  *
  * @return the PropertyReader used by this object if setSource was
  *         already called, NULL otherwise.
  *
  * @see PropertyReader
  */
PropertyReader *TPropertyLabel::propertyReader() const
{
    if(d_ptr->tTextDbProperty)
        return d_ptr->tTextDbProperty->propertyReader();
    return NULL;
}

/** \brief Configures the label in order to retrieve the value of the device or class
  *        property specified by the src parameter.
  *
  * Configures the label in order to retrieve the value of the device or class
  * property specified by the src parameter.
  *
  * @param src the source in the form a/b/c/propName for device properties and
  *        ClassName/propName for class properties
  *
  * \par Example
  * \code
  * label->setSource("test/device/1/helperApplication");
  * \endcode
  * Will retrieve the helperApplication device property from the tango database.
  * \code
  * label->setSource("TangoTest/cvs_location");
  * \endcode
  * Will retrieve the "cvs_location" class property from the TangoTest class.
  *
  * \par Qt Designer plugin
  * The TPropertyLabel is available through the Qt designer plugins for QTango.
  * You can drag and drop a TPropertyLabel from the QTango object list and
  * configure the source as described here.
  *
  * @see source
  *
  * \par Note
  * If it is not possible to correctly retrieve the device or class property
  * specified by source, then this widget is disabled and its tooltip describes
  * the problem in brief, just as each other QTango widget.
  *
  */
void TPropertyLabel::setSource(const QString &src)
{

    QString sourceName, propName;
    QStringList l;

    if(src.isEmpty())
    {
        perr("PropertyLabel::setSource: src is empty");
        return;
    }

    setDisabled(false);
    if(d_ptr->tTextDbProperty)
    {
        printf("\e[1;35mdeleting previous tTextDbProperty\e[0m\n");
        delete d_ptr->tTextDbProperty;
        d_ptr->tTextDbProperty = NULL;
    }
    d_ptr->source = src;

    if(src.count("/") == 1) /* class property */
    {
        l = src.split("/");
        sourceName = l.first();
        propName = l.last();
        setToolTip(QString("\"%1\" \"%2\" class property").arg(sourceName).arg(propName));
    }
    else if(src.count("/") == 3)
    {
        l = src.split("/");
        sourceName = l.at(0 ) + "/" + l.at(1) + "/" + l.at(2);
        propName = l.last();
        setToolTip(QString("\"%1\" \"%2\" device property").arg(sourceName).arg(propName));
    }
    else /* return with an error */
    {
        perr("PropertyLabel::setSource: source \"%s\" is not syntactically correct", qstoc(src));
        return;
    }

    d_ptr->tTextDbProperty = new TTextDbProperty(sourceName, propName,
                                          this, SLOT(setText(QString)),
                                          src.count("/") == 3);
    connect(d_ptr->tTextDbProperty->propertyReader(), SIGNAL(error(QString)),
            this, SLOT(setError(QString)));
}
