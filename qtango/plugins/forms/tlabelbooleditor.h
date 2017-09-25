#ifndef TSCALARBOOLEDITOR_H
#define TSCALARBOOLEDITOR_H

#include "ui_tlabelbooleditor.h"

class TLabelBoolEditor: public QDialog
{
Q_OBJECT
	
public:
    TLabelBoolEditor(QString, QString, QColor, QColor, QWidget * = NULL);
    virtual ~TLabelBoolEditor(){};

    Ui::TLabelBoolEditor ui;
	
 signals:
	void propertyChanged(const QString& propName, const QVariant &val);
	
  protected slots:
	void accept();

private slots:
    void updateColorTrue();
    void updateColorFalse();
    void updateEditor();
    void updateEditor(QString){ qDebug("update"); updateEditor(); };
};

#endif
