#ifndef TACTIONLISTHELPER_H
#define TACTIONLISTHELPER_H

class QTreeWidget;
class QTreeWidgetItem;
class QStringList;
class QWidget;
class QString;
class QDomElement;

class TActionListHelper
{
public:
    TActionListHelper();

    QWidget* createWidget(const QString& xml);
    QStringList getItemStringList(const QString &actionAsXml);

private:
    /* given a QDomElement representing a TAction field, adds to the widget
     * the section aimed at visualizing/editing the fields.
     */
    void m_addSection(QWidget *w, const QDomElement &el);

    int m_sectionCnt;
};

#endif // TACTIONLISTHELPER_H
