#ifndef QTVALUEFILTER_H
#define QTVALUEFILTER_H

#include <tvariant.h>

/** \brief Filters a value extracted from a TVariant during the update method invoked
  *        when new data is available.
  *
  * Used in conjunction with QTWatcher or QTWriter, it allows to perform some last time
  * modifications to the value extracted from the TVariant.
  * You must subclass TValueFilter and provide your implementation of one of the needed
  * methods, according to the type of data you know is contained into the TVariant.
  *
  * \par Example
  * \code
  *
  * class MyFilter  : public TValueFilter
  {
    public:
        // dealing with integer values, we filter the intVal which has been extracted from v.
        //
        void filter(const TVariant& v, int& intVal, bool read, TValueFilter::State state);
  };

  // cpp file
  void MyFilter::filter(const TVariant& v, int& intVal, bool read, TValueFilter::State state)
  {
    if(state == TValueFilter::Update)
        intVal = intVal * 10;
  }

  // The value is multiplied by ten and, if for instance a label is updated with intVal, then
  // the displayed value will be visualized multiplied by ten.
  * \endcode
  *
  */
class TValueFilter
{
public:

    /** \brief the situation in which the data is extracted.
      *
      * <ul>
      * <li>Update: new data arrival</li>
      * <li>AutoConfiguration: extracted data is taken from the first value read during auto configuration</li>
      * </ul>
      */
    enum State { Update, AutoConfiguration };

    /** \brief The class constructor.
      *
      * Supposing you are using QTWatcher.
      *
      * \code
      * QLabel *label = new QLabel(this);
      * QTWatcher *watcher = new QTWatcher(this);
      * watcher->attach(label, SLOT(setText(const QString&)));
      * MyFilter *filter = new MyFilter();
      * watcher->installRefreshFilter(filter);
      * watcher->setSource("test/device/1/long_scalar");
      * \endcode
      *
      * The filter will multiply by ten the long_scalar value before
      * displaying it in the label, if MyFilter is implemented as in the code
      * snippet above.
      */
    TValueFilter();


    /** \brief Filter the integer value intValue
      *
      * This method does nothing: it must be reimplemented in a subclass to manipulate
      * intValue according to your needs.
      *
      * @param variant the TVariant from which you can fetch information about tango data stored
      * @param intValue the value, passed by reference, that you can modify according to your needs
      * @param read if true, we are extracting a read value, if false a set point has been extracted
      * @param updateState the update state. It tells us whether we have just read new data or
      *        if this is the value taken from the auto configuration process.
      *
      */
    virtual void filter(const TVariant& variant, int &intValue, bool read, State updateState);

    /** \brief The same as above, but for unsigned integer values.
      *
      * @see filter(const TVariant& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, unsigned int &uintValue, bool read, State updateState);

    /** \brief The same as above, but for double values.
      *
      * @see filter(const TVariant& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, double &doubleValue, bool read, State updateState);

    /** \brief The same as above, but for QString values.
      *
      * @see filter(const TVariant& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, QString &strValue, bool read, State updateState);

    /** \brief The same as above, but for Tango DevState values.
      *
      * @see filter(const TVariant& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, Tango::DevState &tgState, bool read, State updateState);

    /** \brief The same as above, but for boolean values.
      *
      * @see filter(const TVariant& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, bool &booleanValue, bool read, State updateState);

    /** \brief The same as above, but for boolean vectors.
      *
      * @see filter(const TVariant& variant, int &booleanValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, QVector<bool> &booleanVect, bool read, State updateState);

    /** \brief The same as above, but for double vectors.
      *
      * @see filter(const TVariant& variant, int &doubleValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, QVector<double> &doubleVect, bool read, State updateState);

    /** \brief The same as above, but for int vectors.
      *
      * @see filter(const TVariant& variant, int &intValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, QVector<int> &intVect, bool read, State updateState);

    /** \brief The same as above, but for string lists.
      *
      * @see filter(const TVariant& variant, int &strValue, bool read, State updateState)
      */
    virtual void filter(const TVariant& variant, QStringList &stringList, bool read, State updateState);
};

#endif // QTREFRESHFILTER_H
