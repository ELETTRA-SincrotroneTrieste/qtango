#include <tutil.h>
#include <tgauge.h>
#include <QEvent>
#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <TPlot>
#include <EApplicationLauncher>

//#include <QtDebug>

TLinearGauge::TLinearGauge(QWidget *parent, Qt::Orientation o, bool mode) : ELinearGauge(parent, o), menu(NULL), plot(NULL), m_enableViewTrend(true), m_enableHelperApplication(true), app(NULL)
{
	setAutoConfiguration(mode);
	setHideEventEnabled(true); /* generally enabled for readers */
	setDisabled(true);
	helperApplication = QString();
	setAttribute(Qt::WA_AlwaysShowToolTips);
	connect(readingThread, SIGNAL(configured()), this, SLOT(init()), Qt::QueuedConnection);
	connect(readingThread, SIGNAL(newDataAvailable()), this, SLOT(refresh()), Qt::QueuedConnection);
}

TLinearGauge::~TLinearGauge()
{
        //disconnect(this, SLOT(connectToReadingThread()));
	disconnect(this, SLOT(refresh()));
	unsetSource();
}

void TLinearGauge::init()
{
  if(!autoConfiguration())
	return;
	if (getAutoConfiguration())
	{
		AttributeInfoEx info = getAttributeInfo();
		double MV, ME, MW, mW, mE, mV;
		bool temp, ok=true;
	
		MV = QString::fromStdString(info.max_value).toDouble(&temp);
		ok = ok && temp;
		mV = QString::fromStdString(info.min_value).toDouble(&temp);
		ok = ok && temp;
		if ((ok) && (mV < MV))
		{
			setMaxValue(MV);
			setMinValue(mV);
		}
		else 
			return;

		ok = true;
		ME = QString::fromStdString(info.alarms.max_alarm).toDouble(&temp);
		ok = ok && temp;
		mE = QString::fromStdString(info.alarms.min_alarm).toDouble(&temp);
		ok = ok && temp;
		if ((ok) && (mE < ME) && (mE > mV) && (ME < MV))
		{
			setHighError(ME);
			setLowError(mE);
		}
		else
		{
			ME = MV;
			mE = mV;
			setHighError(MV);
			setLowError(mV);
		}

		ok = true;
		MW = QString::fromStdString(info.alarms.max_warning).toDouble(&temp);
		ok = ok && temp;
		mW = QString::fromStdString(info.alarms.min_warning).toDouble(&temp);
		ok = ok && temp;
		if ((ok) && (mW < MW) && (mW > mE) && (MW < ME))
		{
			setHighWarning(MW);
			setLowWarning(mW);
		}
		else
		{
			setHighWarning(ME);
			setLowWarning(mE);
		}
	}
}

void TLinearGauge::refresh()
{
	setToolTip(message);
        if (quality == ATTR_INVALID)
		setEnabled(false);
	else
	{
		if (canConvertToDouble())
		{
			setEnabled(true);
			ELinearGauge::setValue(getDataAsDouble());
			if (isReferenceEnabled() && (getAttributeInfo().writable == READ_WRITE))
				setReference(getSetDataAsDouble());
		}
	        else
		{
	                setToolTip("wrong data type");
			setDisabled(true);
		}
	}
	update();
}

void TLinearGauge::hideEvent(QHideEvent *)
{
	stopRefresh();
}

void TLinearGauge::showEvent(QShowEvent *)
{
	startRefresh();
}

void TLinearGauge::mousePressEvent(QMouseEvent *ev)
{
	if (!menu)
		delete menu;

	menu = new QMenu(this);
	if (isViewTrendEnabled())
	{
		switch (arg_type)
		{
			case DEV_STATE:
			case DEV_BOOLEAN:
			case DEV_STRING:
			case CONST_DEV_STRING:
			case DEV_UCHAR:
				break;
			default:
				menu->addAction("View Trend...", this, SLOT(viewTrend()));
				break;
		}
	}
        
	if (isHelperApplicationEnabled())
	{
		if (helperApplication.isNull())
		{
			string app = TUtil::instance()->findHelperApplication(getDeviceName());
			if (app != "")
				helperApplication = QString::fromStdString(app + " " + getDeviceName());
		}
		if (!helperApplication.isEmpty())
			menu->addAction("Helper Application...", this, SLOT(launchHelperApplication()));
	}

	if (ev->button() == Qt::RightButton)
		menu->popup(ev->globalPos());		       
}

void TLinearGauge::viewTrend()
{
	if (!plot)
	{
		plot = new TPlot(this);
		
		plot->fillFromHistory(getSource());
		int period = getPeriod();
                if (period == 0)
                        period = 1000;
                plot->setSource(getSource(), getMode(), period);
		plot->setWindowFlags(Qt::Window);
		plot->setAttribute(Qt::WA_DeleteOnClose);
		plot->setAttribute(Qt::WA_QuitOnClose, false);
		plot->resize(400,300);
		plot->show();
		plot->setBufSize(600);
		plot->setMargin(6);
	}
	else
	{
		if (plot->isMinimized())
			plot->showNormal();
		if (plot->isHidden())
			plot->show();
		plot->raise();
	}
}

