#pragma once

#include <QCloseEvent>
#include <QDebug>
#include <QMainWindow>
#include <QSettings>
#include <QStack>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_search_pushButton_clicked();

    void on_filterByNumber_checkBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_filterBySize_checkBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_filterBySize_CustomInterval_radioButton_toggled(bool checked);

    void on_HELM_Data_pushButton_clicked();

    void on_deselect_all_pushButton_clicked();

    void on_select_all_pushButton_clicked();

    void on_loadFromFile_pushButton_clicked();

    void on_export_pushButton_clicked();

    void on_delete_pushButton_clicked();

    void on_clear_pushButton_clicked();

    void on_undo_pushButton_clicked();

    void on_redo_pushButton_clicked();

    void on_exportOptions_pushButton_clicked();

    void on_prompts_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_selectPrompt_pushButton_clicked();

    void on_deselectPrompt_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString m_helmDataPath;
    QString m_outputPath;
    QString m_jsonFileName;
    QString m_compilationName;
    QString m_helmDataJSON;
    QString m_importFileFolder;
    QStack<QPair<QTreeWidgetItem*, QTreeWidgetItem*>> m_undoStack;
    bool m_DontShowEmptySearchMessage = false;

    void closeEvent(QCloseEvent *event)
    {
        writeSettings();
        event->accept();
    }

    void writeSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "IIF-SADAF-CONICET", "HELMPromptBrowser");

        settings.setValue("HELM_Path", m_helmDataPath);
        settings.setValue("Output_Path", m_outputPath);
        settings.setValue("JSONFile", m_jsonFileName);
        settings.setValue("CompilationName", m_compilationName);
        settings.setValue("HELM_JSON", m_helmDataJSON);
        settings.setValue("IMPORT_JSON_FOLDER", m_importFileFolder);
        settings.setValue("DontShowAgain", m_DontShowEmptySearchMessage);
    }

    void readSettings()
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "IIF-SADAF-CONICET", "HELMPromptBrowser");

        m_helmDataPath = settings.value("HELM_Path").toString();
        m_outputPath = settings.value("Output_Path").toString();
        m_jsonFileName = settings.value("JSONFile").toString();
        m_compilationName = settings.value("CompilationName").toString();
        m_helmDataJSON = settings.value("HELM_JSON").toString();
        m_importFileFolder = settings.value("IMPORT_JSON_FOLDER").toString();
        m_DontShowEmptySearchMessage = settings.value("DontShowAgain").toBool();
    }
};
