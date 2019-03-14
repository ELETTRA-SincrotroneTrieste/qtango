#include "colortablemap.h"
#include <math.h>
#include <QColor>
#include <QtDebug>

ColorTableMap::ColorTableMap() : QMap<QString, QVector<QRgb> >()
{
    generateColorMap();
}

void ColorTableMap::generateColorMap()
{
    int i;
    QVector <QRgb> mBWPalette, mCPalette, mHotPalette, mJetPalette;
    // palette 1
    // float base  = 5.0/6;
    for (i=0; i<256; i++) {
        mBWPalette.append(QColor::fromHsv(0,0,i).rgb());
        // paletteColor1.append(QColor::fromHsvF((base-r),1,1).rgb()); // not used
    }

    // palette 2
    float deltaA = 255.0/31;
    float deltaB = 255.0/62;
    float deltaC = 255.0/37;
    //     int indexTmp = 0;
    for (i=0; i<32; i++) {
        mCPalette.append(QColor( round((float)i*deltaA), 0, round((float)i*deltaA)).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << round((float)i*deltaA) << "," << 0 << "," << round((float)i*deltaA);
        //         indexTmp++;
    }
    for (i=30; i>=0; i--) {
        mCPalette.append(QColor( round(i*deltaA), 0, 255).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << round((float)i*deltaA) << "," << 0 << "," << 255;
        //         indexTmp++;
    }
    for (i=1; i<32; i++) {
        mCPalette.append(QColor( 0, round((float)i*deltaA), 255).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << 0 << "," << round((float)i*deltaA) << "," << 255;
        //         indexTmp++;
    }
    for (i=30; i>=0; i--) {
        mCPalette.append(QColor(0, 255, round((float)i*deltaA)).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << 0 << "," << 255 << "," << round((float)i*deltaA);
        //         indexTmp++;
    }
    for (i=1; i<32; i++) {
        mCPalette.append(QColor( round((float)i*deltaA), 255, 0).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << round((float)i*deltaA) << "," << 255 << "," << 0;
        //         indexTmp++;
    }
    for (i=61; i>=0; i--) {
        mCPalette.append(QColor(255, round((float)i*deltaB), 0).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << 255 << "," << round((float)i*deltaB)<< "," << 0 ;
        //         indexTmp++;
    }
    for (i=1; i<38; i++) {
        mCPalette.append(QColor(255, round((float)i*deltaC), round((float)i*deltaC)).rgb());  // (r,g,b)
        //         // qDebug() << indexTmp << "," << i << "," << 255 << "," << round((float)i*deltaC) << "," << round((float)i*deltaC);
        //         indexTmp++;
    }
    mCPalette.append(QColor(255, 255, 255).rgb());  // (r,g,b)
    //     // qDebug() << indexTmp << "," << "x" << "," << 255 << "," << 255 << "," << 255;



    // palette HOT
    deltaA = 1.0/96;
    deltaB = 1.0/64;
    for (i=1; i<96; i++) {
        mHotPalette.append(QColor::fromRgbF( (float)i*deltaA, 0.0, 0.0 ).rgb());
    }
    mHotPalette.append(QColor::fromRgbF( 1.0, 0.0, 0.0 ).rgb());
    for (i=1; i<96; i++) {
        mHotPalette.append(QColor::fromRgbF( 1.0, (float)i*deltaA, 0.0 ).rgb());
    }
    mHotPalette.append(QColor::fromRgbF( 1.0, 1.0, 0.0 ).rgb());
    for (i=1; i<65; i++) {
        mHotPalette.append(QColor::fromRgbF( 1.0, 1.0, (float)i*deltaB ).rgb());
    }

    // palette JET
    for (i=33; i<65; i++) {
        mJetPalette.append(QColor::fromRgbF( 0.0, 0.0, (float)i*deltaB ).rgb());
    }
    for (i=1; i<65; i++) {
        mJetPalette.append(QColor::fromRgbF( 0.0, (float)i*deltaB, 1.0 ).rgb());
    }
    for (i=1; i<65; i++) {
        mJetPalette.append(QColor::fromRgbF( (float)i*deltaB, 1.0,  1.0-(float)i*deltaB ).rgb());
    }
    for (i=1; i<65; i++) {
        mJetPalette.append(QColor::fromRgbF( 1.0, 1.0-(float)i*deltaB, 0.0 ).rgb());
    }
    for (i=1; i<33; i++) {
        mJetPalette.append(QColor::fromRgbF( 1.0-(float)i*deltaB, 0.0, 0.0 ).rgb());
    }

    insert("bw", mBWPalette);
    insert("c", mCPalette);
    insert("hot", mHotPalette);
    insert("jet", mJetPalette);
}
