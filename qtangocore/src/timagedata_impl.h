#ifndef TIMAGEDATA_IMPL_H
#define TIMAGEDATA_IMPL_H

#include "timagedata.h"

template <class T>
TImageDataPrivate<T>::~TImageDataPrivate()
{
    printf("\e[0;31mdeleting TImageDataPrivate %p\e[0m\n", this);
}

template <class T>
TImageData<T>::TImageData(Tango::DeviceAttribute &da)
{
    d = new TImageDataPrivate<T>();
    printf("\e[1;36mnew TImageData(DeviceAttribute& d) %p d: %p\e[0m\n", this, d);
    d->dimx = da.get_dim_x();
    d->dimy = da.get_dim_y();
    d->type = static_cast<Tango::CmdArgType>(da.get_type());
    d->quality = da.get_quality();

    std::vector<T> data;

    da >> data;
    for(int i = 0; i < d->dimx; i++)
    {
        d->data.push_back(QVector<T>());
        for(int j = 0; j < d->dimy; j++)
            d->data[i].push_back(data[i * d->dimy + j]);
    }
}

template <class T>
TImageData<T>::TImageData(const QVector<T> &data,
                       int dimx,
                       int dimy,
                       Tango::AttrQuality q,
                       Tango::CmdArgType t)
{
    d = new TImageDataPrivate<T>();
    printf("\e[1;36mnew TImageData(const QVector<T> &data, int dimx, ...) %p d: %p\e[0m\n", this, d);
    d->dimx = dimx;
    d->dimy = dimy;
    d->quality = q;
    d->type = t;
    for(int i = 0; i < d->dimx; i++)
    {
        d->data.push_back(QVector<T>());
        for(int j = 0; j < d->dimy; j++)
            d->data[i].push_back(data[i * d->dimy + j]);
    }
}

template <class T>
TImageData<T>::TImageData()
{
    d = new TImageDataPrivate<T>();
    printf("\e[1;36mnew TImageData() %p d: %p\e[0m\n", this, d);
    d->dimx = d->dimy = 0;
    d->type = Tango::DEV_VOID;
    d->quality = Tango::ATTR_INVALID;
}

template <class T>
TImageData<T>::TImageData(const TImageData<T> &other)
{
    printf("\e[0;33m COPY CONSTRUCTOR ENTER: this is %p FROM %p\e[0m\n", this, &other);
    d = new TImageDataPrivate<T>();
    d->dimx = other.dimX();
    d->dimy = other.dimY();
    d->type = other.d->type;
    d->quality = other.d->quality;
    d->data = other.d->data;
}

template <class T>
TImageData<T>::~TImageData()
{
    printf("\e[1;31mX deleting %p\e[0m\n", this);
    delete d;
}

template <class T>
int TImageData<T>::dimX() const
{
    return d->dimx;
}

template <class T>
int TImageData<T>::dimY() const
{
    return d->dimy;
}

template <class T>
int TImageData<T>::rowCount() const
{
    return d->dimx;
}

template <class T>
int TImageData<T>::columnCount() const
{
    return d->dimy;
}

template <class T>
int TImageData<T>::quality() const
{
    return d->quality;
}

template <class T>
Tango::CmdArgType TImageData<T>::dataType() const
{
    return d->type;
}

template <class T>
TImageData<T> &TImageData<T>::operator=(const TImageData<T> &o)
{
    printf("\e[1;33m OPERATOR = ENTER\e[0m\n");
    if(d)
        delete d;
    printf("\e[1;32moperator= other %p this %p private d %p \n", &o, this, d);
    d = new TImageDataPrivate<T>();
    printf("\e[1;32moperator= new d: %p\e[0m\n", d);
    d->dimx = o.dimX();
    d->dimy = o.dimY();
    d->type = o.d->type;
    d->data = o.d->data;
    d->quality = o.d->quality;

    return *this;
}

template <class T>
QVector<T> &TImageData<T>::operator[](size_t idx)
{
    return d->data[idx];
}

template <class T>
const QVector<T> &TImageData<T>::operator[](size_t idx) const
{
    return d->data[idx];
}

template <class T>
QString TImageData<T>::typeRepresentation(CmdArgType t) const
{
    char *argTypes [] =
    {
        "DEV_VOID",
                "DEV_BOOLEAN",
                "DEV_SHORT",
                "DEV_LONG",
                "DEV_FLOAT",
                "DEV_DOUBLE",
                "DEV_USHORT",
                "DEV_ULONG",
                "DEV_STRING",
                "DEVVAR_CHARARRAY",
                "DEVVAR_SHORTARRAY",
                "DEVVAR_LONGARRAY",
                "DEVVAR_FLOATARRAY",
                "DEVVAR_DOUBLEARRAY",
                "DEVVAR_USHORTARRAY",
                "DEVVAR_ULONGARRAY",
                "DEVVAR_STRINGARRAY",
                "DEVVAR_LONGSTRINGARRAY",
                "DEVVAR_DOUBLESTRINGARRAY",
                "DEV_STATE",
                "CONST_DEV_STRING",
                "DEVVAR_BOOLEANARRAY",
                "DEV_UCHAR",
                "DEV_LONG64",
                "DEV_ULONG64",
                "DEVVAR_LONG64ARRAY",
                "DEVVAR_ULONG64ARRAY",
                "DEV_INT",
                "DEV_ENCODED",
                "DEV_ENUM",
                "DEV_PIPE_BLOB",
                "DEVVAR_STATEARRAY",
                "DATA_TYPE_UNKNOWN"

    };
    if(t < DEVVAR_ULONG64ARRAY + 6)
        return QString(argTypes[t]);

    return QString("Unknown type for %1. Check tango sources at lib/cpp/server/tango_const.h").arg(t);
}

template <class T>
void TImageData<T>::print() const
{
    printf("TImageData: matrix \e[1;32m%dx%d\e[0m\tdata type: \e[0;36m%s\e[0m\n",
           d->dimx, d->dimy, qstoc(typeRepresentation(d->type)));
    printf("\n\e[1;34m---\e[0m\n");
    printf("--- row nu %d\n", d->data.size());
    for(int r = 0; r < d->dimx; r++)
    {
        printf("\t");
        for(int c = 0; c < d->dimy; c++)
            printf("%s, ", qstoc(QString::number(this->operator [](r)[c])));
        printf("\n");
    }
    printf("\n\e[1;34m---\e[0m\n");
}


#endif // TIMAGEDATA_IMPL_H
