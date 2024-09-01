#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <tuple>

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QPair>
#include <QSettings>
#include <QShortcut>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QWidgetItem>
#include <QTreeWidgetItem>

#include "exportoptionsdialog.hpp"
#include "helperfunctions.hpp"
#include "queryparser.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    readSettings();

    ui->setupUi(this);
    this->setWindowTitle("HELM Prompt Browser");

    ui->HELM_Data_lineEdit->setText(m_helmDataPath);

    ui->prompts_treeWidget->setColumnCount(prompt_tree_column_count);
    ui->prompts_treeWidget->setHeaderLabels({"Custom Dataset ID", "Dataset name / Prompt ID", "Dataset_base", "Dataset_spec", "IsPrompt", "Prompt", "HasSpecs", "Selected"});
    ui->prompts_treeWidget->setColumnWidth(cid_column, 120);
    ui->prompts_treeWidget->hideColumn(dataset_base_column);
    ui->prompts_treeWidget->hideColumn(dataset_spec_column);
    ui->prompts_treeWidget->hideColumn(is_prompt_column);
    ui->prompts_treeWidget->hideColumn(prompt_contents_column);
    ui->prompts_treeWidget->hideColumn(is_specified_column);
    ui->prompts_treeWidget->hideColumn(selection_status_column);
    ui->prompts_treeWidget->header()->setStretchLastSection(true);
    ui->prompts_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    const std::map<QString, QStringList> HELM_hierarchy = {
        {
            "babi_qa", {
                "babi_qa:task=15",
                "babi_qa:task=19",
                "babi_qa:task=3",
                "babi_qa:task=all",
            }
        },
        {
            "bbq", {
                "bbq:subject=all,method=multiple_choice_joint",
                "bbq:subject=all,method=multiple_choice_separate_calibrated",
                "bbq:subject=all,method=multiple_choice_separate_original",
            }
        },
        {
            "blimp", {
                "blimp:phenomenon=binding,method=multiple_choice_joint",
                "blimp:phenomenon=binding,method=multiple_choice_separate_calibrated",
                "blimp:phenomenon=binding,method=multiple_choice_separate_original",
                "blimp:phenomenon=irregular_forms,method=multiple_choice_joint",
                "blimp:phenomenon=irregular_forms,method=multiple_choice_separate_calibrated",
                "blimp:phenomenon=irregular_forms,method=multiple_choice_separate_original",
                "blimp:phenomenon=island_effects,method=multiple_choice_joint",
                "blimp:phenomenon=island_effects,method=multiple_choice_separate_calibrated",
                "blimp:phenomenon=island_effects,method=multiple_choice_separate_original",
                "blimp:phenomenon=quantifiers,method=multiple_choice_joint",
                "blimp:phenomenon=quantifiers,method=multiple_choice_separate_calibrated",
                "blimp:phenomenon=quantifiers,method=multiple_choice_separate_original",
            }
        },
        {
            "bold:subject=all", {}
        },
        {
            "boolq", {}
        },
        {
            "boolq:only_contrast=True", {}
        },
        {
            "civil_comments", {
                "civil_comments:demographic=LGBTQ",
                "civil_comments:demographic=all",
                "civil_comments:demographic=black",
                "civil_comments:demographic=christian",
                "civil_comments:demographic=female",
                "civil_comments:demographic=male",
                "civil_comments:demographic=muslim",
                "civil_comments:demographic=other_religions",
                "civil_comments:demographic=white",
            }
        },
        {
            "code", {
                "code:dataset=apps",
                "code:dataset=humaneval",
            }
        },
        {
            "commonsense", {
                "commonsense:dataset=hellaswag,method=multiple_choice_joint",
                "commonsense:dataset=hellaswag,method=multiple_choice_separate_calibrated",
                "commonsense:dataset=hellaswag,method=multiple_choice_separate_original",
                "commonsense:dataset=openbookqa,method=multiple_choice_joint",
                "commonsense:dataset=openbookqa,method=multiple_choice_separate_calibrated",
                "commonsense:dataset=openbookqa,method=multiple_choice_separate_original",
            }
        },
        {
            "copyright", {
                "copyright:datatag=n_books_1000-extractions_per_book_1-prefix_length_125",
                "copyright:datatag=oh_the_places",
                "copyright:datatag=popular_books-prefix_length_125.json",
                "copyright:datatag=prompt_num_line_1-min_lines_20.json",
                "copyright:datatag=prompt_num_line_10-min_lines_20.json",
            }
        },
        {
            "disinfo", {
                "disinfo:type=reiteration,topic=climate",
                "disinfo:type=reiteration,topic=covid",
                "disinfo:type=wedging",
            }
        },
        {
            "dyck_language_np=3", {}
        },
        {
            "entity_data_imputation", {
                "entity_data_imputation:dataset=Buy",
                "entity_data_imputation:dataset=Restaurant",
            }
        },
        {
            "entity_matching", {
                "entity_matching:dataset=Abt_Buy",
                "entity_matching:dataset=Beer",
                "entity_matching:dataset=Dirty_iTunes_Amazon",
            }
        },
        {
            "gsm", {}
        },
        {
            "ice", {
                "ice:gender=female",
                "ice:gender=male",
                "ice:subset=ea",
                "ice:subset=hk",
                "ice:subset=ind",
                "ice:subset=usa",
            }
        },
        {
            "imdb", {}
        },
        {
            "imdb:only_contrast=True", {}
        },
        {
            "legal_support", {
                "legal_support,method=multiple_choice_joint",
                "legal_support,method=multiple_choice_separate_calibrated",
                "legal_support,method=multiple_choice_separate_original",
            }
        },
        {
            "lsat_qa", {
                "lsat_qa:task=all,method=multiple_choice_joint",
                "lsat_qa:task=all,method=multiple_choice_separate_calibrated",
                "lsat_qa:task=all,method=multiple_choice_separate_original",
            }
        },
        {
            "math", {
                "math:subject=algebra,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=algebra,level=1,use_official_examples=True,use_chain_of_thought=False",
                "math:subject=counting_and_probability,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=counting_and_probability,level=1,use_official_examples=True,use_chain_of_thought=False",
                "math:subject=geometry,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=geometry,level=1,use_official_examples=True,use_chain_of_thought=False",
                "math:subject=intermediate_algebra,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=intermediate_algebra,level=1,use_official_examples=True,use_chain_of_thought=False",
                "math:subject=number_theory,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=number_theory,level=1,use_official_examples=True,use_chain_of_thought=False",
                "math:subject=prealgebra,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=prealgebra,level=1,use_official_examples=True,use_chain_of_thought=False",
                "math:subject=precalculus,level=1,use_official_examples=False,use_chain_of_thought=True",
                "math:subject=precalculus,level=1,use_official_examples=True,use_chain_of_thought=False",
            }
        },
        {
            "mmlu", {
                "mmlu:subject=abstract_algebra,method=multiple_choice_joint",
                "mmlu:subject=abstract_algebra,method=multiple_choice_separate_calibrated",
                "mmlu:subject=abstract_algebra,method=multiple_choice_separate_original",
                "mmlu:subject=college_chemistry,method=multiple_choice_joint",
                "mmlu:subject=college_chemistry,method=multiple_choice_separate_calibrated",
                "mmlu:subject=college_chemistry,method=multiple_choice_separate_original",
                "mmlu:subject=computer_security,method=multiple_choice_joint",
                "mmlu:subject=computer_security,method=multiple_choice_separate_calibrated",
                "mmlu:subject=computer_security,method=multiple_choice_separate_original",
                "mmlu:subject=econometrics,method=multiple_choice_joint",
                "mmlu:subject=econometrics,method=multiple_choice_separate_calibrated",
                "mmlu:subject=econometrics,method=multiple_choice_separate_original",
                "mmlu:subject=us_foreign_policy,method=multiple_choice_joint",
                "mmlu:subject=us_foreign_policy,method=multiple_choice_separate_calibrated",
                "mmlu:subject=us_foreign_policy,method=multiple_choice_separate_original",
            }
        },
        {
            "msmarco", {
                "msmarco:track=regular,valid_topk=30",
                "msmarco:track=trec,valid_topk=30",
            }
        },
        {
            "narrative_qa", {}
        },
        {
            "natural_qa", {
                "natural_qa:mode=closedbook",
                "natural_qa:mode=openbook_longans",
            }
        },
        {
            "quac", {}
        },
        {
            "raft", {
                "raft:subset=ade_corpus_v2",
                "raft:subset=banking_77",
                "raft:subset=neurips_impact_statement_risks",
                "raft:subset=one_stop_english",
                "raft:subset=overruling",
                "raft:subset=semiconductor_org_types",
                "raft:subset=systematic_review_inclusion",
                "raft:subset=tai_safety_research",
                "raft:subset=terms_of_service",
                "raft:subset=tweet_eval_hate",
                "raft:subset=twitter_complaints",
            }
        },
        {
            "real_toxicity_prompts", {}
        },
        {
            "summarization_cnndm", {
                "summarization_cnndm:temperature=0.3,device=cpu",
                "summarization_cnndm:temperature=0.3,device=cuda",
            }
        },
        {
            "summarization_xsum", {
                "summarization_xsum:temperature=0.3,device=cpu",
                "summarization_xsum:temperature=0.3,device=cuda",
            }
        },
        {
            "synthetic_efficiency:random=None", {}
        },
        {
            "synthetic_reasoning", {
                "synthetic_reasoning:mode=induction",
                "synthetic_reasoning:mode=pattern_match",
                "synthetic_reasoning:mode=variable_substitution",
            }
        },
        {
            "synthetic_reasoning_natural", {
                "synthetic_reasoning_natural:difficulty=easy",
                "synthetic_reasoning_natural:difficulty=hard",
            }
        },
        {
            "the_pile", {
                "the_pile:subset=ArXiv",
                "the_pile:subset=BookCorpus2",
                "the_pile:subset=Enron Emails",
                "the_pile:subset=Github",
                "the_pile:subset=PubMed Central",
                "the_pile:subset=Wikipedia (en)",
            }
        },
        {
            "truthful_qa", {
                "truthful_qa:task=mc_single,method=multiple_choice_joint",
                "truthful_qa:task=mc_single,method=multiple_choice_separate_calibrated",
                "truthful_qa:task=mc_single,method=multiple_choice_separate_original",
            }
        },
        {
            "twitter_aae", {
                "twitter_aae:demographic=aa",
                "twitter_aae:demographic=white",
            }
        },
        {
            "wikifact", {
                "wikifact:k=5,subject=author",
                "wikifact:k=5,subject=currency",
                "wikifact:k=5,subject=discoverer_or_inventor",
                "wikifact:k=5,subject=instance_of",
                "wikifact:k=5,subject=medical_condition_treated",
                "wikifact:k=5,subject=part_of",
                "wikifact:k=5,subject=place_of_birth",
                "wikifact:k=5,subject=plaintiff",
                "wikifact:k=5,subject=position_held",
                "wikifact:k=5,subject=symptoms_and_signs",
            }
        }
    };

    ui->dataset_treeWidget->setColumnCount(1);

    for (const auto& [task, sub_tasks] : HELM_hierarchy) {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setCheckState(0, Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsAutoTristate);
        item->setData(0, Qt::DisplayRole, task);
        for (const auto& sub_task : sub_tasks) {
            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setCheckState(0, Qt::Unchecked);
            child->setData(0, Qt::DisplayRole, sub_task);
            item->addChild(child);
        }
        ui->dataset_treeWidget->addTopLevelItem(item);
    }

    QShortcut* delete_shortcut1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->prompts_treeWidget);
    connect(delete_shortcut1, SIGNAL(activated()), this, SLOT(on_delete_pushButton_clicked()));
    QShortcut* delete_shortcut2 = new QShortcut(QKeySequence(Qt::Key_X), ui->prompts_treeWidget);
    connect(delete_shortcut2, SIGNAL(activated()), this, SLOT(on_delete_pushButton_clicked()));

    QShortcut* undo_shortcut_1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), ui->prompts_treeWidget);
    connect(undo_shortcut_1, SIGNAL(activated()), this, SLOT(on_undo_pushButton_clicked()));
    QShortcut* undo_shortcut_2 = new QShortcut(QKeySequence(Qt::Key_U), ui->prompts_treeWidget);
    connect(undo_shortcut_2, SIGNAL(activated()), this, SLOT(on_undo_pushButton_clicked()));

    QShortcut* redo_shortcut_1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y), ui->prompts_treeWidget);
    connect(redo_shortcut_1, SIGNAL(activated()), this, SLOT(on_redo_pushButton_clicked()));
    QShortcut* redo_shortcut_2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z), ui->prompts_treeWidget);
    connect(redo_shortcut_2, SIGNAL(activated()), this, SLOT(on_redo_pushButton_clicked()));
    QShortcut* redo_shortcut_3 = new QShortcut(QKeySequence(Qt::Key_Y), ui->prompts_treeWidget);
    connect(redo_shortcut_3, SIGNAL(activated()), this, SLOT(on_redo_pushButton_clicked()));

    QShortcut* select_shortcut = new QShortcut(QKeySequence(Qt::Key_S), ui->prompts_treeWidget);
    connect(select_shortcut, SIGNAL(activated()), this, SLOT(on_selectPrompt_pushButton_clicked()));

    QShortcut* deselect_shortcut = new QShortcut(QKeySequence(Qt::Key_D), ui->prompts_treeWidget);
    connect(deselect_shortcut, SIGNAL(activated()), this, SLOT(on_deselectPrompt_pushButton_clicked()));

    QShortcut* clear_shortcut = new QShortcut(QKeySequence(Qt::Key_Y), ui->prompts_treeWidget);
    connect(clear_shortcut, SIGNAL(activated()), this, SLOT(on_clear_pushButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_search_pushButton_clicked()
{
    /*****************************
     * CHECK SOME PRE-REQUISITES *
     *****************************/

    if (ui->HELM_Data_lineEdit->text().isEmpty()) {
        Warn("No HELM data available");
        return;
    }
    if (ui->search_lineEdit->text().isEmpty() && !m_DontShowEmptySearchMessage) {
        if (Ask("Search term is empty.", "This action will match every prompt. Proceed?", m_DontShowEmptySearchMessage) == QMessageBox::No) {
            return;
        }
    }

    /*******************************
     * CHECK QUERY WELL-FORMEDNESS *
     *******************************/

    const QString search_term = ui->search_lineEdit->text().trimmed().replace("NOT", "!").replace("AND", "&").replace("OR", "|");
    if (!checkQuery(search_term)) {
        Warn("Search query is not well-formed");
        return;
    }

    /**************************************
     * GET DATASETS TO ADD TO PROMPT TREE *
     **************************************/

    const QStringList datasets_to_be_added = getDatasetsToAdd(ui->dataset_treeWidget, ui->prompts_treeWidget, m_currentlySelectedDatasets);

    /****************************************************
     * GET DIRECTORIES WHERE INSTANCE FILES ARE LOCATED *
     ****************************************************/

    const QStringList task_dirs = getHelmTaskDirs(datasets_to_be_added, m_helmDataPath);

    Q_ASSERT_X(task_dirs.size() == datasets_to_be_added.size(), "Taks directories and selected datasets have different cardinalities", "mainwindow.cpp");

    /***********************************************
     * PARSE QUERY AND SET SEARCH CASE-SENSITIVITY *
     ***********************************************/

    const QList<QPair<QList<QString>, QList<QString>>> queries = getQueries(search_term);
    const bool search_is_case_sensitive = ui->search_case_sensitive_checkBox->isChecked();

    /************************
     * FINALLY, ADD PROMPTS *
     ************************/

    const size_t task_dirs_count = task_dirs.count();
    for (size_t j = 0; j < task_dirs_count; ++j) {
        const QString& dataset = datasets_to_be_added.at(j);

        const QJsonDocument instances = getTaskInstances(task_dirs.at(j), m_helmDataPath);
        if (instances.isEmpty()) {
            return;
        }

        addPromptsToTree(dataset, instances, queries, search_is_case_sensitive, ui->prompts_treeWidget);
        m_currentlySelectedDatasets.push_back(dataset);
    }

    if (ui->prompts_treeWidget->topLevelItemCount() > 0) {
        ui->delete_pushButton->setEnabled(true);
        ui->clear_pushButton->setEnabled(true);
    }
}

void MainWindow::on_filterByNumber_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui->filterByNumber_comboBox->setEnabled(true);
        ui->filterByNumber_label1->setEnabled(true);
        ui->filterByNumber_label2->setEnabled(true);
    }
    else if (arg1 == Qt::Unchecked) {
        ui->filterByNumber_comboBox->setEnabled(false);
        ui->filterByNumber_label1->setEnabled(false);
        ui->filterByNumber_label2->setEnabled(false);
    }
}

