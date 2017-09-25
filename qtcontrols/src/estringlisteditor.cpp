#include "estringlisteditor.h"

#include <QComboBox>
#include <QVariant>


EStringListEditor::EStringListEditor(QWidget *parent, const QStringList& itemList) : QDialog(parent)
{
    ui.setupUi(this);

    QIcon upIcon(":pixmaps/go-up.png");
    QIcon downIcon(":pixmaps/go-down.png");
    QIcon minusIcon(":pixmaps/list-remove.png");
    QIcon plusIcon(":pixmaps/list-add.png");
    ui.moveItemUpButton->setIcon(upIcon);
    ui.moveItemDownButton->setIcon(downIcon);
    ui.newItemButton->setIcon(plusIcon);
    ui.deleteItemButton->setIcon(minusIcon);
	ui.listWidget->addItems(itemList);  
}

EStringListEditor::~EStringListEditor()
{
}

void  EStringListEditor::setList(const QStringList &list)
{
  ui.listWidget->clear();
  ui.listWidget->addItems(list);
}

void EStringListEditor::fillContentsFromListWidget(QListWidget *listWidget)
{
    setWindowTitle(tr("Edit List Widget"));
    for (int i=0; i<listWidget->count(); ++i) {
        QListWidgetItem *oldItem = listWidget->item(i);
        QListWidgetItem *item = oldItem->clone();
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui.listWidget->addItem(item);
    }

    if (ui.listWidget->count() > 0)
        ui.listWidget->setCurrentRow(0);
    else
        updateEditor();
}

void EStringListEditor::fillContentsFromComboBox(QComboBox *comboBox)
{
    setWindowTitle(tr("Edit Combobox"));
    for (int i=0; i<comboBox->count(); ++i) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(comboBox->itemText(i));
        item->setIcon(comboBox->itemData(i).value<QIcon>());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui.listWidget->addItem(item);
    }

    if (ui.listWidget->count() > 0)
        ui.listWidget->setCurrentRow(0);
    else
        updateEditor();
}

void EStringListEditor::on_newItemButton_clicked()
{
    int row = ui.listWidget->currentRow() + 1;

    QListWidgetItem *item = new QListWidgetItem;
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(tr("New Item"));
    if (row < ui.listWidget->count())
        ui.listWidget->insertItem(row, item);
    else
        ui.listWidget->addItem(item);

    ui.listWidget->setCurrentItem(item);
    ui.listWidget->editItem(item);
}

void EStringListEditor::on_deleteItemButton_clicked()
{
    int row = ui.listWidget->currentRow();

    if (row != -1)
        delete ui.listWidget->takeItem(row);

    if (row == ui.listWidget->count())
        row--;
    if (row < 0)
        updateEditor();
    else
        ui.listWidget->setCurrentRow(row);
}

void EStringListEditor::on_moveItemUpButton_clicked()
{
    int row = ui.listWidget->currentRow();
    if (row <= 0)
        return; // nothing to do

    ui.listWidget->insertItem(row - 1, ui.listWidget->takeItem(row));
    ui.listWidget->setCurrentRow(row - 1);
}

void EStringListEditor::on_moveItemDownButton_clicked()
{
    int row = ui.listWidget->currentRow();
    if (row == -1 || row == ui.listWidget->count() - 1)
        return; // nothing to do

    ui.listWidget->insertItem(row + 1, ui.listWidget->takeItem(row));
    ui.listWidget->setCurrentRow(row + 1);
}

void EStringListEditor::on_listWidget_currentRowChanged(int currentRow)
{
    updateEditor();
    if(currentRow >= 0)
    {
      ui.itemTextLineEdit->setText(ui.listWidget->item(currentRow)->text());
      ui.itemTextLineEdit->selectAll();
    }
    ui.itemTextLineEdit->setFocus();
}

void EStringListEditor::on_listWidget_itemChanged(QListWidgetItem *)
{
    updateEditor();
}

void EStringListEditor::on_itemTextLineEdit_textChanged(const QString &text)
{
    int currentRow = ui.listWidget->currentRow();
    if (currentRow != -1) {
        QListWidgetItem *item = ui.listWidget->item(currentRow);
        item->setText(text);
    }
}

int EStringListEditor::count() const
{
    return ui.listWidget->count();
}

QIcon EStringListEditor::icon(int row) const
{
    return ui.listWidget->item(row)->icon();
}

QString EStringListEditor::text(int row) const
{
    return ui.listWidget->item(row)->text();
}

void EStringListEditor::updateEditor()
{
    bool currentItemEnabled = false;

    bool moveRowUpEnabled = false;
    bool moveRowDownEnabled = false;

    QListWidgetItem *item = ui.listWidget->currentItem();
    if (item) {
        currentItemEnabled = true;
        int currentRow = ui.listWidget->currentRow();
        if (currentRow > 0)
            moveRowUpEnabled = true;
        if (currentRow < ui.listWidget->count() - 1)
            moveRowDownEnabled = true;
    }

    ui.moveItemUpButton->setEnabled(moveRowUpEnabled);
    ui.moveItemDownButton->setEnabled(moveRowDownEnabled);
    ui.deleteItemButton->setEnabled(currentItemEnabled);
    ui.textLabel->setEnabled(currentItemEnabled);
    ui.itemTextLineEdit->setEnabled(currentItemEnabled);

    QString itemText;
    QIcon itemIcon;

    if (item) {
        itemText = item->text();
        itemIcon = item->icon();
    }
    //ui.itemTextLineEdit->setText(itemText);
}

QStringList EStringListEditor::stringList()
{
  QStringList list;
  foreach (QListWidgetItem *it, ui.listWidget->findItems("*", Qt::MatchWildcard))
	list << it->text();
  return list;
}

void EStringListEditor::accept()
{
  QStringList list = stringList();
  emit stringList(list);
  QDialog::accept();
}



