#include "timingwidget.h"
#include <TLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>

TimingWidget::TimingWidget(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *vlo = new QHBoxLayout(this);
	TLabel * label = new TLabel(this);
	label->setObjectName("tlabel");
	QSpinBox *sb = new QSpinBox(this);
	sb->setToolTip("Change the tango polling period of the source");
	sb->setObjectName("spinBox");
	sb->setMinimum(10);
	sb->setMaximum(10000);
	QLabel *sbLabel = new QLabel(this);
	sbLabel->setText("Period");
	sbLabel->setToolTip("The tango source is in polling mode, for this example");
	QLabel *srcLabel = new QLabel(this);
	srcLabel->setObjectName("srcLabel");
	QCheckBox *cb = new QCheckBox(this);
	cb->setText("Wait refresh");
	cb->setToolTip("Check to enable global refresh trigger.\nUncheck to let the widget refresh asynchronously");
	cb->setObjectName("waitRefreshCheckBox");
	QPushButton *pb = new QPushButton(this);
	pb->setText("Apply");
	pb->setToolTip("Click  to apply the polling period and/or the refresh mode");
	/* combo box for polling mode */
	QComboBox *cbRefMode = new QComboBox(this);
	cbRefMode->setObjectName("comboRefMode");
	cbRefMode->addItems(QStringList() << "AUTO REFRESH" << "USER EVENT REFRESH" <<	"CHANGE EVENT REFRESH" <<
		"PERIODIC_EVENT_REFRESH" << 	"ARCHIVE_EVENT_REFRESH" << 	"POLLED_REFRESH");
	
	/* add elements to layout */
	QList<QWidget *>widgets;
	widgets << srcLabel << label << sbLabel << sb << cbRefMode << pb << cb;
	foreach(QWidget *w, widgets)
		vlo->addWidget(w);
	
	/* signals/slots */
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(enableWaitRefresh(bool)));
	connect(pb, SIGNAL(clicked()), this, SLOT(changeRefresh()));
}

void TimingWidget::changeRefresh()
{
	QSpinBox *sb = findChild<QSpinBox *>("spinBox");
	TLabel *label = findChild<TLabel *>("tlabel");
	QComboBox *cb = findChild<QComboBox *>("comboRefMode");
	if (sb && label && cb)
	{
		label->setPeriod(sb->value());	
		/* exact map between currentIndex and RefreshMode enum in qtangocore_common.h */
		label->setRefreshMode((RefreshMode) cb->currentIndex());
	}
}

void TimingWidget::enableWaitRefresh(bool en)
{
	TLabel *label = findChild<TLabel *>("tlabel");
	label->setWaitRefresh(en);
}

void TimingWidget::setSource(QString s)
{
	TLabel *label = findChild<TLabel *>("tlabel");
	QLabel *srcLabel = findChild<QLabel *>("srcLabel");
	if(label && srcLabel)
	{
		label->setRefreshMode(POLLED_REFRESH);
		label->setSource(s);
		srcLabel->setText(label->realSource());
		srcLabel->setToolTip("The name of the tango source point ");
	}
	QSpinBox *sb  = findChild<QSpinBox *>("spinBox");
	if(sb)
		sb->setValue(label->period());
	QCheckBox *cb = findChild<QCheckBox *>("waitRefreshCheckBox");
	cb->setChecked(label->waitRefresh());
}