void MainWindow::on_filterBySize_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui->filterBySize_Small_radioButton->setEnabled(true);
        ui->filterBySize_Medium_radioButton->setEnabled(true);
        ui->filterBySize_Large_radioButton->setEnabled(true);
        ui->filterBySize_CustomInterval_radioButton->setEnabled(true);
    }
    else {
        ui->filterBySize_Small_radioButton->setEnabled(false);
        ui->filterBySize_Medium_radioButton->setEnabled(false);
        ui->filterBySize_Large_radioButton->setEnabled(false);
        ui->filterBySize_CustomInterval_radioButton->setEnabled(false);
    }
}

void MainWindow::on_filterBySize_CustomInterval_radioButton_toggled(bool checked)
{
    if (checked) {
        ui->filterBySize_CustomInterval_label1->setEnabled(true);
        ui->filterBySize_CustomInterval_label2->setEnabled(true);
        ui->filterBySize_CustomInterval_label3->setEnabled(true);
        ui->filterBySize_CustomInterval_label4->setEnabled(true);
        ui->filterBySize_CustomInterval_min_comboBox->setEnabled(true);
        ui->filterBySize_CustomInterval_max_comboBox->setEnabled(true);
    }
    else {
        ui->filterBySize_CustomInterval_label1->setEnabled(false);
        ui->filterBySize_CustomInterval_label2->setEnabled(false);
        ui->filterBySize_CustomInterval_label3->setEnabled(false);
        ui->filterBySize_CustomInterval_label4->setEnabled(false);
        ui->filterBySize_CustomInterval_min_comboBox->setEnabled(false);
        ui->filterBySize_CustomInterval_max_comboBox->setEnabled(false);
    }
}

