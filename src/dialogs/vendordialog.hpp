#pragma once

#include <QDialog>
#include <QList>

namespace Ui {
class VendorDialog;
} // namespace Ui

class VendorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VendorDialog(QList<int>& vendorList, QWidget *parent = nullptr);
    ~VendorDialog() override;

private slots:
    void on_buttonBox_accepted();

    void on_clearAll_pushButton_clicked();

    void on_selectAll_pushButton_clicked();

private:
    Ui::VendorDialog *ui;
    QList<int>& m_VendorList;
};
