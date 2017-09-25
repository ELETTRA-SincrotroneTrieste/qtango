#include "tactionlisthelper.h"

#include <QTreeWidget>
#include <QStringList>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDomDocument>
#include <QtDebug>
#include <macros.h>
#include <QGridLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>

#define WIDGET_MAX_ROWS 10

TActionListHelper::TActionListHelper()
{
 m_sectionCnt = 0;
}

QStringList TActionListHelper::getItemStringList(const QString &actionAsXml)
{
    QStringList ret;

    QDomDocument d("TActionDoc");
    if(d.setContent(actionAsXml))
    {
        QDomElement rootEl = d.firstChildElement("ta");
        QDomElement e = rootEl.firstChildElement("pt");
        QDomElement e1 = rootEl.firstChildElement("drm");
        QDomElement e2 = rootEl.firstChildElement("args");
        if(!e.isNull() && !e1.isNull() && !e2.isNull())
            ret << e.text() << e1.text() << e2.text();
        else
            qDebug() << e.isNull() << e1.isNull();
    }
    return ret;
}

QWidget* TActionListHelper::createWidget(const QString& xml)
{
    QGroupBox *w = new QGroupBox(0);
    QGridLayout *lo  = new QGridLayout(w);
    QDomDocument d("TActionDoc");
    {
//        QStringList list = getItemStringList(xml);
//        if(list.size() < 3)
//        {
//            delete w;
//            QTextEdit *te = new QTextEdit(w);
//            lo->addWidget(te, 0, 1, 1, 1);
//            te->setText("<h2>Malformed xml</h2>" + xml);
//            return te;
//        }
//        w->setTitle(QString("%1 [%2]").arg(list.at(0)).arg(list.at(2)));
//        if(list.at(0) == it->text(0) && list.at(2) == it->text(2))
//        {
            /* found action representation */

            if(d.setContent(xml))
            {
              QDomElement root = d.firstChildElement("taction");
              QDomNodeList children = root.childNodes();
              for(int i = 0; i < children.size(); i++)
              {
                  QDomElement el = children.at(i).toElement();
                  if(!el.isNull())
                  {
                      m_addSection(w, el);
                  }
              }

            }
//        }
//        else
//            printf("\e[1;31mno match %s-->%s %s-->%s %s-->%s\e[0m\n", qstoc(list.at(0)), qstoc(it->text(0)),
//                   qstoc(list.at(1)), qstoc(it->text(1)), qstoc(list.at(2)), qstoc(it->text(2)));
    }
    QPushButton *pb = new QPushButton(w);
    pb->setText("Refresh");
    pb->setObjectName("ApplyButton");
    lo->addWidget(pb, lo->rowCount(), 0, 1, 1);
    return w;
}

void TActionListHelper::m_addSection(QWidget *w, const QDomElement &e)
{
    QWidget *ew = NULL;
    QGridLayout *lo = static_cast<QGridLayout *>(w->layout());
    int r = lo->rowCount();
    QString t = e.attribute("type");
    QString a = e.attribute("access");
    QString tooltip;
    if(t == "string" || t == "float")
    {
        QLineEdit *le = new QLineEdit(w);
        le->setText(e.text());
        tooltip = e.text();
        le->setReadOnly(a == "read");
        ew = le;
    }
    else if(t == "int")
    {
        QSpinBox *sb = new QSpinBox(w);
        sb->setMinimum(INT_MIN);
        sb->setMaximum(INT_MAX);
        /* set value after setMinimum and maximum */
        sb->setValue(e.text().toInt());
        sb->setReadOnly(a == "read");
        ew = sb;
    }
    else if(t == "bool")
    {
        QCheckBox *cb  = new QCheckBox(w);
        cb->setChecked(e.text() == "true" || e.text().toInt() != 0);
        ew = cb;
        ew->setDisabled(a == "read");
    }

    if(ew != NULL)
    {
        if(tooltip.isEmpty() && a == "read")
            tooltip += "[Read only]";
        else if( a == "read")
            tooltip += "\n[Read only]";
        ew->setToolTip(tooltip);
        QLabel *label = new QLabel(w);
        label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        label->setText(e.tagName() + ":");
        lo->addWidget(label, m_sectionCnt % WIDGET_MAX_ROWS, 19 * (m_sectionCnt / WIDGET_MAX_ROWS) , 1, 1);
        lo->addWidget(ew, m_sectionCnt % WIDGET_MAX_ROWS, 19 * (m_sectionCnt / WIDGET_MAX_ROWS) + 1, 1, 18);
        m_sectionCnt++;
    }

}
