#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <map>
#include <ranges>
#include <tuple>

#include <QCompleter>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QPair>
#include <QSettings>
#include <QShortcut>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QTreeWidgetItem>

#include "exportoptionsdialog.hpp"
#include "helperfunctions.hpp"
#include "hpb_globals.hpp"
#include "queryparser.hpp"
#include "vendordialog.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    readSettings();

    ui->setupUi(this);
    this->setWindowTitle("HELM Prompt Browser");

    ui->HELM_Data_lineEdit->setText(m_helmDataPath);

    m_CIDCompleter = new QCompleter(m_CIDList, this);
    m_CIDCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->filterPromptsByCID_lineEdit->setCompleter(m_CIDCompleter);

    /***********************
     * Set up dataset tree *
     ***********************/

    const std::map<QString, QList<std::tuple<QString, int, QList<int>>>> HELMDatasetHierarchy = {
        {
            "babi_qa", QList({
                std::make_tuple(QString("babi_qa:task=15"), 70, HPB::list_70),
                std::make_tuple(QString("babi_qa:task=19"), 70, HPB::list_70),
                std::make_tuple(QString("babi_qa:task=3"), 70, HPB::list_70),
                std::make_tuple(QString("babi_qa:task=all"), 70, HPB::list_70),
            })
        },
        {
            "bbq", QList({
                std::make_tuple(QString("bbq:subject=all,method=multiple_choice_joint"), 42, HPB::list_42),
                std::make_tuple(QString("bbq:subject=all,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("bbq:subject=all,method=multiple_choice_separate_original"), 6, HPB::list_6),
            })
        },
        {
            "blimp", QList({
                std::make_tuple(QString("blimp:phenomenon=binding,method=multiple_choice_joint"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=binding,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=binding,method=multiple_choice_separate_original"), 32, HPB::list_32),
                std::make_tuple(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_joint"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_separate_original"), 32, HPB::list_32),
                std::make_tuple(QString("blimp:phenomenon=island_effects,method=multiple_choice_joint"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=island_effects,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=island_effects,method=multiple_choice_separate_original"), 32, HPB::list_32),
                std::make_tuple(QString("blimp:phenomenon=quantifiers,method=multiple_choice_joint"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=quantifiers,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("blimp:phenomenon=quantifiers,method=multiple_choice_separate_original"), 32, HPB::list_32),
            })
        },
        {
            "bold:subject=all", QList({
                std::make_tuple(QString(""), 42, HPB::list_42)
            })
        },
        {
            "boolq", QList({
                std::make_tuple(QString(""), 67, HPB::list_67)
            })
        },
        {
            "boolq:only_contrast=True", QList({
                std::make_tuple(QString(""), 42, HPB::list_42)
            })
        },
        {
            "civil_comments", QList({
                std::make_tuple(QString("civil_comments:demographic=LGBTQ"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=all"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=black"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=christian"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=female"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=male"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=muslim"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=other_religions"), 67, HPB::list_67),
                std::make_tuple(QString("civil_comments:demographic=white"), 67, HPB::list_67),
            })
        },
        {
            "code", QList({
                std::make_tuple(QString("code:dataset=apps"), 2, HPB::list_2),
                std::make_tuple(QString("code:dataset=humaneval"), 2, HPB::list_2)
            })
        },
        {
            "commonsense", QList({
                std::make_tuple(QString("commonsense:dataset=hellaswag,method=multiple_choice_joint"), 6, HPB::list_6),
                std::make_tuple(QString("commonsense:dataset=hellaswag,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("commonsense:dataset=hellaswag,method=multiple_choice_separate_original"), 32, HPB::list_32),
                std::make_tuple(QString("commonsense:dataset=openbookqa,method=multiple_choice_joint"), 6, HPB::list_6),
                std::make_tuple(QString("commonsense:dataset=openbookqa,method=multiple_choice_separate_calibrated"), 32, HPB::list_32),
                std::make_tuple(QString("commonsense:dataset=openbookqa,method=multiple_choice_separate_original"), 6, HPB::list_6),
            })
        },
        {
            "copyright", QList({
                std::make_tuple(QString("copyright:datatag=n_books_1000-extractions_per_book_1-prefix_length_125"), 42, HPB::list_42),
                std::make_tuple(QString("copyright:datatag=oh_the_places"), 10, HPB::list_10),
                std::make_tuple(QString("copyright:datatag=popular_books-prefix_length_125.json"), 42, HPB::list_42),
                std::make_tuple(QString("copyright:datatag=prompt_num_line_1-min_lines_20.json"), 2, HPB::list_2),
                std::make_tuple(QString("copyright:datatag=prompt_num_line_10-min_lines_20.json"), 2, HPB::list_2),
            })
        },
        {
            "disinfo", QList({
                std::make_tuple(QString("disinfo:type=reiteration,topic=climate"), 42, HPB::list_42),
                std::make_tuple(QString("disinfo:type=reiteration,topic=covid"), 42, HPB::list_42),
                std::make_tuple(QString("disinfo:type=wedging"), 42, HPB::list_42),
            })
        },
        {
            "dyck_language_np=3", QList({
                std::make_tuple(QString(""), 69, HPB::list_69)
            })
        },
        {
            "entity_data_imputation", QList({
                std::make_tuple(QString("entity_data_imputation:dataset=Buy"), 69, HPB::list_69),
                std::make_tuple(QString("entity_data_imputation:dataset=Restaurant"), 69, HPB::list_69),
            })
        },
        {
            "entity_matching", QList({
                std::make_tuple(QString("entity_matching:dataset=Abt_Buy"), 69, HPB::list_69),
                std::make_tuple(QString("entity_matching:dataset=Beer"), 69, HPB::list_69),
                std::make_tuple(QString("entity_matching:dataset=Dirty_iTunes_Amazon"), 69, HPB::list_69),
            })
        },
        {
            "gsm", QList({
                std::make_tuple(QString(""), 69, HPB::list_69)
            })
        },
        {
            "ice", QList({
                std::make_tuple(QString("ice:gender=female"), 32, HPB::list_32),
                std::make_tuple(QString("ice:gender=male"), 32, HPB::list_32),
                std::make_tuple(QString("ice:subset=ea"), 32, HPB::list_32),
                std::make_tuple(QString("ice:subset=hk"), 32, HPB::list_32),
                std::make_tuple(QString("ice:subset=ind"), 32, HPB::list_32),
                std::make_tuple(QString("ice:subset=usa"), 32, HPB::list_32),
            })
        },
        {
            "imdb", QList({
                std::make_tuple(QString(""), 67, HPB::list_67)
            })
        },
        {
            "imdb:only_contrast=True", QList({
                std::make_tuple(QString(""),42, HPB::list_42)
            })
        },
        {
            "legal_support", QList({
                std::make_tuple(QString("legal_support,method=multiple_choice_joint"), 69, HPB::list_69),
                std::make_tuple(QString("legal_support,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("legal_support,method=multiple_choice_separate_original"), 6, HPB::list_6),
            })
        },
        {
            "lsat_qa", QList({
                std::make_tuple(QString("lsat_qa:task=all,method=multiple_choice_joint"), 69, HPB::list_69),
                std::make_tuple(QString("lsat_qa:task=all,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("lsat_qa:task=all,method=multiple_choice_separate_original"), 6, HPB::list_6),
            })
        },
        {
            "math", QList({
                std::make_tuple(QString("math:subject=algebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=algebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=counting_and_probability,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=counting_and_probability,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=geometry,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=geometry,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=intermediate_algebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=intermediate_algebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=number_theory,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=number_theory,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=prealgebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=prealgebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=precalculus,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HPB::list_69),
                std::make_tuple(QString("math:subject=precalculus,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HPB::list_69),
            })
        },
        {
            "mmlu", QList({
                std::make_tuple(QString("mmlu:subject=abstract_algebra,method=multiple_choice_joint"), 67, HPB::list_67),
                std::make_tuple(QString("mmlu:subject=abstract_algebra,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=abstract_algebra,method=multiple_choice_separate_original"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=college_chemistry,method=multiple_choice_joint"), 67, HPB::list_67),
                std::make_tuple(QString("mmlu:subject=college_chemistry,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=college_chemistry,method=multiple_choice_separate_original"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=computer_security,method=multiple_choice_joint"), 67, HPB::list_67),
                std::make_tuple(QString("mmlu:subject=computer_security,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=computer_security,method=multiple_choice_separate_original"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=econometrics,method=multiple_choice_joint"), 67, HPB::list_67),
                std::make_tuple(QString("mmlu:subject=econometrics,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=econometrics,method=multiple_choice_separate_original"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_joint"), 67, HPB::list_67),
                std::make_tuple(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_separate_original"), 6, HPB::list_6),
            })
        },
        {
            "msmarco", QList({
                std::make_tuple(QString("msmarco:track=regular,valid_topk=30"), 32, HPB::list_32),
                std::make_tuple(QString("msmarco:track=trec,valid_topk=30"), 32, HPB::list_32),
            })
        },
        {
            "narrative_qa", QList({
                std::make_tuple(QString(""), 66, HPB::list_66b)
            })
        },
        {
            "natural_qa", QList({
                std::make_tuple(QString("natural_qa:mode=closedbook"), 67, HPB::list_67),
                std::make_tuple(QString("natural_qa:mode=openbook_longans"), 66, HPB::list_66a),
            })
        },
        {
            "quac", QList({
                std::make_tuple(QString(""), 66, HPB::list_66b)
            })
        },
        {
            "raft", QList({
                std::make_tuple(QString("raft:subset=ade_corpus_v2"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=banking_77"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=neurips_impact_statement_risks"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=one_stop_english"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=overruling"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=semiconductor_org_types"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=systematic_review_inclusion"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=tai_safety_research"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=terms_of_service"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=tweet_eval_hate"), 67, HPB::list_67),
                std::make_tuple(QString("raft:subset=twitter_complaints"), 67, HPB::list_67),
            })
        },
        {
            "real_toxicity_prompts", QList({
                std::make_tuple(QString(""), 42, HPB::list_42)
            })
        },
        {
            "summarization_cnndm", QList({
                std::make_tuple(QString("summarization_cnndm:temperature=0.3,device=cpu"), 42, HPB::list_42),
                std::make_tuple(QString("summarization_cnndm:temperature=0.3,device=cuda"), 40, HPB::list_40),
            })
        },
        {
            "summarization_xsum", QList({
                std::make_tuple(QString("summarization_xsum:temperature=0.3,device=cpu"), 42, HPB::list_42),
                std::make_tuple(QString("summarization_xsum:temperature=0.3,device=cuda"), 40, HPB::list_40),
            })
        },
        {
            "synthetic_efficiency:random=None", QList({
             std::make_tuple(QString(""), 39, HPB::list_39)
            })
        },
        {
            "synthetic_reasoning", QList({
                std::make_tuple(QString("synthetic_reasoning:mode=induction"), 69, HPB::list_69),
                std::make_tuple(QString("synthetic_reasoning:mode=pattern_match"), 69, HPB::list_69),
                std::make_tuple(QString("synthetic_reasoning:mode=variable_substitution"), 69, HPB::list_69),
            })
        },
        {
            "synthetic_reasoning_natural", QList({
                std::make_tuple(QString("synthetic_reasoning_natural:difficulty=easy"), 69, HPB::list_69),
                std::make_tuple(QString("synthetic_reasoning_natural:difficulty=hard"), 69, HPB::list_69),
            })
        },
        {
            "the_pile", QList({
                std::make_tuple(QString("the_pile:subset=ArXiv"), 24, HPB::list_24),
                std::make_tuple(QString("the_pile:subset=BookCorpus2"), 32, HPB::list_32),
                std::make_tuple(QString("the_pile:subset=Enron Emails"), 32, HPB::list_32),
                std::make_tuple(QString("the_pile:subset=Github"), 24, HPB::list_24),
                std::make_tuple(QString("the_pile:subset=PubMed Central"), 24, HPB::list_24),
                std::make_tuple(QString("the_pile:subset=Wikipedia (en)"), 32, HPB::list_32),
            })
        },
        {
            "truthful_qa", QList({
                std::make_tuple(QString("truthful_qa:task=mc_single,method=multiple_choice_joint"), 67, HPB::list_67),
                std::make_tuple(QString("truthful_qa:task=mc_single,method=multiple_choice_separate_calibrated"), 6, HPB::list_6),
                std::make_tuple(QString("truthful_qa:task=mc_single,method=multiple_choice_separate_original"), 6, HPB::list_6),
            })
        },
        {
            "twitter_aae", QList({
                std::make_tuple(QString("twitter_aae:demographic=aa"), 32, HPB::list_32),
                std::make_tuple(QString("twitter_aae:demographic=white"), 32, HPB::list_32),
            })
        },
        {
            "wikifact", QList({
                std::make_tuple(QString("wikifact:k=5,subject=author"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=currency"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=discoverer_or_inventor"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=instance_of"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=medical_condition_treated"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=part_of"), 66, HPB::list_66b),
                std::make_tuple(QString("wikifact:k=5,subject=place_of_birth"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=plaintiff"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=position_held"), 67, HPB::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=symptoms_and_signs"), 67, HPB::list_67),
            })
        }
    };

    ui->dataset_treeWidget->setColumnCount(HPB::DTColumnCount);

    for (int i : _range(HPB::DTNumberOfModels, HPB::DTColumnCount)) {
        ui->dataset_treeWidget->hideColumn(i);
    }

    for (const auto& [task, vector] : HELMDatasetHierarchy) {
        auto *item = new QTreeWidgetItem();
        item->setCheckState(HPB::DTDatasetNameColumn, Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsAutoTristate);
        item->setData(HPB::DTDatasetNameColumn, Qt::DisplayRole, task);
        for (const auto& [subTask, numberOfModels, modelList]  : vector) {
            if (subTask.isEmpty()) {
                item->setData(HPB::DTNumberOfModels, Qt::DisplayRole, numberOfModels);
                item->setData(HPB::DTLMListColumn, Qt::DisplayRole, QVariant::fromValue<QList<int>>(modelList));
                continue;
            }
            auto *child = new QTreeWidgetItem();
            child->setCheckState(HPB::DTDatasetNameColumn, Qt::Unchecked);
            child->setData(HPB::DTDatasetNameColumn, Qt::DisplayRole, subTask);
            child->setData(HPB::DTNumberOfModels, Qt::DisplayRole, numberOfModels);
            child->setData(HPB::DTLMListColumn, Qt::DisplayRole, QVariant::fromValue<QList<int>>(modelList));
            item->addChild(child);
        }
        ui->dataset_treeWidget->addTopLevelItem(item);
    }

    /**********************
     * Set up prompt tree *
     **********************/

    ui->prompts_treeWidget->setColumnCount(HPB::PTColumnCount);
    ui->prompts_treeWidget->setHeaderLabels({"Custom Dataset ID", "Dataset name / Prompt ID"});
    for (int i : _range(HPB::PTDatasetBaseColumn, HPB::PTColumnCount)) {
        ui->prompts_treeWidget->hideColumn(i);
    }
    ui->prompts_treeWidget->setColumnWidth(HPB::PTCIDColumn, 120);
    ui->prompts_treeWidget->header()->setStretchLastSection(true);
    ui->prompts_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    /*********************
     * Editing shortcuts *
     *********************/

    auto *deleteShortcut1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->prompts_treeWidget);
    connect(deleteShortcut1, SIGNAL(activated()), this, SLOT(on_delete_pushButton_clicked()));
    auto *undoShortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), ui->prompts_treeWidget);
    connect(undoShortcut1, SIGNAL(activated()), this, SLOT(on_undo_pushButton_clicked()));
    auto *redoShortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y), ui->prompts_treeWidget);
    connect(redoShortcut1, SIGNAL(activated()), this, SLOT(on_redo_pushButton_clicked()));
    auto *redoShortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z), ui->prompts_treeWidget);
    connect(redoShortcut2, SIGNAL(activated()), this, SLOT(on_redo_pushButton_clicked()));
    auto *clearShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), ui->prompts_treeWidget);
    connect(clearShortcut, SIGNAL(activated()), this, SLOT(on_clear_pushButton_clicked()));

    auto *selectShortcut = new QShortcut(QKeySequence(Qt::Key_S), ui->prompts_treeWidget);
    connect(selectShortcut, SIGNAL(activated()), this, SLOT(on_selectPrompt_pushButton_clicked()));
    auto *deselectShortcut = new QShortcut(QKeySequence(Qt::Key_D), ui->prompts_treeWidget);
    connect(deselectShortcut, SIGNAL(activated()), this, SLOT(on_deselectPrompt_pushButton_clicked()));
    auto *assignCIDShortcut = new QShortcut(QKeySequence(Qt::Key_A), ui->prompts_treeWidget);
    connect(assignCIDShortcut, SIGNAL(activated()), this, SLOT(on_assignCID_pushButton_clicked()));
    auto *clearCIDShortcut = new QShortcut(QKeySequence(Qt::Key_C), ui->prompts_treeWidget);
    connect(clearCIDShortcut, SIGNAL(activated()), this, SLOT(on_clearCID_pushButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_HELM_Data_pushButton_clicked()
{
    const QString path = QFileDialog::getExistingDirectory(this, "Select HELM data folder", QStandardPaths::displayName(QStandardPaths::DocumentsLocation));
    m_helmDataPath = path.isEmpty() ? m_helmDataPath : path;
    ui->HELM_Data_lineEdit->setText(m_helmDataPath);
}

/*******************************************
 * Dataset selection and filtering widgets *
 *******************************************/

void MainWindow::on_select_all_pushButton_clicked()
{
    const auto checkDataset = [&](QTreeWidgetItem* dataset) -> void { dataset->setCheckState(HPB::DTDatasetNameColumn, Qt::Checked); };
    transformDatasetTree(ui->dataset_treeWidget, checkDataset);
}
void MainWindow::on_deselect_all_pushButton_clicked()
{
    const auto uncheckDataset = [&](QTreeWidgetItem* dataset) -> void { dataset->setCheckState(HPB::DTDatasetNameColumn, Qt::Unchecked); };
    transformDatasetTree(ui->dataset_treeWidget, uncheckDataset);
}

namespace {
    QPair<QMap<QString, QStringList>, QList<std::tuple<QString, QString, QMap<QString, QString>>>> extractDataFromJSON(QFile& jsonFile)
    {
        const QJsonDocument customCompilation = QJsonDocument::fromJson(jsonFile.readAll());

        /*
         * Import JSON structure:
         *
         * customCompilation
         *    +- mainObject
         *         +- compilationName
         *         +- datasetArray
         *              +- anonymous_obj
         *                   +- dataset_base
         *                        +- dataset_spec
         *                        +- split
         *                        +- metric
         *                        +- samples
         *                             +- CID
         *                             +- CID
         *              +- anonymous_obj
         *                   +- ...
         *              +- ...
         */

        const QJsonArray datasetArray = customCompilation["datasets"].toArray();

        QMap<QString, QStringList> selectedDatasets;
        QList<std::tuple<QString, QString, QMap<QString, QString>>> selectedPrompts;

        for (auto&& value : datasetArray) {
            const QJsonObject dataset = value.toObject();
            const QString datasetName = dataset.keys().at(0);
            const QString datasetSpec = dataset[datasetName]["dataset_spec"].toString();

            if (!selectedDatasets.contains(datasetName)) {
                selectedDatasets[datasetName] = {};
            }
            if (datasetSpec.isEmpty()) {
                continue;
            }
            if (datasetName == "legal_support") {
                selectedDatasets[datasetName].push_back(datasetName + "," + datasetSpec);
            }
            else {
                selectedDatasets[datasetName].push_back(datasetName + ":" + datasetSpec);
            }

            const QJsonObject samples = dataset[datasetName]["samples"].toObject();
            QMap<QString, QString> selected;
            std::ranges::for_each(samples.keys(), [&](const QString& promptId) { selected[promptId] = samples[promptId].toString(); });
            selectedPrompts.push_back({ datasetName, datasetSpec, selected });
        }

        return { selectedDatasets, selectedPrompts };
    }

    void restoreDatasetSelection(const QMap<QString, QStringList>& selectedDatasets, QTreeWidget* datasetTree)
    {
        const auto uncheckDataset = [&](QTreeWidgetItem* dataset) -> void { dataset->setCheckState(HPB::DTDatasetNameColumn, Qt::Unchecked); };
        transformDatasetTree(datasetTree, uncheckDataset);

        for (const auto& [datasetName, subDatasets] : selectedDatasets.asKeyValueRange()) {
            QTreeWidgetItem* dataset = datasetTree->findItems(datasetName, Qt::MatchExactly, HPB::DTDatasetNameColumn).at(0);

            if (subDatasets.isEmpty()) {
                dataset->setCheckState(0, Qt::Checked);
                continue;
            }

            const int childCount = dataset->childCount();
            for (int i : _range(0, childCount)) {
                QTreeWidgetItem* child = dataset->child(i);
                const QString childName = child->data(HPB::DTDatasetNameColumn, Qt::DisplayRole).toString();
                if (subDatasets.contains(childName)) {
                    child->setCheckState(HPB::DTDatasetNameColumn, Qt::Checked);
                }
            }
        }
    }
}

void MainWindow::on_loadFromFile_pushButton_clicked()
{
    /********************************
     * 1. Open JSON file for import *
     ********************************/

    const QString fromFile = QFileDialog::getOpenFileName(
        this,
        "Select custom compilation file",
        m_importFileFolder.isEmpty() ? QStandardPaths::displayName(QStandardPaths::DocumentsLocation)
                                     : m_importFileFolder,
        "*.json"
        );

    if (fromFile.isEmpty()) {
        return;
    }

    const qsizetype pos = fromFile.lastIndexOf("/");
    m_importFileFolder = fromFile.sliced(0, pos);

    QFile jsonFile(fromFile);

    if (!jsonFile.open(QIODevice::ReadOnly)) {
        Warn("Unable to open file for import");
        return;
    }


    /***********************************************
     * 2. Extract selection and CID data from JSON *
     ***********************************************/

    const auto [selectedDatasets, selectedPrompts] = extractDataFromJSON(jsonFile);


    /***************************************
     * 3. Restore dataset selection status *
     ***************************************/

    restoreDatasetSelection(selectedDatasets, ui->dataset_treeWidget);


    /*********************************
     * 4. Add prompts to prompt tree *
     *********************************/

    const QStringList datasetsToBeAdded = getSelectedDatasetNames(ui->dataset_treeWidget);
    const QStringList taskDirs = getHelmTaskDirs(datasetsToBeAdded, m_helmDataPath);
    Q_ASSERT_X(taskDirs.size() == datasetsToBeAdded.size(), "Taks directories and selected datasets have different cardinalities", "mainwindow.cpp");
    const qsizetype taskDirsCount = taskDirs.count();

    for (qsizetype j : _range(0, taskDirsCount)) {
        const QString& dataset = datasetsToBeAdded.at(j);

        const QJsonDocument instances = getTaskInstances(taskDirs.at(j), m_helmDataPath);
        if (instances.isEmpty()) {
            return;
        }

        addPromptsToTree(dataset, instances, {{},{}}, false, false, ui->prompts_treeWidget);
    }


    /*******************************************************************
     * 5. Restore prompt selection status and store CIDs for completer *
     *******************************************************************/

    // has a side-effect on m_CIDList
    const auto restorePromptTreeData = [&](QTreeWidgetItem* item) -> void {
        if (item == nullptr) {
            return;
        }
        if (!isPrompt(item)) {
            return;
        }

        const QString datasetBase = getDatasetBase(item);
        const QString datasetSpec = getDatasetSpec(item);
        const QString promptId = getName(item);
        QString promptCId;

        for (const auto& [db, ds, idCIdMap] : selectedPrompts) {
            if (db != datasetBase || ds != datasetSpec) {
                continue;
            }

            if (!idCIdMap.contains(promptId)) {
                return;
            }

            promptCId = idCIdMap[promptId];
            setCID(item, promptCId);
            setSelectedStatus(item, true);

            if (!m_CIDList.contains(promptCId)) {
                m_CIDList.push_back(promptCId);
            }
        }
    };

    m_CIDList.clear();
    transformPromptTree(ui->prompts_treeWidget, restorePromptTreeData);
    auto* model = dynamic_cast<QStringListModel*>(m_CIDCompleter->model());
    model->setStringList(m_CIDList);

    /*************************
     * 6. Manage GUI changes *
     *************************/

    if (ui->prompts_treeWidget->topLevelItemCount() > 0) {
        ui->delete_pushButton->setEnabled(true);
        ui->clear_pushButton->setEnabled(true);
        ui->selectPrompt_pushButton->setEnabled(true);
        ui->deselectPrompt_pushButton->setEnabled(true);
        ui->assignCID_pushButton->setEnabled(true);
        ui->clearCID_pushButton->setEnabled(true);
    }
}

void MainWindow::on_filterByNumber_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui->filterByNumber_modelNumber_spinBox->setEnabled(true);
        ui->filterByNumber_label1->setEnabled(true);
        ui->filterByNumber_label2->setEnabled(true);
        ui->applyDatasetFilters_pushButton->setEnabled(true);
        ui->clearDatasetFilters_pushButton->setEnabled(true);
    }
    else {
        ui->filterByNumber_modelNumber_spinBox->setEnabled(false);
        ui->filterByNumber_label1->setEnabled(false);
        ui->filterByNumber_label2->setEnabled(false);

        if (ui->filterBySize_checkBox->checkState() == Qt::Unchecked && ui->filterByVendor_checkBox->checkState() == Qt::Unchecked) {
            ui->applyDatasetFilters_pushButton->setEnabled(false);
            ui->clearDatasetFilters_pushButton->setEnabled(false);
            on_clearDatasetFilters_pushButton_clicked();
        }
    }
}
void MainWindow::on_filterBySize_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui->filterBySize_ReallyTiny_radioButton->setEnabled(true);
        ui->filterBySize_Tiny_radioButton->setEnabled(true);
        ui->filterBySize_Small_radioButton->setEnabled(true);
        ui->filterBySize_Medium_radioButton->setEnabled(true);
        ui->filterBySize_Large_radioButton->setEnabled(true);
        ui->filterBySize_CustomInterval_radioButton->setEnabled(true);
        ui->applyDatasetFilters_pushButton->setEnabled(true);
        ui->clearDatasetFilters_pushButton->setEnabled(true);
        if (ui->filterBySize_CustomInterval_radioButton->isChecked()) {
            ui->filterBySize_CustomInterval_min_comboBox->setEnabled(true);
            ui->filterBySize_CustomInterval_max_comboBox->setEnabled(true);
            ui->filterBySize_CustomInterval_label1->setEnabled(true);
            ui->filterBySize_CustomInterval_label2->setEnabled(true);
            ui->filterBySize_CustomInterval_label3->setEnabled(true);
            ui->filterBySize_CustomInterval_label4->setEnabled(true);
        }
    }
    else {
        ui->filterBySize_ReallyTiny_radioButton->setEnabled(false);
        ui->filterBySize_Tiny_radioButton->setEnabled(false);
        ui->filterBySize_Small_radioButton->setEnabled(false);
        ui->filterBySize_Medium_radioButton->setEnabled(false);
        ui->filterBySize_Large_radioButton->setEnabled(false);
        ui->filterBySize_CustomInterval_radioButton->setEnabled(false);
        if (ui->filterByNumber_checkBox->checkState() == Qt::Unchecked && ui->filterByVendor_checkBox->checkState() == Qt::Unchecked) {
            ui->applyDatasetFilters_pushButton->setEnabled(false);
            ui->clearDatasetFilters_pushButton->setEnabled(false);
            on_clearDatasetFilters_pushButton_clicked();
        }
        ui->filterBySize_CustomInterval_min_comboBox->setEnabled(false);
        ui->filterBySize_CustomInterval_max_comboBox->setEnabled(false);
        ui->filterBySize_CustomInterval_label1->setEnabled(false);
        ui->filterBySize_CustomInterval_label2->setEnabled(false);
        ui->filterBySize_CustomInterval_label3->setEnabled(false);
        ui->filterBySize_CustomInterval_label4->setEnabled(false);
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
void MainWindow::on_filterByVendor_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        ui->filterByVendor_pushButton->setEnabled(true);
        ui->applyDatasetFilters_pushButton->setEnabled(true);
        ui->clearDatasetFilters_pushButton->setEnabled(true);
    }
    else {
        ui->filterByVendor_pushButton->setEnabled(false);
        if (ui->filterBySize_checkBox->checkState() == Qt::Unchecked && ui->filterByNumber_checkBox->checkState() == Qt::Unchecked) {
            ui->applyDatasetFilters_pushButton->setEnabled(false);
            ui->clearDatasetFilters_pushButton->setEnabled(false);
            on_clearDatasetFilters_pushButton_clicked();
        }
    }
}
void MainWindow::on_filterByVendor_pushButton_clicked()
{
    auto* dialog = new VendorDialog(m_VendorFilterList);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}
void MainWindow::on_applyDatasetFilters_pushButton_clicked()
{
    on_clearDatasetFilters_pushButton_clicked();

    int numberOfModels = 2;
    double min = 350e6;
    double max = 531e9;

    if (ui->filterByVendor_checkBox->isChecked()) {
        if (m_VendorFilterList.isEmpty()) {
            PopUp("Select at least one vendor");
            return;
        }
    }
    else {
        m_VendorFilterList.clear();
    }

    if (ui->filterByNumber_checkBox->isChecked()) {
        numberOfModels = ui->filterByNumber_modelNumber_spinBox->value();
    }

    if (ui->filterBySize_checkBox->isChecked()) {
        switch(ui->filterBySize_buttonGroup->checkedId()) {
        case -1:
            break;
        case -3:
            max = 1e9;
            break;
        case -2:
            min = 1e9; max = 20e9;
            break;
        case -5:
            min = 20e9; max = 70e9;
            break;
        case -6:
            min = 70e9; max = 180e9;
            break;
        case -7:
            min = 180e9;
            break;
        case -4:
            min = ui->filterBySize_CustomInterval_min_comboBox->currentText().toDouble();
            max = ui->filterBySize_CustomInterval_max_comboBox->currentText().toDouble();
            break;
        default:
            break;
        }
    }

    const auto filter = [&](QTreeWidgetItem* item) -> void {
        if (item == nullptr || item->isHidden()) {
            return;
        }

        const bool has_enough_models = item->data(HPB::DTNumberOfModels, Qt::DisplayRole).toInt() >= numberOfModels;
        bool parameters_in_range = false;
        bool tested_on_vendor = false;

        const QList<QVariant>& model_id_list_variant = item->data(HPB::DTLMListColumn, Qt::DisplayRole).toList();
        for (const auto& model_id_variant : model_id_list_variant) {
            int model_id = model_id_variant.toInt();
            const auto model = std::ranges::find_if(m_Models, [&](const LanguageModel& model) { return model.id() == model_id; });
            if ((m_VendorFilterList.isEmpty() || m_VendorFilterList.contains(static_cast<int>(model->vendor()))) && (model->parameters() >= min && model->parameters() < max)) {
                parameters_in_range = true;
                tested_on_vendor = true;
                break;
            }
        }

        if (has_enough_models && parameters_in_range && tested_on_vendor) {
            return;
        }

        item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
        item->setHidden(true);

        if (item->parent() == nullptr) {
            return;
        }

        QTreeWidgetItem* parent = item->parent();
        const int childCount = parent->childCount();
        bool all_children_disabled = true;
        for (int i : _range(0, childCount)) {
            if (!parent->child(i)->isHidden()) {
                all_children_disabled = false;
                break;
            }
        }
        if (all_children_disabled) {
            parent->setFlags(parent->flags() & (~Qt::ItemIsUserCheckable));
            parent->setHidden(true);
        }
    };

    transformDatasetTree(ui->dataset_treeWidget, filter);
}
void MainWindow::on_clearDatasetFilters_pushButton_clicked()
{
    const auto enableAllDatasets = [&](QTreeWidgetItem* item) -> void {
        if (item == nullptr) {
            return;
        }
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setHidden(false);

        if (item->parent() != nullptr) {
            QTreeWidgetItem* parent = item->parent();
            parent->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate);
            parent->setHidden(false);
        }
    };

    transformDatasetTree(ui->dataset_treeWidget, enableAllDatasets);
}

/****************************
 * Prompt wrangling widgets *
 ****************************/

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

    const QString searchTerm = ui->search_lineEdit->text().trimmed().replace("NOT", "!").replace("AND", "&").replace("OR", "|");
    if (!checkQuery(searchTerm)) {
        Warn("Search query is not well-formed");
        return;
    }

    /**************************************
     * GET DATASETS TO ADD TO PROMPT TREE *
     **************************************/

    const QStringList datasetsToBeAdded = getSelectedDatasetNames(ui->dataset_treeWidget);

    /****************************************************
     * GET DIRECTORIES WHERE INSTANCE FILES ARE LOCATED *
     ****************************************************/

    const QStringList taskDirs = getHelmTaskDirs(datasetsToBeAdded, m_helmDataPath);

    Q_ASSERT_X(taskDirs.size() == datasetsToBeAdded.size(), "Taks directories and selected datasets have different cardinalities", "mainwindow.cpp");

    /***********************************************
     * PARSE QUERY AND SET SEARCH CASE-SENSITIVITY *
     ***********************************************/

    const QList<QPair<QList<QString>, QList<QString>>> queries = getQueries(searchTerm);
    const bool searchIsCaseSensitive = ui->search_case_sensitive_checkBox->isChecked();
    const bool searchIsRegex = ui->searchRegex_checkBox->isChecked();

    /************************
     * FINALLY, ADD PROMPTS *
     ************************/

    const qsizetype taskDirscount = taskDirs.count();
    for (qsizetype j : _range(0,taskDirscount)) {
        const QString& dataset = datasetsToBeAdded.at(j);

        const QJsonDocument instances = getTaskInstances(taskDirs.at(j), m_helmDataPath);
        if (instances.isEmpty()) {
            return;
        }

        addPromptsToTree(dataset, instances, queries, searchIsCaseSensitive, searchIsRegex, ui->prompts_treeWidget);
    }

    if (ui->prompts_treeWidget->topLevelItemCount() > 0) {
        ui->delete_pushButton->setEnabled(true);
        ui->clear_pushButton->setEnabled(true);
        ui->selectPrompt_pushButton->setEnabled(true);
        ui->deselectPrompt_pushButton->setEnabled(true);
        ui->assignCID_pushButton->setEnabled(true);
        ui->clearCID_pushButton->setEnabled(true);
    }
    else {
        PopUp("No match found in selected datasets");
    }
}
void MainWindow::on_filter_pushButton_clicked()
{
    /*****************************
     * CHECK SOME PRE-REQUISITES *
     *****************************/

    if (ui->prompts_treeWidget->topLevelItemCount() == 0){
        Warn("Nothing to filter!");
        return;
    }

    if (ui->filter_lineEdit->text().isEmpty()) {
        Warn("No filtering condition has been provided");
        return;
    }

    /*******************************
     * CHECK QUERY WELL-FORMEDNESS *
     *******************************/

    const QString filter_term = ui->filter_lineEdit->text().trimmed().replace("NOT", "!").replace("AND", "&").replace("OR", "|");
    if (!checkQuery(filter_term)) {
        Warn("Filter query is not well-formed");
        return;
    }

    /***********************************************
     * PARSE QUERY AND SET SEARCH CASE-SENSITIVITY *
     ***********************************************/

    const QList<QPair<QList<QString>, QList<QString>>> queries = getQueries(filter_term);
    const bool filterIsCaseSensitive = ui->filter_case_sensitive_checkBox->isChecked();
    const bool filterIsRegex = ui->filterRegex_checkBox->isChecked();

    /***************************
     * FINALLY, FILTER PROMPTS *
     ***************************/

    const auto filter_prompt = [&](QTreeWidgetItem* item) -> void {
        if (item == nullptr) {
            return;
        }
        const QString& prompt = getPrompt(item);
        if (matches(prompt, queries, filterIsCaseSensitive, filterIsRegex)) {
            QTreeWidgetItem* parent = item->parent();
            parent->removeChild(item);
            delete item;
        }
    };

    transformPromptTree(ui->prompts_treeWidget, filter_prompt);
}

void MainWindow::on_selectPrompt_pushButton_clicked()
{
    for (QTreeWidgetItem* currentItem : ui->prompts_treeWidget->selectedItems()) {
        setSelectedStatus(currentItem, true);
    }
}
void MainWindow::on_deselectPrompt_pushButton_clicked()
{
    for (QTreeWidgetItem* currentItem : ui->prompts_treeWidget->selectedItems()) {
        setSelectedStatus(currentItem, false);
    }
}
void MainWindow::on_assignCID_pushButton_clicked()
{
    auto *dialog = new QInputDialog(this);
    dialog->setLabelText("CID:");
    dialog->setInputMode(QInputDialog::TextInput);
    dialog->setWindowTitle("Enter CID");
    dialog->adjustSize();

    auto *lineEdit = dialog->parent()->findChild<QLineEdit *>("");

    if (lineEdit != nullptr) {
        lineEdit->setCompleter(m_CIDCompleter);
    }

    const bool result = dialog->exec();
    const QString CID = dialog->textValue();

    if (result) {
        if (!m_CIDList.contains(CID)) {
            m_CIDList.push_back(CID);
            QStringListModel* model = dynamic_cast<QStringListModel*>(m_CIDCompleter->model());
            model->setStringList(m_CIDList);
        }
        const QList<QTreeWidgetItem*> selectedPrompts = ui->prompts_treeWidget->selectedItems();
        for (QTreeWidgetItem* prompt : selectedPrompts) {
            setCID(prompt, CID);
        }
    }

    delete dialog;
    //delete completer;
}
void MainWindow::on_clearCID_pushButton_clicked()
{
    const QList<QTreeWidgetItem*> selectedPrompts = ui->prompts_treeWidget->selectedItems();
    for (QTreeWidgetItem* prompt : selectedPrompts) {
        setCID(prompt, "");
    }
}
void MainWindow::on_delete_pushButton_clicked()
{
    for (QTreeWidgetItem* currentItem : ui->prompts_treeWidget->selectedItems()) {
        QTreeWidgetItem* currentParent = currentItem->parent(); // may be nullptr
        m_undoStack.push({ currentItem, currentParent });

        if (currentParent == nullptr) { // currentItem is top level dataset
            const int index = ui->prompts_treeWidget->indexOfTopLevelItem(currentItem);
            (void)ui->prompts_treeWidget->takeTopLevelItem(index);
            continue;
        }

        currentParent->removeChild(currentItem);
    }

    if (ui->prompts_treeWidget->topLevelItemCount() == 0) {
        ui->clear_pushButton->setEnabled(false);
    }

    ui->undo_pushButton->setEnabled(true);
}
void MainWindow::on_undo_pushButton_clicked()
{
    auto [item, parent] = m_undoStack.pop();
    if (parent == nullptr) {
        ui->prompts_treeWidget->addTopLevelItem(item);
        return;
    }

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

    if (parent == nullptr) {
        const int index = ui->prompts_treeWidget->indexOfTopLevelItem(item);
        (void)ui->prompts_treeWidget->takeTopLevelItem(index);
        return;
    }

    parent->removeChild(item);

    ui->undo_pushButton->setEnabled(true);

    if (m_redoStack.isEmpty()) {
        ui->redo_pushButton->setEnabled(false);
    }
}
void MainWindow::on_clear_pushButton_clicked()
{
    ui->prompts_treeWidget->clear();
    ui->prompt_plainTextEdit->clear();
    ui->delete_pushButton->setEnabled(false);
    ui->clear_pushButton->setEnabled(false);
    ui->selectPrompt_pushButton->setEnabled(false);
    ui->deselectPrompt_pushButton->setEnabled(false);
    ui->assignCID_pushButton->setEnabled(false);
    ui->clearCID_pushButton->setEnabled(false);
}

void MainWindow::on_filterPromptsByCID_pushButton_clicked()
{
    const QString cid = ui->filterPromptsByCID_lineEdit->text();

    const auto filter = [&](QTreeWidgetItem* item) -> void {
        if (!isPrompt(item)) {
            const int childCount = item->childCount();
            bool allChildrenHidden = true;
            for (int i : _range(0, childCount)) {
                if (!item->child(i)->isHidden()) {
                    allChildrenHidden = false;
                    break;
                }
            }
            if (allChildrenHidden) {
                item->setHidden(true);
            }
            return;
        }

        if (getCID(item) != cid) {
            item->setHidden(true);
        }
        else {
            item->setHidden(false);
        }
    };
    transformPromptTree(ui->prompts_treeWidget, filter);

    ui->prompts_treeWidget->expandAll();
}
void MainWindow::on_clearPromptFilter_pushButton_clicked()
{
    const auto showAll= [&](QTreeWidgetItem* item) -> void {
        item->setHidden(false);

    };
    transformPromptTree(ui->prompts_treeWidget, showAll);
}

void MainWindow::on_prompts_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current == nullptr) {
        return;
    }

    if (!isPrompt(current)) {
        ui->delete_pushButton->setEnabled(true);
        return;
    }

    ui->delete_pushButton->setEnabled(false);
    ui->prompt_plainTextEdit->clear();
    ui->references_plainTextEdit->clear();
    ui->prompt_plainTextEdit->insertPlainText(getPrompt(current));
    ui->references_plainTextEdit->insertPlainText(getReferences(current));
}

/**************************
 * Export to JSON widgets *
 **************************/

void MainWindow::on_exportOptions_pushButton_clicked()
{
    auto *options = new ExportOptionsDialog(m_outputPath,
                                            m_jsonFileName,
                                            m_compilationName,
                                            m_helmDataJSON);
    options->setAttribute(Qt::WA_DeleteOnClose);
    options->exec();
}
void MainWindow::on_export_pushButton_clicked()
{
    /*
     * This function is somewhat complex. Here's the layout:
     *
     * 1. Ensure that export pre-requisites are met
     * 2. Open file for export
     * 3. Load PNYX's `helm_tests.json` file, with HELM test data
     * 4. Construct JSON document for custom compilation
     * 5. Write to file and notify the user
     */

    /*******************************************
     * 1. Ensure export pre-requisites are met *
     *******************************************/

    if (ui->prompts_treeWidget->topLevelItemCount() == 0) {
        Warn("Nothing to export");
        return;
    }

    while (!exportPrerequisitesMet()) {
        const int result = launchExportOptionsDialog();
        // ExportOptionsDialog can be accepted only if all fields are filled
        if (result == QDialog::Rejected) {
            return;
        }
    }

    /***************************
     * 2. open file for export *
     ***************************/

    if (!QDir(m_outputPath).exists()) {
        QDir().mkdir(m_outputPath);
    }

    QFile outputFile(m_outputPath + "/" + m_jsonFileName + ".json");

    if (!outputFile.open(QIODevice::WriteOnly)) {
        Warn("Failed to open " + m_jsonFileName +  " for writing");
        return;
    }

    /***************************
     * 3. Load helm_tests.json *
     ***************************/

    QJsonObject const helmDataJson = loadHelmDataConfig(m_helmDataJSON);
    if (helmDataJson.isEmpty()) {
        Warn("Helm Dataset Configuration empty!\nAborting export.");
        return;
    }

    /********************************************
     * 4. Construct JSON for custom compilation *
     ********************************************/

    // TO-DO
    // check for incompatible metrics in custom dataset
    // notify CID change if found

    QJsonDocument customCompilation;

    /*
     * Output JSON structure:
     *
     * customCompilation
     *    +- mainObject
     *         +- compilationName
     *         +- datasetArray
     */

    QJsonObject mainObject;
    QJsonArray datasetArray;

    /*
     * datasetArray structure:
     *
     * datasetArray
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

    // construct the array

    const int topLevelDatasetCount = ui->prompts_treeWidget->topLevelItemCount();
    for (int i : _range(0, topLevelDatasetCount)) {
        const QTreeWidgetItem* dataset = ui->prompts_treeWidget->topLevelItem(i);

        if (!hasSpecifications(dataset)) {
            if (hasSelectedPrompts(dataset)) {
                datasetArray.append(generateCustomDataset(dataset, getName(dataset), {}, helmDataJson));
            }
            continue;
        }

        const int subDatasetCount = dataset->childCount();
        for (int i : _range(0, subDatasetCount)) {
            const QTreeWidgetItem* subDataset = dataset->child(i);
            if (hasSelectedPrompts(subDataset)) {
                datasetArray.append(generateCustomDataset(subDataset, getName(dataset), getName(subDataset), helmDataJson));
            }
        }
    }

    // put everything together

    mainObject.insert("compilation_name", m_compilationName);
    mainObject.insert("datasets", datasetArray);
    customCompilation.setObject(mainObject);

    /****************************************
     * 5. Write to file and notify the user *
     ****************************************/

    outputFile.write(customCompilation.toJson());

    PopUp("JSON exported");
}

bool MainWindow::exportPrerequisitesMet() const
{
    return !m_outputPath.isEmpty() && !m_jsonFileName.isEmpty() && !m_compilationName.isEmpty() && !m_helmDataJSON.isEmpty();
}
int MainWindow::launchExportOptionsDialog()
{
    auto *options = new ExportOptionsDialog(m_outputPath, m_jsonFileName, m_compilationName, m_helmDataJSON);
    options->setAttribute(Qt::WA_DeleteOnClose);
    return options->exec();
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
    settings.setValue("DontShowAgainSearch", m_DontShowEmptySearchMessage);
}
void MainWindow::readSettings()
{
    const QSettings settings(QSettings::IniFormat, QSettings::UserScope, "IIF-SADAF-CONICET", "HELMPromptBrowser");

    m_helmDataPath = settings.value("HELM_Path").toString();
    m_importFileFolder = settings.value("IMPORT_JSON_FOLDER").toString();
    m_outputPath = settings.value("Output_Path").toString();

    m_helmDataJSON = settings.value("HELM_JSON").toString();
    m_jsonFileName = settings.value("JSONFile").toString();

    m_compilationName = settings.value("CompilationName").toString();
    m_DontShowEmptySearchMessage = settings.value("DontShowAgainSearch").toBool();

    if (!QDir(m_importFileFolder).exists()) {
        m_importFileFolder = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
    }

    if (!QDir(m_helmDataPath).exists()) {
        m_helmDataPath.clear();
    }

    if (!QFile(m_helmDataJSON).exists()) {
        m_helmDataJSON.clear();
    }
}
