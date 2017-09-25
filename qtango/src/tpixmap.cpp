#include "tpixmap.h"
#include "elettracolors.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>

TPixmap::TPixmap(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f), QTangoComProxyReader(this), QTangoWidgetCommon(this)
{
	nullpix = false;
	setMinimumSize(QSize(22, 22) );
	setAlignment(Qt::AlignCenter);
	setAttribute(Qt::WA_AlwaysShowToolTips);
	connect(QTangoComProxyReader::qtangoComHandle(), 
		SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
	setPixmapByName(default_pixmap);
	setDisabled(true);
	
	setAutoConfiguration(false);
	setHideEventEnabled(true); /* generally enabled for readers */
	setViewTrendEnabled(false);
	
}

TPixmap::~TPixmap()
{
        
}

/* Associates a state to a pixmap. Calls qt `load()' function to set the pixmap file */
void TPixmap::setStatePixmap(int state, const QString & filename)
{
	/* Add the couple state/filename to the list of the states and the pixmaps */
	QPair<int, QString> pair(state, filename);
	/* if the state was already defined, replace it, otherwise push_back a new pair */
	int i;
	for(i = 0; i < assoc.size(); i++)
	{
		if(assoc[i].first == state) /* already present, replace it */
		{
			assoc[i].second = filename; /* the new filename */
			break; /* stop searching */
		}
	}
	assoc.push_back(pair);
}

void TPixmap::setStatePixmap(Tango::DevState state, const QString &filename)
{
  int st = (int) state;
  setStatePixmap(st, filename);
}

void TPixmap::setDefaultPixmap(const QString &filename)
{
  default_pixmap = filename;
  if(!configured())
    setPixmapByName(filename);
}

/* Returns the state associated to the pixmap `filename',
 * -1 if no state is associated with the pixmap.
 */
int TPixmap::stateForPixmap(QString filename)
{
	int i;
	for(i = 0; i < assoc.size(); i++)
	{
		if(assoc[i].second == filename)
			return assoc[i].first;
	}
	return -1;
}

void TPixmap::paintEvent(QPaintEvent *e)
{
  QLabel::paintEvent(e);
  Tango::AttrQuality quality = currentValue().quality();
  if(!qtangoComHandle()->configured() || quality == ATTR_VALID)
        return;
  Config *cfg = Config::instance();
  QColor color;
  color = cfg->qualityColor(quality);
  QPainter painter(this);
  QPen pen(color);
  color.setAlpha(40);
  painter.fillRect(e->rect(), QBrush(color));
}

/* Returns the file name of the pixmap associated to the state, an empty string ("") if 
 * no pixmap is associated to that state.
 */
QString TPixmap::pixmapForState(int state)
{
	int i;
	for(i = 0; i < assoc.size(); i++)
	{
		if(assoc[i].first == state)
			return assoc[i].second;
	}
	return QString("");
}

/* Given the state, this one sets the pixmap associated */
void TPixmap::apply_state(unsigned int s)
{
	QString pixfile = pixmapForState(s);
	if(pixfile == "")
	{
		pinfo("No pixmap defined for the state %d", s);
		if(!default_pixmap.isEmpty())
		  setPixmapByName(default_pixmap);
		else
		  pinfo("you might want to define a default pixmap to show when there is not an associated state.\n"
			  "  You may use setDefaultPixmap() with a valid file name or Qt resource name");
		return;
	}
	
	setPixmapByName(pixfile);
	/* Store the current pixmap */
	SavePixmap(pixfile);
	state = s;
}

void TPixmap::refresh(const TVariant &v)
{
        setReaderToolTip(v.message());

        if (v.quality() == ATTR_INVALID)
        {
        	setReaderToolTip("The attribute is not valid!");
		setDisabled(true);
		setPixmapByName(default_pixmap);
                return;
        }

	if(!isEnabled())
	 setEnabled(true);
        
	if (v.canConvertToBool() )
	{
	  if(v.toBool() )
			apply_state(1);
		else
			apply_state(0);
	}
	else if (v.canConvertToState())
        {
		DevState stato;
                stato = v.toState();
		/* Change the label according to the state */
		apply_state((unsigned int) stato);
        }
	else if(v.canConvertToInt() )
	{
	   int sta = v.toInt();
	   apply_state((unsigned int ) sta);
	}
	else if(v.canConvertToUInt() )
	{
		unsigned s = v.toUInt();
		apply_state(s);
	}
    else
	{
			setReaderToolTip("wrong data type");
			setDisabled(true);
	setPixmapByName(default_pixmap);
	}
}

void TPixmap::updateConfiguration()
{
  bool ok;
  if(d_stateList.size() == d_pixmapList.size())
  {
	int j = 0;
	QStringList tangoStates;
	
	while(j < Tango::UNKNOWN)
	{
	  qDebug() << j << tangoStates;
	  tangoStates << DevStateName[j];
	  /* a security check in case Tango::DevState is not aligned with DevStateNames[] */
	  if(tangoStates.last() == "UNKNOWN")
		break;
	  j++;
	}
	for(int i = 0; i < d_stateList.size(); i++)
	{
	  int state = d_stateList.at(i).toInt(&ok);
	  if(ok) /* it was a number */
	  {
		pinfo("TPixmap: associating integer %d to pixmap \"%s\"", state, qstoc(d_pixmapList.at(i)));
		setStatePixmap(state, d_pixmapList.at(i));
	  }
	  else
	  {
		QString tangoState = d_stateList.at(i);
		tangoState.remove("Tango::");
		int index = tangoStates.indexOf(tangoState);
		if(index >= 0)
		{
		  pinfo("TPixmap: associating tango state \"%s\" (%d) to pixmap \"%s\"", qstoc(tangoStates.at(index)), 
				index, qstoc(d_pixmapList.at(i)));
		  setStatePixmap(index, d_pixmapList.at(i));
		}
		else
		{
		  perr("\"%s\" is not a valid tango state: skipping!", qstoc(d_stateList.at(i)));
		}
	  }
	}
  }
}

void TPixmap::setStateList(const QStringList &sl)
{
  d_stateList = sl;
  updateConfiguration();
}

void TPixmap::setPixmapList(const QStringList &names)
{
  d_pixmapList = names;
  updateConfiguration();
}

void TPixmap::setPixmapByName(const QString &name)
{
  QPixmap pixmap;
  if(name.contains(":") || name.contains('/'))
	pixmap = QPixmap(name);
  else if(!name.contains('/'))
	pixmap = QPixmap(TUtil::instance()->iconPath() + "/" + name);
  
  if(!pixmap.isNull())
  {
	QLabel::setPixmap(pixmap);
  }
  else
  {
	setText("Error setting pixmap");
	pinfo("TPixmap: error building the pixmap");
	pinfo("The file \"%s\", if not a qt resource system file, is searched into the directory", qstoc(name));
	pinfo("\"%s\"", qstoc(TUtil::instance()->iconPath()));
	pinfo("which is a user wide directory stored into a QSetting object.");
	pinfo("Check that the icon is there or change the system wide setting by calling TUtil::setIconPath()");
	pinfo("with the correct path where icons and pixmap are installed.");
  }
}





