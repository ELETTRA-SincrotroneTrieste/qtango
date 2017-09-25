#ifndef STRINGLIST_EDITOR_H
#define STRINGLIST_EDITOR_H

#include "ui_estringlisteditor.h"

class QListWidget;
class QComboBox;

class EStringListEditor: public QDialog
{
    Q_OBJECT
public:
    EStringListEditor(QWidget * = NULL, const QStringList& initList = QStringList());
    virtual ~EStringListEditor();

    void fillContentsFromListWidget(QListWidget *listWidget);
    void fillContentsFromComboBox(QComboBox *comboBox);

    int count() const;
    QIcon icon(int row) const;
    QString text(int row) const;
	
	QStringList stringList();
	void setList(const QStringList &);

    Ui::StringListEditor ui;
	
  signals:
	void stringList(const QStringList&);
	
  protected slots:
	void accept();

private slots:
    void on_newItemButton_clicked();
    void on_deleteItemButton_clicked();
    void on_moveItemUpButton_clicked();
    void on_moveItemDownButton_clicked();
    void on_listWidget_currentRowChanged(int currentRow);
    void on_listWidget_itemChanged(QListWidgetItem *item);
    void on_itemTextLineEdit_textChanged(const QString &text);

private:
    void updateEditor();

    
};


#endif
