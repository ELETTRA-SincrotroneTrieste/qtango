#ifndef TIMAGEDATA_H
#define TIMAGEDATA_H

#include <tango.h>
#include <QVector>
#include <QVector>
#include <macros.h>

/*
 * This file contains the definitions of TImageData. Users must include also timagedata_impl.h, where implementation
 * is. Since it's a template class, the users must include the headers to be able to work with it.
 * The .h files have been split into two to save qtango build time when the implementation is modified
 */

template <class T>
class TImageDataPrivate
{
public:
    ~TImageDataPrivate();

    int dimx, dimy;
    Tango::AttrQuality quality;
    Tango::CmdArgType type;
    QVector< QVector<T> > data;
};

/** \brief This class provides a representation of the Tango Image data type, providing matrix-like
 *         access.
 *
 * \par Includes
 * Both the following directives are necessary:
 * \include <timagedata.h>
 * \include <timagedata_impl.h>
 *
 */
template <class T>
class TImageData
{
public:

    TImageData(Tango::DeviceAttribute &da);

    TImageData(const QVector<T> &data, int dimx, int dimy, Tango::AttrQuality q, Tango::CmdArgType t);

    TImageData(const TImageData<T> &other);

    TImageData();

    virtual ~TImageData();

    int dimX() const;

    int dimY() const;

    int rowCount() const;

    int columnCount() const;

    int quality() const;

    Tango::CmdArgType dataType() const;

    TImageData<T> & operator=(const TImageData<T> & other); // copy assignment

    QVector<T>& operator[](size_t idx);
    const QVector<T>& operator[](size_t idx) const;

    void print() const;

    QString typeRepresentation(CmdArgType t) const;
private:
    TImageDataPrivate<T> *d;
};

#endif // TIMAGEDATA_H