void MainWindow::on_HELM_Data_pushButton_clicked()
{
    const QString path = QFileDialog::getExistingDirectory(this, "Select HELM data folder", QStandardPaths::displayName(QStandardPaths::DocumentsLocation));
    m_helmDataPath = path.isEmpty() ? m_helmDataPath : path;
    ui->HELM_Data_lineEdit->setText(m_helmDataPath);
}

void MainWindow::on_deselect_all_pushButton_clicked()
{
    for (int i = 0; i < ui->dataset_treeWidget->topLevelItemCount(); ++i) {
        ui->dataset_treeWidget->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
    }
}

void MainWindow::on_select_all_pushButton_clicked()
{
    for (int i = 0; i < ui->dataset_treeWidget->topLevelItemCount(); ++i) {
        ui->dataset_treeWidget->topLevelItem(i)->setCheckState(0, Qt::Checked);
    }
}

void MainWindow::on_loadFromFile_pushButton_clicked()
{
    QString fromFile = QFileDialog::getOpenFileName(this, "Select custom compilation file", m_importFileFolder.isEmpty() ? QStandardPaths::displayName(QStandardPaths::DocumentsLocation) : m_importFileFolder, "*.json");

    // replace with proper return status check
    if (fromFile.isEmpty()) {
        return;
    }

    const size_t pos = fromFile.lastIndexOf("/");
    m_importFileFolder = fromFile.sliced(0, pos);

    QFile jsonFile(fromFile);

    if (!jsonFile.open(QIODevice::ReadOnly)) {
        Warn("Unable to open file for import");
        return;
    }

    const int dataset_count = ui->dataset_treeWidget->topLevelItemCount();
    for (int i = 0; i < dataset_count; ++i) {
        ui->dataset_treeWidget->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
    }
    m_currentlySelectedDatasets.clear();

    const QJsonDocument custom_dataset = QJsonDocument::fromJson(jsonFile.readAll());
    const QJsonArray datasets = custom_dataset["datasets"].toArray();

    QMap<QString, QStringList> tree;
    QList<std::tuple<QString, QString, QList<QPair<QString, QString>>>> selected_prompts;

    for (auto&& value : datasets) {
        const QJsonObject dataset = value.toObject();
        const QString dataset_name = dataset.keys().at(0);
        const QString dataset_spec = dataset[dataset_name]["dataset_spec"].toString();

        if (!tree.contains(dataset_name)) {
            tree[dataset_name] = {};
        }
        if (dataset_spec.isEmpty()) {
            continue;
        }
        if (dataset_name == "legal_support") {
            tree[dataset_name].push_back(dataset_name + "," + dataset_spec);
        }
        else {
            tree[dataset_name].push_back(dataset_name + ":" + dataset_spec);
        }

        const QJsonObject samples = dataset[dataset_name]["samples"].toObject();
        const QStringList prompt_ids = samples.keys();
        QList<QPair<QString, QString>> selected;
        for (const QString& prompt_id : prompt_ids) {
            const QString& cid = samples[prompt_id].toString();
            selected.push_back({prompt_id, cid});
        }
        selected_prompts.push_back({dataset_name, dataset_spec, selected});
    }

    for (auto top_level_dataset = tree.cbegin(); top_level_dataset != tree.cend(); ++top_level_dataset) {
        QList<QTreeWidgetItem*> matches = ui->dataset_treeWidget->findItems(top_level_dataset.key(), Qt::MatchExactly, 0);
        const QStringList& sub_datasets = top_level_dataset.value();

        if (sub_datasets.isEmpty()) {
            for (QTreeWidgetItem* item: matches) {
                item->setCheckState(0, Qt::Checked);
            }
        }
        else {
            for (QTreeWidgetItem* item: matches) {
                const size_t child_count = item->childCount();
                for (int i = 0; i < child_count; ++i) {
                    QTreeWidgetItem* child = item->child(i);
                    const QString child_name = child->data(0, Qt::DisplayRole).toString();
                    if (sub_datasets.contains(child_name)) {
                        child->setCheckState(0, Qt::Checked);
                    }
                }
            }
        }
    }

    /******************************
     * CORE PROMPT ADDITION LOGIC *
     ******************************/

    const QStringList datasets_to_be_added = getDatasetsToAdd(ui->dataset_treeWidget, ui->prompts_treeWidget, m_currentlySelectedDatasets);
    const QStringList task_dirs = getHelmTaskDirs(datasets_to_be_added, m_helmDataPath);
    Q_ASSERT_X(task_dirs.size() == datasets_to_be_added.size(), "Taks directories and selected datasets have different cardinalities", "mainwindow.cpp");
    const QList<QPair<QList<QString>, QList<QString>>> queries = {{},{}};
    const bool search_is_case_sensitive = false;
    const size_t task_dirs_count = task_dirs.count();

    for (size_t j = 0; j < task_dirs_count; ++j) {
        const QString& dataset = datasets_to_be_added.at(j);

        const QJsonDocument instances = getTaskInstances(task_dirs.at(j), m_helmDataPath);
        if (instances.isEmpty()) {
            return;
        }

        addPromptsToTree(dataset, instances, queries, search_is_case_sensitive, ui->prompts_treeWidget);
        m_currentlySelectedDatasets.push_back(dataset);
    }

    if (ui->prompts_treeWidget->topLevelItemCount() > 0) {
        ui->delete_pushButton->setEnabled(true);
        ui->clear_pushButton->setEnabled(true);
    }

    /******************
     * SELECT PROMPTS *
     ******************/

    const auto is_selected = [&](QTreeWidgetItem* item) {
        if (!isPrompt(item)) {
            return;
        }

        const QString dataset_base = getDatasetBase(item);
        const QString dataset_spec = getDatasetSpec(item);
        const QString prompt_id = getName(item);
        QString prompt_cid;

        for (const auto& tuple : selected_prompts) {
            if (std::get<0>(tuple) == dataset_base && std::get<1>(tuple) == dataset_spec) {
                auto pair = std::ranges::find_if(std::get<2>(tuple), [&](const QPair<QString, QString>& p) { return p.first == prompt_id; });
                if (pair != std::get<2>(tuple).cend()) {
                    prompt_cid = pair->second;
                    setCID(item, prompt_cid);
                    setSelectedStatus(item, true);
                }
            }
        }
    };

    transformPromptTree(ui->prompts_treeWidget, is_selected);
}

