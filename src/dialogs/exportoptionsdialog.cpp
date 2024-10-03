#include "exportoptionsdialog.hpp"
#include "ui_exportoptionsdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

ExportOptionsDialog::ExportOptionsDialog(QString& outputPath, QString& outputFile, QString& compilationName, QString& helmDataJson, QWidget *parent)
    : m_outputPath(outputPath)
    , m_outputFile(outputFile)
    , m_compilationName(compilationName)
    , m_helmDataJSON(helmDataJson)
    , QDialog(parent)
    , ui(new Ui::ExportOptionsDialog)
{
    ui->setupUi(this);

    ui->export_path_lineEdit->setText(m_outputPath);
    ui->fileName_lineEdit->setText(m_outputFile);
    ui->compilationName_lineEdit->setText(m_compilationName);
    ui->helmDataJSON_lineEdit->setText(m_helmDataJSON);
}

ExportOptionsDialog::~ExportOptionsDialog()
{
    delete ui;
}

void ExportOptionsDialog::on_buttonBox_accepted()
{
    if (ui->fileName_lineEdit->text().isEmpty() || ui->compilationName_lineEdit->text().isEmpty() || ui->export_path_lineEdit->text().isEmpty()) {
        QMessageBox msg;
        msg.setText("Please, provide all required information");
        msg.exec();
        return;
    }
    m_outputFile = ui->fileName_lineEdit->text();
    m_compilationName = ui->compilationName_lineEdit->text();
    accept();
}

void ExportOptionsDialog::on_export_path_pushButton_clicked()
{
    m_outputPath = QFileDialog::getExistingDirectory(this, "Select output folder", QStandardPaths::displayName(QStandardPaths::DocumentsLocation));
    ui->export_path_lineEdit->setText(m_outputPath);
}


void ExportOptionsDialog::on_helmDataJSON_pushButton_clicked()
{
    m_helmDataJSON = QFileDialog::getOpenFileName(this, "Select helm_tests.json", QStandardPaths::displayName(QStandardPaths::DocumentsLocation), "*.json");
    ui->helmDataJSON_lineEdit->setText(m_helmDataJSON);
}

