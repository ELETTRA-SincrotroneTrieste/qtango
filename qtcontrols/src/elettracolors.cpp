#include <QtDebug>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QToolTip>
#include <QApplication>
#include "elettracolors.h"
#include "macros.h"

using namespace Elettra;

const char *colorStrings[] = { "white",   "darkGray",  "gray",  "lightGray",  "red",  "darkRed",  "darkMaroon",  "maroon",  "camel",
		 "orange",   "yellow",   "dirtyYellow",  "darkYellow",  "green",   "greenWater",  "darkGreen",  
		"cyan",  "darkCyan",  "darkWater",  "blue",  "darkBlue",  "veryDarkBlue", 
		"blueGray",  "violet",  "darkViolet", 
  		"darkPink",  "pink",  "pinkPig",   "fuchsia",  "almostBlack",  "black",  "boosterColor",  "elettraColor",  "fermiColor",  "elettraHighlight",  
 		"elettraAlternateBase",  "boosterHighlight",  "boosterAlternateBase",  "fermiHighlight", 
		/* let this color fermiAlternateBase be the LAST! */ "fermiAlternateBase"}; /* NOTE: must reflect colors!! */

QColor EColor(const QString& colorString)
{
  for(int i = 0; i < fermiAlternateBase; i++)
  {
	if(QString(colorStrings[i]) == colorString)
	  return EColor(i);
  }
  perr("EColor: string \"%s\" does not correspond to any colour: returning white", qstoc(colorString));
  return Qt::white;
}

QColor EColor(int colorcode)
{
              switch(colorcode)
                {
                        case white:
                                return Qt::white;
                        case red:
                                return KRED90SAT;
                        case green: /* KGREEN non e` il massimo  */
                                return EGREEN;
                        case blue:
                                return KBLUE;
                        case gray:
                                return KGRAY;
                        case cyan:
                                return KCYAN;
                        case orange:
                                return KORANGE;
                        case violet:
                                return KVIOLET;
			case yellow:
				return EYELLOW;
			case black:
				return EBLACK;
			case lightGray:
				return KLIGHTGRAY;
			case darkGray:
				return KDARKGRAY;
			case pink:
				return KPINK;
			case fuchsia:
				return EFUCHSIA;
			case darkGreen:
				return KDARKGREEN;
			case greenWater:
				return EGREENWATER;
			case dirtyYellow:
				return EDIRTYYELLOW;
			case blueGray:
				return EBLUEGRAY;
			case darkRed:
				return KDARKRED;
			case darkMaroon:
				return KDARKMAROON;
			case maroon:
				return KMAROON;
			case camel:
				return KCAMEL;
			case darkYellow:
				return KDARKYELLOW;
			case darkCyan:
				return KDARKCYAN;
			case darkBlue:
				return KDARKBLUE;
			case veryDarkBlue:
				return KVERYDARKBLUE;
			case darkViolet:
				return KDARKVIOLET;
			case darkPink:
				return KDARKPINK;
			case pinkPig:
				return KPINKPIG;
			case darkWater:
				return KDARKWATER;
			case almostBlack:
				return KALMOSTBLACK;
			case boosterColor:
				return BOOSTERCOLOR;
			case elettraColor:
				return ELETTRACOLOR;
   			case fermiColor:
	      			return FERMICOLOR;
			case fermiHighlight:
				return KGRAYFERMIHIGHLIGHT;
			case fermiAlternateBase:
				return KGRAYFERMIALTERNATEBASE;
			case elettraAlternateBase:
				return KELETTRAALTERNATEBASE;
			case elettraHighlight:
				return KELETTRAHIGHLIGHT;
			case boosterHighlight:
				return KBOOSTERHIGHLIGHT;
			case boosterAlternateBase:
				return KBOOSTERALTERNATEBASE;

                        default:
                                //qDebug() << "Color " << colorcode << " not recognized!";
                                return Qt::white;
                }

}

EColorSet::EColorSet(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Available colours");
	QLineEdit *label;
	QGridLayout *layout = new QGridLayout(this);
	QPalette palette;

	const char* colorlist[fermiAlternateBase + 1] =
                { "white",  "darkGray", "gray", "lightGray", "red", "darkRed", "darkMaroon", "maroon", "camel",
                           "orange", "yellow", "dirtyYellow", "darkYellow", "green", "greenWater", "darkGreen",
                             "cyan", "darkCyan", "darkWater", "blue", "darkBlue", "veryDarkBlue",
                               "blueGray", "violet", "darkViolet",
                              "darkPink", "pink", "pinkPig", "fuchsia", "almostBlack", "black", "boosterColor", "elettraColor", "fermiColor",
			"elettraHighlight", "elettraAlternateBase", "boosterHighlight", "boosterAlternateBase",
   			"fermiHighlight", "fermiAlternateBase" };
	int i;
	
	for(i = white; i < fermiColor; i++)
	{
		label = new QLineEdit(this);
		palette.setColor(label->backgroundRole(), EColor(i) );
		label->setMinimumSize(QSize(40, 20) );
		label->setText(QString("%1").arg(colorlist[i]) );
		label->setPalette(palette);
		label->setFrame(false);
		label->setReadOnly(true);
		label->setToolTip(QString("Elettra::%1").arg(colorlist[i]) );
		layout->addWidget(label, i % 5, i / 5);
	}
}

EColorSet::~EColorSet()
{

}

QPalette EPalette(QString def)
{
	QPalette p;
	if(def == "booster")
	{
		p = QPalette(EColor(boosterColor));
		p.setColor(QPalette::Highlight, KBOOSTERHIGHLIGHT );
		p.setColor(QPalette::AlternateBase, KBOOSTERALTERNATEBASE );
	}
	else if(def == "elettra")
	{
		p = QPalette(EColor(elettraColor));
		p.setColor(QPalette::Highlight, KELETTRAHIGHLIGHT );
		p.setColor(QPalette::AlternateBase, KELETTRAALTERNATEBASE);
	}
	else if(def == "fermi")
	{
		p = QPalette(EColor(fermiColor));
		p.setColor(QPalette::Highlight, KGRAYFERMIHIGHLIGHT );
		p.setColor(QPalette::AlternateBase, KGRAYFERMIALTERNATEBASE );
	}
	else
		return qApp->palette();

	return p;
}


namespace Elettra
{

  /* must reflect colors enum!! Look at elettracolors.h */
  
  
void showColorPalette()
{
	EColorSet ecset(0);
	ecset.exec();
}

}

