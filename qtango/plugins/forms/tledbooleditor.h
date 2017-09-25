#ifndef TLEDBOOLEDITOR_H
#define TLEDBOOLEDITOR_H

#include "ui_tledbooleditor.h"

class TLedBoolEditor: public QDialog
{
Q_OBJECT
	
public:
    TLedBoolEditor(QColor, QColor, QWidget * = NULL);
    virtual ~TLedBoolEditor(){};

    Ui::TLedBoolEditor ui;
  
  signals:
	void propertyChanged(const QString& propName, const QVariant &val);
	
  protected slots:
	void accept();
	
private slots:
    void updateColorTrue();
    void updateColorFalse();
};

#endif
