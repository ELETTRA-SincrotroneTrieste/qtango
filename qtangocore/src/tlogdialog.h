#ifndef _TLOGDIALOG_H
#define _TLOGDIALOG_H

#include <qtangocore.h>

#if QT_VERSION < 0x050000
    #include "ui_logdialog.h"
#else
    #include <ui_logdialog-qt5.h>
#endif

#include <QPushButton>

/**
 * \brief a dialog to display logs
 *
 * TLogDialog is a simple QDialog derived object that allows
 * to simply display logs of an application
 *
 * \par Example
\verbatim
TLogDialog d;
d.exec();
\endverbatim
 */
class TLogDialog : public QDialog
{
Q_OBJECT
public:
    TLogDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~TLogDialog();

public slots:
	void clearLogs();
	void showLogs(bool = true);
	void print();
	void changeStack(bool);

private:
	Ui::LogDialog ui;
};

#endif
