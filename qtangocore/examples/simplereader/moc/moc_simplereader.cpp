/****************************************************************************
** Meta object code from reading C++ file 'simplereader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../simplereader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simplereader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SimpleReader_t {
    QByteArrayData data[7];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SimpleReader_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SimpleReader_t qt_meta_stringdata_SimpleReader = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SimpleReader"
QT_MOC_LITERAL(1, 13, 7), // "refresh"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 8), // "TVariant"
QT_MOC_LITERAL(4, 31, 1), // "v"
QT_MOC_LITERAL(5, 33, 4), // "init"
QT_MOC_LITERAL(6, 38, 35) // "const TangoConfigurationParam..."

    },
    "SimpleReader\0refresh\0\0TVariant\0v\0init\0"
    "const TangoConfigurationParameters*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SimpleReader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x09 /* Protected */,
       5,    1,   27,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    2,

       0        // eod
};

void SimpleReader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SimpleReader *_t = static_cast<SimpleReader *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->refresh((*reinterpret_cast< const TVariant(*)>(_a[1]))); break;
        case 1: _t->init((*reinterpret_cast< const TangoConfigurationParameters*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TVariant >(); break;
            }
            break;
        }
    }
}

const QMetaObject SimpleReader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SimpleReader.data,
      qt_meta_data_SimpleReader,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SimpleReader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimpleReader::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SimpleReader.stringdata0))
        return static_cast<void*>(const_cast< SimpleReader*>(this));
    if (!strcmp(_clname, "QTangoComProxyReader"))
        return static_cast< QTangoComProxyReader*>(const_cast< SimpleReader*>(this));
    return QObject::qt_metacast(_clname);
}

int SimpleReader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
