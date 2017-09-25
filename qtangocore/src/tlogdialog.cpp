#include <tutil.h>
#include "tlogdialog.h"
#include "tlog.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextDocument>
#include <QPrintDialog>
#include <QPrinter>

TLogDialog::TLogDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
//	setAttribute(Qt::WA_DeleteOnClose);
        setAttribute(Qt::WA_QuitOnClose, false);

	QStringList labels;
	labels << "Description" << "Origin" << "Reason" << "Severity" << "Time";
	ui.listLog->setHeaderLabels(labels);
	connect(ui.printButton, SIGNAL(clicked()), this, SLOT(print()));
	connect(ui.pushClear, SIGNAL(clicked()), this, SLOT(clearLogs()));
	connect(ui.pushMore, SIGNAL(toggled(bool)), this, SLOT(changeStack(bool)));
	changeStack(false);
}	

void TLogDialog::showLogs(bool all)
{
	ui.pushMore->setChecked(all);
	unsigned int i;
	QList<QPair<time_t, DevErrorList> > errs = TUtil::instance()->getLogs();
	QList<QPair<time_t, DevErrorList> >::const_iterator it;
	//show();
	QString sev;
	//listLog->setSorting(-1, false);
	ui.listLog->clear();
	int cnt = 0;
	for (it = errs.begin(); it != errs.end(); it++)
	{
		QTreeWidgetItem *base = new QTreeWidgetItem(ui.listLog);
		QTreeWidgetItem *temp;
		for (i = 0; i < it->second.length(); i++)
		{
			switch (it->second[i].severity)
			{
				case Tango::WARN:
					sev = "WARNING ";
					break;
				case Tango::ERR :
					sev = "ERROR ";
					break;
				case Tango::PANIC :
					sev = "PANIC ";
					break;
				default:
					sev =  "Unknown severity code ";
					break;
			}
			QTreeWidgetItem *der;
			if (i == 0)
				temp = base;
			else
			{
				der = new QTreeWidgetItem();
				//base->insertChild(base->childCount(), der);
				base->insertChild(0, der);
				temp = der;
			}						
			temp->setText(0, QString(it->second[i].desc));
			temp->setText(1, QString(it->second[i].origin));
			temp->setText(2, QString(it->second[i].reason));
			temp->setText(3, sev);
			if (i == 0)
			{
				temp->setText(4, QString(ctime(&(it->first))).remove("\n"));
			}
			//if (i == (it->second.length()-1))
			if (i == 0)
			{
				cnt++;
				if (cnt == errs.size())
				{
					ui.lineReason->setText(QString(it->second[i].reason));
					ui.lineSeverity->setText(sev);
					ui.lineOrigin->setText(QString(it->second[i].origin));
					ui.lineDescription->setText(QString(it->second[i].desc));
					QList<QLineEdit *> l;
					l << ui.lineReason << ui.lineSeverity << ui.lineOrigin << ui.lineDescription;
					foreach (QLineEdit *le, l)
						le->setMinimumWidth(10 + QFontMetrics(le->font()).width(le->text()));
				}
			}
		}
	}
	for (int i = 0; i < 5; i++)
		ui.listLog->resizeColumnToContents(i);
	this->show();
}

void TLogDialog::clearLogs()
{
	TUtil::instance()->clearLogs();
	showLogs();
}

void TLogDialog::print()
{
    QString testo(qApp->arguments().first());
	testo.append(" Logs\n\n");
	
	QList<QPair<time_t, DevErrorList> > errs = TUtil::instance()->getLogs();
	QList<QPair<time_t, DevErrorList> >::const_iterator it;
	for (it = errs.begin(); it != errs.end(); it++)
	{
	  TLog log;
      testo.append(log.formatMessage(*it));
	}
	QTextDocument *td = new QTextDocument(testo, this);
	QPrinter printer;

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Print Document"));
	if (dialog->exec() == QDialog::Accepted)
		td->print(&printer);

	close();
}

void TLogDialog::changeStack(bool toggled)
{
	ui.stackedWidget->setCurrentIndex(toggled == true);
	if (toggled)
	{
		ui.pushMore->setText("Show Last");
		ui.pushClear->setVisible(true);
		ui.printButton->setVisible(true);
		resize(600,400);
	}
	else
	{
		ui.pushMore->setText(QString("Show All %1 Error(s)").arg(TUtil::instance()->getLogs().size()));
		ui.pushClear->setVisible(false);
		ui.printButton->setVisible(false);
		resize(300,200);
	}
}

TLogDialog::~TLogDialog()
{
	ui.listLog->clear();
}