void TLinearGauge::launchHelperApplication()
{
	if (!app)
		app = new EApplicationLauncher(helperApplication);
	app->Rise();
}

TCircularGauge::TCircularGauge(QWidget *parent, bool mode) : ECircularGauge(parent), menu(NULL), plot(NULL), m_enableViewTrend(true), m_enableHelperApplication(true), app(NULL)
{
	setAutoConfiguration(mode);
	setDisabled(true);
	helperApplication = QString();
	connect(readingThread, SIGNAL(configured()), this, SLOT(init()), Qt::QueuedConnection);
        connect(readingThread, SIGNAL(newDataAvailable()), this, SLOT(refresh()), Qt::QueuedConnection);
}

TCircularGauge::~TCircularGauge()
{
        disconnect(this, SLOT(refresh()));
	unsetSource();
}

void TCircularGauge::refresh()
{
	setToolTip(message);
        if (quality == ATTR_INVALID)
		setEnabled(false);
	else
	{
		if (canConvertToDouble())
		{
			setEnabled(true);
			ECircularGauge::setValue(getDataAsDouble());
			if (isReferenceEnabled() && (getAttributeInfo().writable == READ_WRITE))
				setReference(getSetDataAsDouble());
		}
		else
		{
	                setToolTip("wrong data type");
			setDisabled(true);
		}
	}
	update();
}		

void TCircularGauge::hideEvent(QHideEvent *)
{
	stopRefresh();
}

void TCircularGauge::showEvent(QShowEvent *)
{
	startRefresh();
}

void TCircularGauge::init()
{
  if(!autoConfiguration())
	return;
	if (getAutoConfiguration())
	{
		AttributeInfoEx info = getAttributeInfo();
		double MV, ME, MW, mW, mE, mV;
		bool temp, ok=true;
		
		MV = QString::fromStdString(info.max_value).toDouble(&temp);
		ok = ok && temp;
		mV = QString::fromStdString(info.min_value).toDouble(&temp);
		ok = ok && temp;
		if ((ok) && (mV < MV))
		{
			setMaxValue(MV);
			setMinValue(mV);
		}
		else 
			return;

		ok = true;
		ME = QString::fromStdString(info.alarms.max_alarm).toDouble(&temp);
		ok = ok && temp;
		mE = QString::fromStdString(info.alarms.min_alarm).toDouble(&temp);
		ok = ok && temp;
		if ((ok) && (mE < ME) && (mE >= mV) && (ME <= MV))
		{
			setHighError(ME);
			setLowError(mE);
		}
		else
		{
			ME = MV;
			mE = mV;
			setHighError(MV);
			setLowError(mV);
		}

		ok = true;
		MW = QString::fromStdString(info.alarms.max_warning).toDouble(&temp);
		ok = ok && temp;
		mW = QString::fromStdString(info.alarms.min_warning).toDouble(&temp);
		ok = ok && temp;
		if ((ok) && (mW < MW) && (mW >= mE) && (MW <= ME))
		{
			setHighWarning(MW);
			setLowWarning(mW);
		}
		else
		{
			setHighWarning(ME);
			setLowWarning(mE);
		}

		if ((info.unit != "No unit") && (info.unit != "") && (info.unit != "None"))
			setLabel(QString::fromStdString(info.unit));
		setValueFormat(QString::fromStdString(info.format));
	}
}

void TCircularGauge::mousePressEvent(QMouseEvent *ev)
{
	if (!menu)
		delete menu;

	menu = new QMenu(this);
	if (isViewTrendEnabled())
	{
		switch (arg_type)
		{
			case DEV_STATE:
			case DEV_BOOLEAN:
			case DEV_STRING:
			case CONST_DEV_STRING:
			case DEV_UCHAR:
				break;
			default:
				menu->addAction("View Trend...", this, SLOT(viewTrend()));
				break;
		}
	}

	if (isHelperApplicationEnabled())
	{
		if (helperApplication.isNull())
		{
			string app = TUtil::instance()->findHelperApplication(getDeviceName());
			if (app != "")
				helperApplication = QString::fromStdString(app + " " + getDeviceName());
		}
		if (!helperApplication.isNull())
			menu->addAction("Helper Application...", this, SLOT(launchHelperApplication()));
	}

	if (ev->button() == Qt::RightButton)
		menu->popup(ev->globalPos());		       
}

void TCircularGauge::viewTrend()
{
	if (!plot)
	{
		plot = new TPlot(this);
		
		plot->fillFromHistory(getSource());
		int period = getPeriod();
                if (period == 0)
                        period = 1000;
                plot->setSource(getSource(), getMode(), period);
		plot->setWindowFlags(Qt::Window);
		plot->setAttribute(Qt::WA_DeleteOnClose);
		plot->setAttribute(Qt::WA_QuitOnClose, false);
		plot->resize(400,300);
		plot->show();
		plot->setBufSize(600);
		plot->setMargin(6);
	}
	else
	{
		if (plot->isMinimized())
			plot->showNormal();
		if (plot->isHidden())
			plot->show();
		plot->raise();
	}
}

void TCircularGauge::launchHelperApplication()
{
	if (!app)
		app = new EApplicationLauncher(helperApplication);
	app->Rise();
}

