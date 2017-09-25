#ifndef TSPECTRUMBOOLEDITOR_H
#define TSPECTRUMBOOLEDITOR_H

#include "ui_ttablebooleditor.h"

class TTableBoolEditor: public QDialog
{
Q_OBJECT
	
public:
    TTableBoolEditor(int, int, QString, QString, QString, QString, QString, QWidget * = NULL);
    virtual ~TTableBoolEditor(){};

    Ui::TTableBoolEditor ui;
	
	
  signals:
	void propertyChanged(const QString& propName, const QVariant &val);
	
  protected slots:
	void accept();

private slots:
    void updateColorTrue();
    void updateColorFalse();
    void updateFlags();
    void updateLabels();
};

#endif
