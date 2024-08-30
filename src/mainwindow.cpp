#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <map>

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

    const int prompt_tree_column_count = 8;
    const int cid_column = 0;
    ui->prompts_treeWidget->setColumnCount(prompt_tree_column_count);
    ui->prompts_treeWidget->setHeaderLabels({"Custom Dataset ID", "Dataset name / Prompt ID", "Dataset_base", "Dataset_spec", "IsPrompt", "Prompt", "HasSpecs", "Selected"});
    ui->prompts_treeWidget->setColumnWidth(cid_column, 120);
    ui->prompts_treeWidget->hideColumn(2);
    ui->prompts_treeWidget->hideColumn(3);
    ui->prompts_treeWidget->hideColumn(4);
    ui->prompts_treeWidget->hideColumn(5);
    ui->prompts_treeWidget->hideColumn(6);
    ui->prompts_treeWidget->hideColumn(7);
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
            "ms_marco", {
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

static QString prettyPrint(const QJsonObject& obj, const QString& dataset)
{
    const QJsonArray references = obj["references"].toArray();

    const QString prompt_id = obj["id"].toString();

    QString input_text;

    if (!obj["input"].toObject()["text"].toString().isEmpty()) {
        input_text = obj["input"].toObject()["text"].toString() + "\n\n";
    }

    QString references_text;

    if (!references.empty()) {
        references_text += "REFERENCES:\n\n";
        for (auto&& value : references) {
            const QJsonObject reference = value.toObject();
            references_text += "- " + reference["output"].toObject()["text"].toString();
            references_text += " [ ";
            const QJsonArray tags = reference["tags"].toArray();
            for (auto&& tag : tags) {
                references_text += tag.toString() + " ";
            }
            references_text += "]\n";
        }
        references_text += "\n";
    }

    QString subsplit;

    if (obj.contains("sub_split")) {
        subsplit += "SUB-SPLIT: " + obj["sub_split"].toString() + "\n\n";
    }

    QString perturbed;

    if (obj.contains("perturbation")) {
        subsplit += "PERTURBATION: prompt is perturbed";
    }

    QString str = "DATASET: " + dataset + "\n" +
                  "PROMPT ID: " + prompt_id + "\n\n" +
                  input_text +
                  references_text +
                  subsplit +
                  perturbed;

    return str.trimmed();
}

static int Ask(const QString& text, const QString& informative_text, bool& dont_show_again) {
    QMessageBox msg;
    msg.setText(text);
    msg.setInformativeText(informative_text);
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QCheckBox* cb = new QCheckBox();
    cb->setCheckState(Qt::Unchecked);
    cb->setText("Don't show this again");
    msg.setCheckBox(cb);
    int result = msg.exec();
    dont_show_again = cb->checkState();
    return result;
}

static void Warn(const QString& message)
{
    QMessageBox msg;
    msg.setText(message);
    msg.setIcon(QMessageBox::Warning);
    msg.exec();
}

void MainWindow::on_search_pushButton_clicked()
{
    if (ui->HELM_Data_lineEdit->text().isEmpty()) {
        Warn("No HELM data available");
        return;
    }

    if (ui->search_lineEdit->text().isEmpty() && !m_DontShowEmptySearchMessage) {
        if (Ask("Search term is empty.", "This action will match every prompt. Proceed?", m_DontShowEmptySearchMessage) == QMessageBox::No) {
            return;
        }
    }

    const QString search_term = ui->search_lineEdit->text().trimmed().replace("NOT", "!").replace("AND", "&").replace("OR", "|");


    if (!checkQuery(search_term)) {
        Warn("Search query is not well-formed");
        return;
    }

    QList<QPair<QList<QString>, QList<QString>>> queries = getQueries(search_term);
    bool case_sensitive_search = ui->search_case_sensitive_checkBox->isChecked();

    QStringList selected_datasets;
    QStringList filters;

    const QString os = QSysInfo::productType();
    const int top_level_dataset_count = ui->dataset_treeWidget->topLevelItemCount();

    for (int i = 0; i < top_level_dataset_count; ++i) {
        auto parent = ui->dataset_treeWidget->topLevelItem(i);
        if (parent->checkState(0) == Qt::Unchecked) {
            continue;
        }
        if (parent->childCount() == 0) {
            QString dataset_dir = parent->data(0, Qt::DisplayRole).toString();
            selected_datasets.push_back(dataset_dir);
            filters.push_back(os == "windows" ? dataset_dir.replace(":", "_") + "*" : dataset_dir.replace(":", "_") + "*");
            continue;
        }
        for (int j = 0; j < parent->childCount(); ++j) {
            auto child = parent->child(j);
            if (child->checkState(0) == Qt::Checked) {
                QString dataset_dir = child->data(0, Qt::DisplayRole).toString();
                selected_datasets.push_back(dataset_dir);
                filters.push_back(os == "windows" ? dataset_dir.replace(":", "_") + "*" : dataset_dir.replace(":", "_") + "*");
            }
        }
    }

    QStringList task_dirs;

    QDir helm_dir(m_helmDataPath);
    for (const auto& filter : filters) {
        helm_dir.setNameFilters({ filter });
        task_dirs.push_back(helm_dir.entryList().at(0));
    }

    Q_ASSERT_X(task_dirs.size() == selected_datasets.size(), "Taks directories and selected datasets have different cardinalities", "mainwindow.cpp");

    int counter = 0;

    const int cardinality = task_dirs.count();
    for (int j = 0; j < cardinality; ++j) {
        const QString& dir = task_dirs.at(j);
        QFile instances_file(m_helmDataPath + "/" + dir + "/instances.json");
        if (!instances_file.open(QIODevice::ReadOnly)) {
            QMessageBox msg;
            msg.setText("Failed to open instances.json from " + dir);
            msg.exec();
            return;
        }

        const QString dataset = selected_datasets.at(j);

        QString dataset_base;
        QString dataset_spec;

        if (dataset.contains(":")) {
            dataset_base = dataset.split(":").at(0);
            dataset_spec = dataset.split(":").at(1);
        }
        else {
            dataset_base = dataset;
            dataset_spec = {};
        }

        QList<QTreeWidgetItem*> base_item_match = ui->prompts_treeWidget->findItems(dataset_base, Qt::MatchExactly, 1);
        QList<QTreeWidgetItem*> spec_item_match;

        //if (!dataset_base.isEmpty()) {
        if (!dataset_spec.isEmpty()) {
            spec_item_match = ui->prompts_treeWidget->findItems(dataset_spec, Qt::MatchExactly, 1);
        }

        QTreeWidgetItem* base_item = nullptr;

        if (base_item_match.size() > 0) {
            base_item = base_item_match.at(0);
        }
        else {
            base_item = new QTreeWidgetItem();
            base_item->setData(1, Qt::DisplayRole, dataset_base);
            base_item->setData(4, Qt::DisplayRole, false);
            if (dataset_spec.isEmpty()) {
                base_item->setData(6, Qt::DisplayRole, false);
            }
            else {
                base_item->setData(6, Qt::DisplayRole, true);
            }
        }

        QTreeWidgetItem* spec_item = nullptr;
        if (!dataset_spec.isEmpty()) {
            if (spec_item_match.size() > 0) {
                spec_item = spec_item_match.at(0);
            }
            else {
                spec_item = new QTreeWidgetItem();
            }
            spec_item->setData(1, Qt::DisplayRole, dataset_spec);
            spec_item->setData(4, Qt::DisplayRole, false);
            spec_item->setData(6, Qt::DisplayRole, false);
        }

        QTreeWidgetItem* parent = spec_item != nullptr ? spec_item : base_item;

        const QJsonDocument instances = QJsonDocument::fromJson(instances_file.readAll());

        const int instance_count = instances.array().count();
        for (int i = 0; i < instance_count; ++i) {
            counter++;
            const QJsonObject obj = instances.array().at(i).toObject();
            const QString prompt = obj["input"].toObject()["text"].toString();

            const auto prompt_matches_term = [&](const QString& term) { return prompt.contains(term, (case_sensitive_search ? Qt::CaseSensitive : Qt::CaseInsensitive));};

            // for every pair in queries
            // if matches every inclusion, and matches no exclusion, add prompt

            bool prompt_matches = false;

            for (const auto& query : queries) {
                bool matches_all_inclusions = std::ranges::all_of(query.first, prompt_matches_term);
                bool matches_some_exclusion = std::ranges::any_of(query.second, prompt_matches_term);
                if (!matches_all_inclusions || matches_some_exclusion)
                {
                    continue;
                }
                else {
                    prompt_matches = true;
                    break;
                }
            }

            if (!prompt_matches) {
                continue;
            }

            const QString prompt_id = obj["id"].toString();

            QTreeWidgetItem* child = new QTreeWidgetItem();
            child->setFlags(child->flags() | Qt::ItemIsEditable);
            child->setBackground(0, Qt::lightGray);
            child->setForeground(1, Qt::darkGray);
            child->setData(0, Qt::DisplayRole, "");
            child->setData(1, Qt::DisplayRole, prompt_id);
            child->setData(2, Qt::DisplayRole, dataset_base);
            child->setData(3, Qt::DisplayRole, dataset_spec);
            child->setData(4, Qt::DisplayRole, true);
            child->setData(5, Qt::DisplayRole, prettyPrint(obj, dataset));
            child->setData(6, Qt::DisplayRole, false);
            child->setData(7, Qt::DisplayRole, false);
            parent->addChild(child);
        }

        if (spec_item != nullptr) {
            base_item->addChild(parent);
        }
        ui->prompts_treeWidget->addTopLevelItem(base_item);
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
    m_helmDataPath = QFileDialog::getExistingDirectory(this, "Select HELM data folder", QStandardPaths::displayName(QStandardPaths::DocumentsLocation));
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
    int pos = std::max(fromFile.lastIndexOf("\\"), fromFile.lastIndexOf("/"));
    m_importFileFolder = fromFile.sliced(0, pos);
}

static QString getCID(const QTreeWidgetItem* item)
{
    return item->data(0, Qt::DisplayRole).toString();
}

static QString getPID(const QTreeWidgetItem* item)
{
    return item->data(1, Qt::DisplayRole).toString();
}

static QString getName(const QTreeWidgetItem* item)
{
    return item->data(1, Qt::DisplayRole).toString();
}

static QString getDatasetBase(const QTreeWidgetItem* item)
{
    return item->data(2, Qt::DisplayRole).toString();
}

static QString getDatasetSpec(const QTreeWidgetItem* item)
{
    return item->data(3, Qt::DisplayRole).toString();
}

static bool isPrompt(const QTreeWidgetItem* item)
{
    if (item->columnCount() < 7) {
        return false;
    }
    return item->data(4, Qt::DisplayRole).toBool();
}

static QString getPrompt(const QTreeWidgetItem* item)
{
    return item->data(5, Qt::DisplayRole).toString();
}

static bool hasSpecifications(const QTreeWidgetItem* item)
{
    if (item->columnCount() < 7) {
        return false;
    }
    return item->data(6, Qt::DisplayRole).toBool();
}

static QJsonObject getSamples(const QTreeWidgetItem* item)
{
    QJsonObject samples;

    const int child_count = item->childCount();
    for (int i = 0; i < child_count; ++i) {
        const QTreeWidgetItem* child = item->child(i);
        if (child->data(7, Qt::DisplayRole).toBool()) {
            samples.insert(getPID(child), getCID(child));
        }
    }

    return samples;
}

static QJsonObject getDatasetObj(const QTreeWidgetItem* item, const QString& dataset_base, const QString& dataset_spec, const QJsonObject& helm_data_json)
{
    QString metric;
    QString split;

    QString dataset_name = dataset_base;
    if (!dataset_spec.isEmpty()) {
        dataset_name += ":" + dataset_spec;
    }
    for (const auto& array : helm_data_json) {
        for (auto&& obj : array.toArray()) {
            if (obj.toObject()["name"] != dataset_name) {
                continue;
            }
            metric = obj.toObject()["metric"].toString();
            split = obj.toObject()["split"].toString();
        }
    }

    QJsonObject dataset_specification;
    QJsonObject samples = getSamples(item);
    dataset_specification.insert("dataset_spec", dataset_spec);
    dataset_specification.insert("metric", metric);
    dataset_specification.insert("split", split);
    dataset_specification.insert("samples", samples);

    QJsonObject dataset;
    dataset.insert(dataset_base, dataset_specification);

    return dataset;
}

static QJsonObject loadHelmDataConfig(const QString& helm_data_json)
{
    QFile helm_data_json_file(helm_data_json);
    if (!helm_data_json_file.open(QIODevice::ReadOnly)) {
        return {};
    }
    const QJsonDocument helm_dataset_config = QJsonDocument::fromJson(helm_data_json_file.readAll());

    return helm_dataset_config.toVariant().toJsonObject();
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
        QMessageBox msg;
        msg.setText("Failed to open " + m_jsonFileName +  " for writing");
        msg.exec();
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
    QJsonObject compilation_name;
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

    QMessageBox msgBox;
    msgBox.setText("JSON exported");
    msgBox.setStandardButtons(QMessageBox::NoButton);
    QTimer::singleShot(1250, &msgBox, &QMessageBox::accept);
    msgBox.exec();
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
    ui->delete_pushButton->setEnabled(false);
    ui->clear_pushButton->setEnabled(false);
}


void MainWindow::on_undo_pushButton_clicked()
{
    auto [item, parent] = m_undoStack.pop();
    parent->addChild(item);

    // ui->redo_pushButton->setEnabled(true);

    if (m_undoStack.isEmpty()) {
        ui->undo_pushButton->setEnabled(false);
    }
}


void MainWindow::on_redo_pushButton_clicked()
{
    qDebug() << "Not implemented";
}


void MainWindow::on_exportOptions_pushButton_clicked()
{
    ExportOptionsDialog* options = new ExportOptionsDialog(m_outputPath, m_jsonFileName, m_compilationName, m_helmDataJSON);
    options->setAttribute(Qt::WA_DeleteOnClose);
    options->exec();
}


void MainWindow::on_prompts_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current || !isPrompt(current)) {
        return;
    }

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
