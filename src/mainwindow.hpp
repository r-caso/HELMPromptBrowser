#pragma once

#include <QCloseEvent>
#include <QPair>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QMainWindow>
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
    QStringList m_currentlySelectedDatasets;
    QStack<QPair<QTreeWidgetItem*, QTreeWidgetItem*>> m_undoStack;
    QStack<QPair<QTreeWidgetItem*, QTreeWidgetItem*>> m_redoStack;
    bool m_DontShowEmptySearchMessage = false;

    void closeEvent(QCloseEvent *event) override;
    void writeSettings();
    void readSettings();
};
