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

    const std::map<QString, QList<QPair<QString, int>>> HELM_hierarchy = {
        {
            "babi_qa", QList({
                std::make_pair(QString("babi_qa:task=15"), 69),
                std::make_pair(QString("babi_qa:task=19"), 69),
                std::make_pair(QString("babi_qa:task=3"),  69),
                std::make_pair(QString("babi_qa:task=all"), 69),
            })
        },
        {
            "bbq", QList({
                std::make_pair(QString("bbq:subject=all,method=multiple_choice_joint"), 42),
                std::make_pair(QString("bbq:subject=all,method=multiple_choice_separate_calibrated"), 6),
                std::make_pair(QString("bbq:subject=all,method=multiple_choice_separate_original"), 6),
            })
        },
        {
            "blimp", QList({
                std::make_pair(QString("blimp:phenomenon=binding,method=multiple_choice_joint"),6),
                std::make_pair(QString("blimp:phenomenon=binding,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("blimp:phenomenon=binding,method=multiple_choice_separate_original"), 32),
                std::make_pair(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_joint"),6),
                std::make_pair(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_separate_original"),32),
                std::make_pair(QString("blimp:phenomenon=island_effects,method=multiple_choice_joint"),6),
                std::make_pair(QString("blimp:phenomenon=island_effects,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("blimp:phenomenon=island_effects,method=multiple_choice_separate_original"), 32),
                std::make_pair(QString("blimp:phenomenon=quantifiers,method=multiple_choice_joint"),6),
                std::make_pair(QString("blimp:phenomenon=quantifiers,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("blimp:phenomenon=quantifiers,method=multiple_choice_separate_original"), 32),
            })
        },
        {
            "bold:subject=all", QList({
                std::make_pair(QString(""), 42)
            })
        },
        {
            "boolq", QList({
                std::make_pair(QString(""), 66)
            })
        },
        {
            "boolq:only_contrast=True", QList({
                std::make_pair(QString(""), 42)
            })
        },
        {
            "civil_comments", QList({
                std::make_pair(QString("civil_comments:demographic=LGBTQ"), 66),
                std::make_pair(QString("civil_comments:demographic=all"), 66),
                std::make_pair(QString("civil_comments:demographic=black"), 66),
                std::make_pair(QString("civil_comments:demographic=christian"),66),
                std::make_pair(QString("civil_comments:demographic=female"), 66),
                std::make_pair(QString("civil_comments:demographic=male"), 66),
                std::make_pair(QString("civil_comments:demographic=muslim"), 66),
                std::make_pair(QString("civil_comments:demographic=other_religions"),66),
                std::make_pair(QString("civil_comments:demographic=white"),66),
            })
        },
        {
            "code", QList({
                std::make_pair(QString("code:dataset=apps"), 2),
                std::make_pair(QString("code:dataset=humaneval"), 2)
            })
        },
        {
            "commonsense", QList({
                std::make_pair(QString("commonsense:dataset=hellaswag,method=multiple_choice_joint"), 6),
                std::make_pair(QString("commonsense:dataset=hellaswag,method=multiple_choice_separate_calibrated"),  6),
                std::make_pair(QString("commonsense:dataset=hellaswag,method=multiple_choice_separate_original"), 32),
                std::make_pair(QString("commonsense:dataset=openbookqa,method=multiple_choice_joint"), 6),
                std::make_pair(QString("commonsense:dataset=openbookqa,method=multiple_choice_separate_calibrated"), 32),
                std::make_pair(QString("commonsense:dataset=openbookqa,method=multiple_choice_separate_original"), 6),
            })
        },
        {
            "copyright", QList({
                std::make_pair(QString("copyright:datatag=n_books_1000-extractions_per_book_1-prefix_length_125"), 42),
                std::make_pair(QString("copyright:datatag=oh_the_places"), 10),
                std::make_pair(QString("copyright:datatag=popular_books-prefix_length_125.json"), 42),
                std::make_pair(QString("copyright:datatag=prompt_num_line_1-min_lines_20.json"), 2),
                std::make_pair(QString("copyright:datatag=prompt_num_line_10-min_lines_20.json"),2),
            })
        },
        {
            "disinfo", QList({
                std::make_pair(QString("disinfo:type=reiteration,topic=climate"), 42),
                std::make_pair(QString("disinfo:type=reiteration,topic=covid"), 42),
                std::make_pair(QString("disinfo:type=wedging"), 42),
            })
        },
        {
            "dyck_language_np=3", QList({
                std::make_pair(QString(""), 68)
            })
        },
        {
            "entity_data_imputation", QList({
                std::make_pair(QString("entity_data_imputation:dataset=Buy"), 66),
                std::make_pair(QString("entity_data_imputation:dataset=Restaurant"), 66),
            })
        },
        {
            "entity_matching", QList({
                std::make_pair(QString("entity_matching:dataset=Abt_Buy"), 66),
                std::make_pair(QString("entity_matching:dataset=Beer"), 66),
                std::make_pair(QString("entity_matching:dataset=Dirty_iTunes_Amazon"), 66),
            })
        },
        {
            "gsm", QList({
                std::make_pair(QString(""), 68)
            })
        },
        {
            "ice", QList({
                std::make_pair(QString("ice:gender=female"),32),
                std::make_pair(QString("ice:gender=male"),32),
                std::make_pair(QString("ice:subset=ea"),32),
                std::make_pair(QString("ice:subset=hk"),32),
                std::make_pair(QString("ice:subset=ind"),32),
                std::make_pair(QString("ice:subset=usa"),32),
            })
        },
        {
            "imdb", QList({
                std::make_pair(QString(""), 66)
            })
        },
        {
            "imdb:only_contrast=True", QList({
                std::make_pair(QString(""),42)
            })
        },
        {
            "legal_support", QList({
                std::make_pair(QString("legal_support,method=multiple_choice_joint"), 68),
                std::make_pair(QString("legal_support,method=multiple_choice_separate_calibrated"), 6),
                std::make_pair(QString("legal_support,method=multiple_choice_separate_original"),6),
            })
        },
        {
            "lsat_qa", QList({
                std::make_pair(QString("lsat_qa:task=all,method=multiple_choice_joint"), 68),
                std::make_pair(QString("lsat_qa:task=all,method=multiple_choice_separate_calibrated"), 6),
                std::make_pair(QString("lsat_qa:task=all,method=multiple_choice_separate_original"), 6),
            })
        },
        {
            "math", QList({
                std::make_pair(QString("math:subject=algebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=algebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
                std::make_pair(QString("math:subject=counting_and_probability,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=counting_and_probability,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
                std::make_pair(QString("math:subject=geometry,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=geometry,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
                std::make_pair(QString("math:subject=intermediate_algebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=intermediate_algebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
                std::make_pair(QString("math:subject=number_theory,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=number_theory,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
                std::make_pair(QString("math:subject=prealgebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=prealgebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
                std::make_pair(QString("math:subject=precalculus,level=1,use_official_examples=False,use_chain_of_thought=True"), 68),
                std::make_pair(QString("math:subject=precalculus,level=1,use_official_examples=True,use_chain_of_thought=False"), 68),
            })
        },
        {
            "mmlu", QList({
                std::make_pair(QString("mmlu:subject=abstract_algebra,method=multiple_choice_joint"),66),
                std::make_pair(QString("mmlu:subject=abstract_algebra,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("mmlu:subject=abstract_algebra,method=multiple_choice_separate_original"),6),
                std::make_pair(QString("mmlu:subject=college_chemistry,method=multiple_choice_joint"),66),
                std::make_pair(QString("mmlu:subject=college_chemistry,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("mmlu:subject=college_chemistry,method=multiple_choice_separate_original"),6),
                std::make_pair(QString("mmlu:subject=computer_security,method=multiple_choice_joint"),66),
                std::make_pair(QString("mmlu:subject=computer_security,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("mmlu:subject=computer_security,method=multiple_choice_separate_original"),6),
                std::make_pair(QString("mmlu:subject=econometrics,method=multiple_choice_joint"),66),
                std::make_pair(QString("mmlu:subject=econometrics,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("mmlu:subject=econometrics,method=multiple_choice_separate_original"),6),
                std::make_pair(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_joint"),66),
                std::make_pair(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_separate_original"),6),
            })
        },
        {
            "msmarco", QList({
                std::make_pair(QString("msmarco:track=regular,valid_topk=30"),32),
                std::make_pair(QString("msmarco:track=trec,valid_topk=30"),32),
            })
        },
        {
            "narrative_qa", QList({
                std::make_pair(QString(""), 65)
            })
        },
        {
            "natural_qa", QList({
                std::make_pair(QString("natural_qa:mode=closedbook"), 66),
                std::make_pair(QString("natural_qa:mode=openbook_longans"), 65),
            })
        },
        {
            "quac", QList({
                std::make_pair(QString(""), 65)
            })
        },
        {
            "raft", QList({
                std::make_pair(QString("raft:subset=ade_corpus_v2"),66),
                std::make_pair(QString("raft:subset=banking_77"),66),
                std::make_pair(QString("raft:subset=neurips_impact_statement_risks"),66),
                std::make_pair(QString("raft:subset=one_stop_english"),66),
                std::make_pair(QString("raft:subset=overruling"),66),
                std::make_pair(QString("raft:subset=semiconductor_org_types"),66),
                std::make_pair(QString("raft:subset=systematic_review_inclusion"),66),
                std::make_pair(QString("raft:subset=tai_safety_research"),66),
                std::make_pair(QString("raft:subset=terms_of_service"),66),
                std::make_pair(QString("raft:subset=tweet_eval_hate"),66),
                std::make_pair(QString("raft:subset=twitter_complaints"),66),
            })
        },
        {
            "real_toxicity_prompts", QList({
                std::make_pair(QString(""), 42)
            })
        },
        {
            "summarization_cnndm", QList({
                std::make_pair(QString("summarization_cnndm:temperature=0.3,device=cpu"), 42),
                std::make_pair(QString("summarization_cnndm:temperature=0.3,device=cuda"), 40),
            })
        },
        {
            "summarization_xsum", QList({
                std::make_pair(QString("summarization_xsum:temperature=0.3,device=cpu"), 42),
                std::make_pair(QString("summarization_xsum:temperature=0.3,device=cuda"), 40),
            })
        },
        {
            "synthetic_efficiency:random=None", QList({
                std::make_pair(QString(""), 39)
            })
        },
        {
            "synthetic_reasoning", QList({
                std::make_pair(QString("synthetic_reasoning:mode=induction"),68),
                std::make_pair(QString("synthetic_reasoning:mode=pattern_match"),68),
                std::make_pair(QString("synthetic_reasoning:mode=variable_substitution"),68),
            })
        },
        {
            "synthetic_reasoning_natural", QList({
                std::make_pair(QString("synthetic_reasoning_natural:difficulty=easy"),68),
                std::make_pair(QString("synthetic_reasoning_natural:difficulty=hard"),68),
            })
        },
        {
            "the_pile", QList({
                std::make_pair(QString("the_pile:subset=ArXiv"),24),
                std::make_pair(QString("the_pile:subset=BookCorpus2"),32),
                std::make_pair(QString("the_pile:subset=Enron Emails"),32),
                std::make_pair(QString("the_pile:subset=Github"),24),
                std::make_pair(QString("the_pile:subset=PubMed Central"),24),
                std::make_pair(QString("the_pile:subset=Wikipedia (en)"),32),
            })
        },
        {
            "truthful_qa", QList({
                std::make_pair(QString("truthful_qa:task=mc_single,method=multiple_choice_joint"),66),
                std::make_pair(QString("truthful_qa:task=mc_single,method=multiple_choice_separate_calibrated"),6),
                std::make_pair(QString("truthful_qa:task=mc_single,method=multiple_choice_separate_original"),6),
            })
        },
        {
            "twitter_aae", QList({
                std::make_pair(QString("twitter_aae:demographic=aa"),32),
                std::make_pair(QString("twitter_aae:demographic=white"),32),
            })
        },
        {
            "wikifact", QList({
                std::make_pair(QString("wikifact:k=5,subject=author"),66),
                std::make_pair(QString("wikifact:k=5,subject=currency"),66),
                std::make_pair(QString("wikifact:k=5,subject=discoverer_or_inventor"),66),
                std::make_pair(QString("wikifact:k=5,subject=instance_of"),66),
                std::make_pair(QString("wikifact:k=5,subject=medical_condition_treated"),66),
                std::make_pair(QString("wikifact:k=5,subject=part_of"),65),
                std::make_pair(QString("wikifact:k=5,subject=place_of_birth"),66),
                std::make_pair(QString("wikifact:k=5,subject=plaintiff"),66),
                std::make_pair(QString("wikifact:k=5,subject=position_held"),66),
                std::make_pair(QString("wikifact:k=5,subject=symptoms_and_signs"),66),
            })
        }
    };

    ui->dataset_treeWidget->setColumnCount(2);
    ui->dataset_treeWidget->hideColumn(1);

    for (const auto& [task, sub_task_number_of_models] : HELM_hierarchy) {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setCheckState(0, Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsAutoTristate);
        item->setData(0, Qt::DisplayRole, task);
        for (const auto& [sub_task, number_of_models]  : sub_task_number_of_models) {
            if (sub_task.isEmpty()) {
                item->setData(1, Qt::DisplayRole, number_of_models);
                continue;
            }
            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setCheckState(0, Qt::Unchecked);
            child->setData(0, Qt::DisplayRole, sub_task);
            child->setData(1, Qt::DisplayRole, number_of_models);
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
        ui->filterByNumber_modelNumber_spinBox->setEnabled(true);
        ui->filterByNumber_label1->setEnabled(true);
        ui->filterByNumber_label2->setEnabled(true);
        ui->filterByNumber_FilterModels_pushButton->setEnabled(true);
    }
    else if (arg1 == Qt::Unchecked) {
        ui->filterByNumber_modelNumber_spinBox->setEnabled(false);
        ui->filterByNumber_label1->setEnabled(false);
        ui->filterByNumber_label2->setEnabled(false);
        ui->filterByNumber_FilterModels_pushButton->setEnabled(false);

        const auto enable_all_datasets = [&](QTreeWidgetItem* item) -> void {
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setBackground(0, Qt::NoBrush);

            if (item->parent() != nullptr) {
                QTreeWidgetItem* parent = item->parent();
                parent->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                parent->setBackground(0, Qt::NoBrush);
            }
        };

        transformDatasetTree(ui->dataset_treeWidget, enable_all_datasets);
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

void MainWindow::on_filterByNumber_FilterModels_pushButton_clicked()
{
    const int number_of_models = ui->filterByNumber_modelNumber_spinBox->value();

    const auto disable_datasets_by_number_of_models = [&](QTreeWidgetItem* item) -> void {
        if (item->data(1, Qt::DisplayRole).toInt() < number_of_models) {
            item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
            item->setBackground(0, Qt::lightGray);
            item->setCheckState(0, Qt::Unchecked);

            if (item->parent() != nullptr) {
                QTreeWidgetItem* parent = item->parent();
                const size_t child_count = parent->childCount();
                bool all_children_disabled = true;
                for (size_t i = 0; i < child_count; ++i) {
                    if (parent->child(i)->flags() & Qt::ItemIsUserCheckable) {
                        all_children_disabled = false;
                    }
                }
                if (all_children_disabled) {
                    parent->setFlags(parent->flags() & (~Qt::ItemIsUserCheckable));
                    parent->setBackground(0, Qt::lightGray);
                    parent->setCheckState(0, Qt::Unchecked);
                }
            }
        }
        else {
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setBackground(0, Qt::NoBrush);
            if (item->parent() != nullptr) {
                QTreeWidgetItem* parent = item->parent();
                parent->setFlags(parent->flags() | Qt::ItemIsUserCheckable);
                parent->setBackground(0, Qt::NoBrush);
            }
        }
    };

    transformDatasetTree(ui->dataset_treeWidget, disable_datasets_by_number_of_models);
}