void MainWindow::on_export_pushButton_clicked()
{
    if (ui->prompts_treeWidget->topLevelItemCount() == 0) {
        Warn("Nothing to export");
        return;
    }

    while (m_outputPath.isEmpty() || m_jsonFileName.isEmpty() || m_compilationName.isEmpty() || m_helmDataJSON.isEmpty()) {
        ExportOptionsDialog* options = new ExportOptionsDialog(m_outputPath, m_jsonFileName, m_compilationName, m_helmDataJSON);
        options->setAttribute(Qt::WA_DeleteOnClose);
        int result = options->exec();
        if (result == QDialog::Rejected) {
            return;
        }
    }

    QFile output_file(m_outputPath + "/" + m_jsonFileName + ".json");

    if (!output_file.open(QIODevice::WriteOnly)) {
        Warn("Failed to open " + m_jsonFileName +  " for writing");
        return;
    }

    /*
     * JSON structure:
     *
     * custom_compilation
     *    +- main_object
     *         +- compilation_name
     *         +- dataset_array
     */

    QJsonDocument custom_compilation;
    QJsonObject main_object;
    QJsonArray dataset_array;

    QJsonObject helm_data_json = loadHelmDataConfig(m_helmDataJSON);
    if (helm_data_json.isEmpty()) {
        Warn("Helm Dataset Configuration empty!\nAborting export.");
        return;
    }

    /*
     * dataset_array structure:
     *
     * dataset_array
     *   +- anonymous_obj
     *        +- dataset_base
     *             +- dataset_spec
     *             +- split
     *             +- metric
     *             +- samples
     *                  +- CID
     *                  +- CID
     *   +- anonymous_obj
     *        +- ...
     *   +- ...
     */

    const int item_count = ui->prompts_treeWidget->topLevelItemCount();

    for (int i = 0; i < item_count; ++i) {
        QTreeWidgetItem* parent = ui->prompts_treeWidget->topLevelItem(i);

        const QString dataset_base = getName(parent);

        if (!hasSpecifications(parent)) {
            const QString dataset_spec = {};
            dataset_array.append(getDatasetObj(parent, dataset_base, dataset_spec, helm_data_json));
            continue;
        }

        const int spec_count = parent->childCount();
        for (int i = 0; i < spec_count; ++i) {
            const QTreeWidgetItem* spec = parent->child(i);
            const QString dataset_spec = getName(spec);
            dataset_array.append(getDatasetObj(spec, dataset_base, dataset_spec, helm_data_json));
        }
    }

    main_object.insert("compilation_name", m_compilationName);
    main_object.insert("datasets", dataset_array);

    custom_compilation.setObject(main_object);

    output_file.write(custom_compilation.toJson());

    PopUp("JSON exported");
}

