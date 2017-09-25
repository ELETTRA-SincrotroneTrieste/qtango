#ifndef SECONDINSTANCEFILTER_H
#define SECONDINSTANCEFILTER_H

class QObject;

class SecondInstanceHandler
{
public:
    SecondInstanceHandler(QObject *obj);

    virtual ~SecondInstanceHandler();

    /** \brief This method is invoked when a second instance of a unique application has been detected.
      *
      * The default implementation is empty. You can customize the behaviour in your custom class.
      * Then install this filter by calling TApplication::installSecondInstanceFilter.
      *
      * @see TApplication
      */
    virtual void filter();

protected:
    QObject* d_object;

};

#endif // SECONDINSTANCEFILTER_H
