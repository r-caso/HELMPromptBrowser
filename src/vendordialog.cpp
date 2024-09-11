#include "vendordialog.hpp"
#include "ui_vendordialog.h"

#include <QListWidget>
#include <QListWidgetItem>

VendorDialog::VendorDialog(QList<int>& vendorList, QWidget *parent)
    : m_VendorList(vendorList)
    , QDialog(parent)
    , ui(new Ui::VendorDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Select vendors");

    for (const auto& vendor : {
            "Aleph Alpha",
            "AI21 Labs",
            "Anthropic",
            "Cohere",
            "EleutherAI",
            "LMSYS Org",
            "Meta",
            "Microsoft",
            "Mistral AI",
            "MosaicML",
            "OpenAI",
            "Stanford",
            "TII UAE",
            "Together",
            "Writer",
        })
    {
        auto* item = new QListWidgetItem(vendor);
        item->setCheckState(Qt::Unchecked);
        ui->listWidget->addItem(item);
    }

    if (!m_VendorList.isEmpty()) {
        for (int i : m_VendorList) {
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        }
    }
}

VendorDialog::~VendorDialog()
{
    delete ui;
}

void VendorDialog::on_buttonBox_accepted()
{
    m_VendorList.clear();

    const size_t numberOfRows = ui->listWidget->count();
    for (size_t i = 0; i < numberOfRows; ++i) {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked) {
            m_VendorList.push_back(i);
        }
    }

    accept();
}


void VendorDialog::on_clearAll_pushButton_clicked()
{
    const size_t numberOfRows = ui->listWidget->count();
    for (size_t i = 0; i < numberOfRows; ++i) {
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}


void VendorDialog::on_selectAll_pushButton_clicked()
{
    const size_t numberOfRows = ui->listWidget->count();
    for (size_t i = 0; i < numberOfRows; ++i) {
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