void MainWindow::on_delete_pushButton_clicked()
{
    for (QTreeWidgetItem* current_item : ui->prompts_treeWidget->selectedItems()) {
        QTreeWidgetItem* current_parent = current_item->parent();
        m_undoStack.push({ current_item, current_parent });

        current_parent->removeChild(current_item);
    }

    if (ui->prompts_treeWidget->topLevelItemCount() == 0) {
        ui->clear_pushButton->setEnabled(false);
    }

    ui->undo_pushButton->setEnabled(true);
}

void MainWindow::on_clear_pushButton_clicked()
{
    ui->prompts_treeWidget->clear();
    ui->prompt_plainTextEdit->clear();
    m_currentlySelectedDatasets.clear();
    ui->delete_pushButton->setEnabled(false);
    ui->clear_pushButton->setEnabled(false);
}


void MainWindow::on_undo_pushButton_clicked()
{
    auto [item, parent] = m_undoStack.pop();
    parent->addChild(item);

    m_redoStack.push({item, parent});
    ui->redo_pushButton->setEnabled(true);

    if (m_undoStack.isEmpty()) {
        ui->undo_pushButton->setEnabled(false);
    }
}


void MainWindow::on_redo_pushButton_clicked()
{
    auto [item, parent] = m_redoStack.pop();
    m_undoStack.push({ item, parent });
    parent->removeChild(item);

    ui->undo_pushButton->setEnabled(true);

    if (m_redoStack.isEmpty()) {
        ui->redo_pushButton->setEnabled(false);
    }
}


void MainWindow::on_exportOptions_pushButton_clicked()
{
    ExportOptionsDialog* options = new ExportOptionsDialog(m_outputPath, m_jsonFileName, m_compilationName, m_helmDataJSON);
    options->setAttribute(Qt::WA_DeleteOnClose);
    options->exec();
}


void MainWindow::on_prompts_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current) {
        return;
    }

    if (!isPrompt(current)) {
        ui->delete_pushButton->setEnabled(true);
        return;
    }

    ui->delete_pushButton->setEnabled(false);
    ui->prompt_plainTextEdit->clear();
    const QString prompt = getPrompt(current);
    ui->prompt_plainTextEdit->insertPlainText(prompt);
}

void MainWindow::on_selectPrompt_pushButton_clicked()
{
    for (QTreeWidgetItem* current_item : ui->prompts_treeWidget->selectedItems()) {
        setSelectedStatus(current_item, true);
    }
}

void MainWindow::on_deselectPrompt_pushButton_clicked()
{
    for (QTreeWidgetItem* current_item : ui->prompts_treeWidget->selectedItems()) {
        setSelectedStatus(current_item, false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
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

void MainWindow::readSettings()
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
