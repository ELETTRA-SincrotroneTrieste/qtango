#ifndef COLORTABLESET_H
#define COLORTABLESET_H

#include <QMap>
#include <QRgb>
#include <QVector>


class ColorTableMap : public QMap<QString, QVector<QRgb> >
{
public:
    ColorTableMap();

    QVector<QRgb> mCPalette, mJetPalette, mBWPalette, mHotPalette;
    void generateColorMap();
};

#endif // COLORTABLESET_H
