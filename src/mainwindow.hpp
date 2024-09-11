#pragma once

#include <QCloseEvent>
#include <QCompleter>
#include <QList>
#include <QMainWindow>
#include <QPair>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>

#include "languagemodel.hpp"

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
    ~MainWindow() override;

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
    void on_applyDatasetFilters_pushButton_clicked();
    void on_assignCID_pushButton_clicked();
    void on_filter_pushButton_clicked();
    void on_clearCID_pushButton_clicked();
    void on_filterByVendor_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_filterPromptsByCID_pushButton_clicked();
    void on_clearPromptFilter_pushButton_clicked();
    void on_clearDatasetFilters_pushButton_clicked();
    void on_filterByVendor_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString m_helmDataPath;
    QString m_outputPath;
    QString m_jsonFileName;
    QString m_compilationName;
    QString m_helmDataJSON;
    QString m_importFileFolder;
    QStringList m_CIDList;
    QStack<QPair<QTreeWidgetItem*, QTreeWidgetItem*>> m_undoStack;
    QStack<QPair<QTreeWidgetItem*, QTreeWidgetItem*>> m_redoStack;
    QCompleter* m_CIDCompleter;
    QList<int> m_VendorFilterList;
    bool m_DontShowEmptySearchMessage = false;

    const QList<HELMPromptBrowser::LanguageModel> m_Models = {
        HELMPromptBrowser::LanguageModel(0x00, "AlephAlpha_luminous-base", 13e9),
        HELMPromptBrowser::LanguageModel(0x01, "AlephAlpha_luminous-extended", 30e9),
        HELMPromptBrowser::LanguageModel(0x02, "AlephAlpha_luminous-supreme", 70e9),
        HELMPromptBrowser::LanguageModel(0x10, "ai21_j1-grande", 17e9),
        HELMPromptBrowser::LanguageModel(0x11, "ai21_j1-grande-v2-beta", 17e9),
        HELMPromptBrowser::LanguageModel(0x12, "ai21_j1-jumbo", 178e9),
        HELMPromptBrowser::LanguageModel(0x13, "ai21_j1-large", 7.5e9),
        HELMPromptBrowser::LanguageModel(0x14, "ai21_j2-grande", 17e9),
        HELMPromptBrowser::LanguageModel(0x15, "ai21_j2-jumbo", 178e9),
        HELMPromptBrowser::LanguageModel(0x16, "ai21_j2-large", 7.5e9),
        HELMPromptBrowser::LanguageModel(0x20, "anthropic_stanford-online-all-v4-s3", 52e9),
        HELMPromptBrowser::LanguageModel(0x30, "cohere_command-medium-beta", 6.1e9),
        HELMPromptBrowser::LanguageModel(0x31, "cohere_command-xlarge-beta", 52.4e9),
        HELMPromptBrowser::LanguageModel(0x32, "cohere_large-20220720", 13.1e9),
        HELMPromptBrowser::LanguageModel(0x33, "cohere_medium-20220720", 6.1e9),
        HELMPromptBrowser::LanguageModel(0x34, "cohere_medium-20221108", 6.1e9),
        HELMPromptBrowser::LanguageModel(0x35, "cohere_small-20220720", 410e6),
        HELMPromptBrowser::LanguageModel(0x36, "cohere_xlarge-20220609", 52.4e9),
        HELMPromptBrowser::LanguageModel(0x37, "cohere_xlarge-20221108", 52.4e9),
        HELMPromptBrowser::LanguageModel(0x40, "eleutherai_pythia-12b-v0", 12e9),
        HELMPromptBrowser::LanguageModel(0x41, "eleutherai_pythia-1b-v0", 1e9),
        HELMPromptBrowser::LanguageModel(0x42, "eleutherai_pythia-6.9b", 6.9e9),
        HELMPromptBrowser::LanguageModel(0x50, "lmsys_vicuna-13b-v1.3", 13e9),
        HELMPromptBrowser::LanguageModel(0x51, "lmsys_vicuna-7b-v1.3", 7e9),
        HELMPromptBrowser::LanguageModel(0x60, "meta_llama-13b", 13e9),
        HELMPromptBrowser::LanguageModel(0x61, "meta_llama-2-13b", 13e9),
        HELMPromptBrowser::LanguageModel(0x62, "meta_llama-2-70b", 70e9),
        HELMPromptBrowser::LanguageModel(0x63, "meta_llama-2-7b", 7e9),
        HELMPromptBrowser::LanguageModel(0x64, "meta_llama-30b", 30e9),
        HELMPromptBrowser::LanguageModel(0x65, "meta_llama-65b", 65e9),
        HELMPromptBrowser::LanguageModel(0x66, "meta_llama-7b", 7e9),
        HELMPromptBrowser::LanguageModel(0x70, "microsoft_TNLGv2_530B", 530e9),
        HELMPromptBrowser::LanguageModel(0x71, "microsoft_TNLGv2_7B", 7e9),
        HELMPromptBrowser::LanguageModel(0x80, "mistralai_mistral-7b-v0.1", 7e9),
        HELMPromptBrowser::LanguageModel(0x90, "mosaicml_mpt-30b", 30e9),
        HELMPromptBrowser::LanguageModel(0x91, "mosaicml_mpt-instruct-30b", 30e9),
        HELMPromptBrowser::LanguageModel(0xA0, "openai_ada", 350e6),
        HELMPromptBrowser::LanguageModel(0xA1, "openai_babbage", 1.3e9),
        HELMPromptBrowser::LanguageModel(0xA2, "openai_code-cushman-001", 12e9),
        HELMPromptBrowser::LanguageModel(0xA3, "openai_code-davinci-002", 175e9),
        HELMPromptBrowser::LanguageModel(0xA4, "openai_curie", 6.7e9),
        HELMPromptBrowser::LanguageModel(0xA5, "openai_davinci", 175e9),
        HELMPromptBrowser::LanguageModel(0xA6, "openai_gpt-3.5-turbo-0301", 20e9),
        HELMPromptBrowser::LanguageModel(0xA7, "openai_gpt-3.5-turbo-0613", 2e9),
        HELMPromptBrowser::LanguageModel(0xA8, "openai_text-ada-001", 350e6),
        HELMPromptBrowser::LanguageModel(0xA9, "openai_text-babbage-001", 1.3e9),
        HELMPromptBrowser::LanguageModel(0xAA, "openai_text-curie-001", 6.7e9),
        HELMPromptBrowser::LanguageModel(0xAB, "openai_text-davinci-002", 175e9),
        HELMPromptBrowser::LanguageModel(0xAC, "openai_text-davinci-003", 175e9),
        HELMPromptBrowser::LanguageModel(0xB0, "stanford_alpaca-7b", 7e9),
        HELMPromptBrowser::LanguageModel(0xC0, "tiiuae_falcon-40b", 40e9),
        HELMPromptBrowser::LanguageModel(0xC1, "tiiuae_falcon-40b-instruct", 40e9),
        HELMPromptBrowser::LanguageModel(0xC2, "tiiuae_falcon-7b", 7e9),
        HELMPromptBrowser::LanguageModel(0xC3, "tiiuae_falcon-7b-instruct", 7e9),
        HELMPromptBrowser::LanguageModel(0xD0, "together_bloom", 176e9),
        HELMPromptBrowser::LanguageModel(0xD1, "together_glm", 130e9),
        HELMPromptBrowser::LanguageModel(0xD2, "together_gpt-j-6b", 6e9),
        HELMPromptBrowser::LanguageModel(0xD3, "together_gpt-neox-20b", 20e9),
        HELMPromptBrowser::LanguageModel(0xD4, "together_opt-175b", 175e9),
        HELMPromptBrowser::LanguageModel(0xD5, "together_opt-66b", 66e9),
        HELMPromptBrowser::LanguageModel(0xD6, "together_redpajama-incite-base-3b-v1", 3e9),
        HELMPromptBrowser::LanguageModel(0xD7, "together_redpajama-incite-base-7b", 7e9),
        HELMPromptBrowser::LanguageModel(0xD8, "together_redpajama-incite-instruct-3b-v1", 3e9),
        HELMPromptBrowser::LanguageModel(0xD9, "together_redpajama-incite-instruct-7b", 7e9),
        HELMPromptBrowser::LanguageModel(0xDA, "together_t0pp", 11e9),
        HELMPromptBrowser::LanguageModel(0xDB, "together_t5-11b", 11e9),
        HELMPromptBrowser::LanguageModel(0xDC, "together_ul2", 20e9),
        HELMPromptBrowser::LanguageModel(0xDD, "together_yalm", 100e9),
        HELMPromptBrowser::LanguageModel(0xE0, "writer_palmyra-instruct-30", 30e9),
        HELMPromptBrowser::LanguageModel(0xE1, "writer_palmyra-x", 100e9),
    };

    void closeEvent(QCloseEvent *event) override;
    void writeSettings();
    void readSettings();
};
