#pragma once

#include <QDialog>

namespace Ui {
class ExportOptionsDialog;
}

class ExportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportOptionsDialog(QString& output_path, QString& output_file, QString& compilation_name, QString& helm_data_json, QWidget *parent = nullptr);
    ~ExportOptionsDialog();

private slots:
    void on_buttonBox_accepted();

    void on_export_path_pushButton_clicked();

    void on_helmDataJSON_pushButton_clicked();

private:
    Ui::ExportOptionsDialog *ui;

    QString& m_outputPath;
    QString& m_outputFile;
    QString& m_compilationName;
    QString& m_helmDataJSON;
};
