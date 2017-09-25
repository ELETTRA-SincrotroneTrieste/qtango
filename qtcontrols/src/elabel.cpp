/* $Id: elabel.cpp,v 1.8 2011-03-07 10:40:10 giacomo Exp $ */
#include <QResizeEvent>
#include <QPainter>
#include "elabel.h"

ELabel::ELabel(QWidget *parent) : ESimpleLabel(parent)
{
	m_falseColor = EColor(Elettra::red);
	m_trueColor = EColor(Elettra::green);
	m_falseString = QString("Zero");
	m_trueString = QString("One");
	/* initialize the vectors of the colours and the strings */
	val = QVariant("No Data");
	last_val = QVariant("Last value not initializeth");
	setAlignment(Qt::AlignCenter);
// 	setWordWrap(true);
	pal = palette();
	if(pal.color(backgroundRole()) != EColor(Elettra::white))
	{
		pal.setColor(backgroundRole(), EColor(Elettra::white));
		setPalette(pal);
	}
	setFrameShape(QFrame::WinPanel);
	setFrameShadow(QFrame::Sunken);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setAutoFillBackground(true);
}

void ELabel::setValue(QVariant v, bool ref)
{
	if ((v.type() == last_val.type()) && (last_val == v))
	{
		//last_val = v;
		qDebug() << "non aggiorno";
		return;
	}
	else
		last_val = val = v;

	if (ref) 
		display();
}

void ELabel::setTrueString(QString s) 
{
	m_trueString = s; 
	display(); 
}

void ELabel::setFalseString(QString s) 
{
	m_falseString = s;
	display(); 
}
		
void ELabel::setTrueColor(QColor c) 
{
	m_trueColor = c; 
	display(); 
}

void ELabel::setFalseColor(QColor c) 
{
	m_falseColor = c;
	display();
}

void ELabel::setBooleanDisplay(QString fS, QString tS, QColor fC, QColor tC)
{
	m_falseString = fS;
	m_trueString = tS;
	m_falseColor = fC;
	m_trueColor = tC;
        display();
}

/* The extended version has got the `SetEnumDisplay()' to configure
 * a string and a color for each desired value 
 */
void ELabel::setEnumDisplay(unsigned int value, QString label, QColor color)
{
	/* If a value has been already set, update it */
	for (int i = 0; i < v_values.size(); i++)
	{
		if (v_values[i] == value)
		{
			v_colors[i] = color;
			v_strings[i] = label;
	                display();
			return;
		}
	}
	/* Put the string and the color into their vectors */
	v_values.push_back(value);
	v_colors.push_back(color);
	v_strings.push_back(label);
	/* refresh the label */
	display();
	return;
}

void ELabel::clearEnumDisplay()
{
        v_values.clear();
        v_colors.clear();
        v_strings.clear();
}

bool ELabel::enumDisplayConfigured()
{
        return (v_values.size() > 0);
}

void ELabel::display()
{
	QPalette palette = this->palette();
	if (val.type() == QVariant::Bool)
	{
		if (val.toBool())
		{
			if (palette.color(backgroundRole()) != m_trueColor || text() != m_trueString)
			{
				palette.setColor(backgroundRole(), m_trueColor);
				setPalette(palette);
				setText(m_trueString);
			}
		}
		else
		{
//			qDebug() << "val.toBool() e` falso!";
			if (palette.color(backgroundRole()) != m_falseColor || m_falseString != text())
			{
				palette.setColor(backgroundRole(), m_falseColor);
				setPalette(palette);
				setText(m_falseString);
			}
		}
	}
	//else if (val.type() == QVariant::UInt)
	else if (val.canConvert(QVariant::UInt) && (v_colors.size()) && (!val.toString().contains("###")))
	{
		/* Look for the value `val' inside the v_values 
		 * vector, to see if a string and a color were 
		 * configured for that value.
		 */
		int index = v_values.indexOf(val.toUInt());
		if (index != -1)
		{
			if(palette.color(backgroundRole()) != v_colors[index])
			{
				palette.setColor(backgroundRole(), v_colors[index]);
				setPalette(palette);
			}
			setText(v_strings[index]);
		}
		else /* No string nor a colour for that value! */
		{
			if(palette.color(backgroundRole()) != QColor("white"))
			{
				palette.setColor(backgroundRole(), QColor("white") );
				setPalette(palette);
			}
			setText(QString("No match for value %1!").arg(val.toUInt() ) );
		}
//		setPalette(pal);
	}
	else
	{
		QString s = val.toString();
		if (s.contains("###"))
		{
			if(palette.color(backgroundRole()) != QColor("white"))
			{
				pal.setColor(backgroundRole(), Qt::white);
				setPalette(pal);
			}
		}
		setText(val.toString());
	}
	qDebug() << "text(): " << text() << "m_trueString: " << m_trueString << "m_falseString: " << m_falseString;
//	QLabel::paintEvent(e);
}




