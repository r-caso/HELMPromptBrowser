#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <map>
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
                std::make_tuple(QString("babi_qa:task=15"), 70, HELMPromptBrowser::list_70),
                std::make_tuple(QString("babi_qa:task=19"), 70, HELMPromptBrowser::list_70),
                std::make_tuple(QString("babi_qa:task=3"), 70, HELMPromptBrowser::list_70),
                std::make_tuple(QString("babi_qa:task=all"), 70, HELMPromptBrowser::list_70),
            })
        },
        {
            "bbq", QList({
                std::make_tuple(QString("bbq:subject=all,method=multiple_choice_joint"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("bbq:subject=all,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("bbq:subject=all,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
            })
        },
        {
            "blimp", QList({
                std::make_tuple(QString("blimp:phenomenon=binding,method=multiple_choice_joint"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=binding,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=binding,method=multiple_choice_separate_original"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_joint"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=irregular_forms,method=multiple_choice_separate_original"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("blimp:phenomenon=island_effects,method=multiple_choice_joint"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=island_effects,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=island_effects,method=multiple_choice_separate_original"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("blimp:phenomenon=quantifiers,method=multiple_choice_joint"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=quantifiers,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("blimp:phenomenon=quantifiers,method=multiple_choice_separate_original"), 32, HELMPromptBrowser::list_32),
            })
        },
        {
            "bold:subject=all", QList({
                std::make_tuple(QString(""), 42, HELMPromptBrowser::list_42)
            })
        },
        {
            "boolq", QList({
                std::make_tuple(QString(""), 67, HELMPromptBrowser::list_67)
            })
        },
        {
            "boolq:only_contrast=True", QList({
                std::make_tuple(QString(""), 42, HELMPromptBrowser::list_42)
            })
        },
        {
            "civil_comments", QList({
                std::make_tuple(QString("civil_comments:demographic=LGBTQ"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=all"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=black"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=christian"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=female"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=male"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=muslim"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=other_religions"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("civil_comments:demographic=white"), 67, HELMPromptBrowser::list_67),
            })
        },
        {
            "code", QList({
                std::make_tuple(QString("code:dataset=apps"), 2, HELMPromptBrowser::list_2),
                std::make_tuple(QString("code:dataset=humaneval"), 2, HELMPromptBrowser::list_2)
            })
        },
        {
            "commonsense", QList({
                std::make_tuple(QString("commonsense:dataset=hellaswag,method=multiple_choice_joint"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("commonsense:dataset=hellaswag,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("commonsense:dataset=hellaswag,method=multiple_choice_separate_original"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("commonsense:dataset=openbookqa,method=multiple_choice_joint"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("commonsense:dataset=openbookqa,method=multiple_choice_separate_calibrated"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("commonsense:dataset=openbookqa,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
            })
        },
        {
            "copyright", QList({
                std::make_tuple(QString("copyright:datatag=n_books_1000-extractions_per_book_1-prefix_length_125"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("copyright:datatag=oh_the_places"), 10, HELMPromptBrowser::list_10),
                std::make_tuple(QString("copyright:datatag=popular_books-prefix_length_125.json"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("copyright:datatag=prompt_num_line_1-min_lines_20.json"), 2, HELMPromptBrowser::list_2),
                std::make_tuple(QString("copyright:datatag=prompt_num_line_10-min_lines_20.json"), 2, HELMPromptBrowser::list_2),
            })
        },
        {
            "disinfo", QList({
                std::make_tuple(QString("disinfo:type=reiteration,topic=climate"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("disinfo:type=reiteration,topic=covid"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("disinfo:type=wedging"), 42, HELMPromptBrowser::list_42),
            })
        },
        {
            "dyck_language_np=3", QList({
                std::make_tuple(QString(""), 69, HELMPromptBrowser::list_69)
            })
        },
        {
            "entity_data_imputation", QList({
                std::make_tuple(QString("entity_data_imputation:dataset=Buy"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("entity_data_imputation:dataset=Restaurant"), 69, HELMPromptBrowser::list_69),
            })
        },
        {
            "entity_matching", QList({
                std::make_tuple(QString("entity_matching:dataset=Abt_Buy"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("entity_matching:dataset=Beer"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("entity_matching:dataset=Dirty_iTunes_Amazon"), 69, HELMPromptBrowser::list_69),
            })
        },
        {
            "gsm", QList({
                std::make_tuple(QString(""), 69, HELMPromptBrowser::list_69)
            })
        },
        {
            "ice", QList({
                std::make_tuple(QString("ice:gender=female"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("ice:gender=male"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("ice:subset=ea"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("ice:subset=hk"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("ice:subset=ind"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("ice:subset=usa"), 32, HELMPromptBrowser::list_32),
            })
        },
        {
            "imdb", QList({
                std::make_tuple(QString(""), 67, HELMPromptBrowser::list_67)
            })
        },
        {
            "imdb:only_contrast=True", QList({
                std::make_tuple(QString(""),42, HELMPromptBrowser::list_42)
            })
        },
        {
            "legal_support", QList({
                std::make_tuple(QString("legal_support,method=multiple_choice_joint"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("legal_support,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("legal_support,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
            })
        },
        {
            "lsat_qa", QList({
                std::make_tuple(QString("lsat_qa:task=all,method=multiple_choice_joint"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("lsat_qa:task=all,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("lsat_qa:task=all,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
            })
        },
        {
            "math", QList({
                std::make_tuple(QString("math:subject=algebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=algebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=counting_and_probability,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=counting_and_probability,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=geometry,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=geometry,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=intermediate_algebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=intermediate_algebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=number_theory,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=number_theory,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=prealgebra,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=prealgebra,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=precalculus,level=1,use_official_examples=False,use_chain_of_thought=True"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("math:subject=precalculus,level=1,use_official_examples=True,use_chain_of_thought=False"), 69, HELMPromptBrowser::list_69),
            })
        },
        {
            "mmlu", QList({
                std::make_tuple(QString("mmlu:subject=abstract_algebra,method=multiple_choice_joint"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("mmlu:subject=abstract_algebra,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=abstract_algebra,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=college_chemistry,method=multiple_choice_joint"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("mmlu:subject=college_chemistry,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=college_chemistry,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=computer_security,method=multiple_choice_joint"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("mmlu:subject=computer_security,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=computer_security,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=econometrics,method=multiple_choice_joint"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("mmlu:subject=econometrics,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=econometrics,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_joint"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("mmlu:subject=us_foreign_policy,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
            })
        },
        {
            "msmarco", QList({
                std::make_tuple(QString("msmarco:track=regular,valid_topk=30"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("msmarco:track=trec,valid_topk=30"), 32, HELMPromptBrowser::list_32),
            })
        },
        {
            "narrative_qa", QList({
                std::make_tuple(QString(""), 66, HELMPromptBrowser::list_66b)
            })
        },
        {
            "natural_qa", QList({
                std::make_tuple(QString("natural_qa:mode=closedbook"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("natural_qa:mode=openbook_longans"), 66, HELMPromptBrowser::list_66a),
            })
        },
        {
            "quac", QList({
                std::make_tuple(QString(""), 66, HELMPromptBrowser::list_66b)
            })
        },
        {
            "raft", QList({
                std::make_tuple(QString("raft:subset=ade_corpus_v2"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=banking_77"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=neurips_impact_statement_risks"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=one_stop_english"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=overruling"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=semiconductor_org_types"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=systematic_review_inclusion"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=tai_safety_research"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=terms_of_service"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=tweet_eval_hate"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("raft:subset=twitter_complaints"), 67, HELMPromptBrowser::list_67),
            })
        },
        {
            "real_toxicity_prompts", QList({
                std::make_tuple(QString(""), 42, HELMPromptBrowser::list_42)
            })
        },
        {
            "summarization_cnndm", QList({
                std::make_tuple(QString("summarization_cnndm:temperature=0.3,device=cpu"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("summarization_cnndm:temperature=0.3,device=cuda"), 40, HELMPromptBrowser::list_40),
            })
        },
        {
            "summarization_xsum", QList({
                std::make_tuple(QString("summarization_xsum:temperature=0.3,device=cpu"), 42, HELMPromptBrowser::list_42),
                std::make_tuple(QString("summarization_xsum:temperature=0.3,device=cuda"), 40, HELMPromptBrowser::list_40),
            })
        },
        {
            "synthetic_efficiency:random=None", QList({
             std::make_tuple(QString(""), 39, HELMPromptBrowser::list_39)
            })
        },
        {
            "synthetic_reasoning", QList({
                std::make_tuple(QString("synthetic_reasoning:mode=induction"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("synthetic_reasoning:mode=pattern_match"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("synthetic_reasoning:mode=variable_substitution"), 69, HELMPromptBrowser::list_69),
            })
        },
        {
            "synthetic_reasoning_natural", QList({
                std::make_tuple(QString("synthetic_reasoning_natural:difficulty=easy"), 69, HELMPromptBrowser::list_69),
                std::make_tuple(QString("synthetic_reasoning_natural:difficulty=hard"), 69, HELMPromptBrowser::list_69),
            })
        },
        {
            "the_pile", QList({
                std::make_tuple(QString("the_pile:subset=ArXiv"), 24, HELMPromptBrowser::list_24),
                std::make_tuple(QString("the_pile:subset=BookCorpus2"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("the_pile:subset=Enron Emails"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("the_pile:subset=Github"), 24, HELMPromptBrowser::list_24),
                std::make_tuple(QString("the_pile:subset=PubMed Central"), 24, HELMPromptBrowser::list_24),
                std::make_tuple(QString("the_pile:subset=Wikipedia (en)"), 32, HELMPromptBrowser::list_32),
            })
        },
        {
            "truthful_qa", QList({
                std::make_tuple(QString("truthful_qa:task=mc_single,method=multiple_choice_joint"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("truthful_qa:task=mc_single,method=multiple_choice_separate_calibrated"), 6, HELMPromptBrowser::list_6),
                std::make_tuple(QString("truthful_qa:task=mc_single,method=multiple_choice_separate_original"), 6, HELMPromptBrowser::list_6),
            })
        },
        {
            "twitter_aae", QList({
                std::make_tuple(QString("twitter_aae:demographic=aa"), 32, HELMPromptBrowser::list_32),
                std::make_tuple(QString("twitter_aae:demographic=white"), 32, HELMPromptBrowser::list_32),
            })
        },
        {
            "wikifact", QList({
                std::make_tuple(QString("wikifact:k=5,subject=author"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=currency"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=discoverer_or_inventor"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=instance_of"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=medical_condition_treated"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=part_of"), 66, HELMPromptBrowser::list_66b),
                std::make_tuple(QString("wikifact:k=5,subject=place_of_birth"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=plaintiff"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=position_held"), 67, HELMPromptBrowser::list_67),
                std::make_tuple(QString("wikifact:k=5,subject=symptoms_and_signs"), 67, HELMPromptBrowser::list_67),
            })
        }
    };

    ui->dataset_treeWidget->setColumnCount(HELMPromptBrowser::DTColumnCount);

    for (int i = HELMPromptBrowser::DTNumberOfModels; i < HELMPromptBrowser::DTColumnCount; ++i) {
        ui->dataset_treeWidget->hideColumn(i);
    }

    for (const auto& [task, vector] : HELMDatasetHierarchy) {
        auto *item = new QTreeWidgetItem();
        item->setCheckState(HELMPromptBrowser::DTDatasetNameColumn, Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsAutoTristate);
        item->setData(HELMPromptBrowser::DTDatasetNameColumn, Qt::DisplayRole, task);
        for (const auto& [subTask, numberOfModels, modelList]  : vector) {
            if (subTask.isEmpty()) {
                item->setData(HELMPromptBrowser::DTNumberOfModels, Qt::DisplayRole, numberOfModels);
                continue;
            }
            auto *child = new QTreeWidgetItem();
            child->setCheckState(HELMPromptBrowser::DTDatasetNameColumn, Qt::Unchecked);
            child->setData(HELMPromptBrowser::DTDatasetNameColumn, Qt::DisplayRole, subTask);
            child->setData(HELMPromptBrowser::DTNumberOfModels, Qt::DisplayRole, numberOfModels);
            child->setData(HELMPromptBrowser::DTLMListColumn, Qt::DisplayRole, QVariant::fromValue<QList<int>>(modelList));
            item->addChild(child);
        }
        ui->dataset_treeWidget->addTopLevelItem(item);
    }

    /**********************
     * Set up prompt tree *
     **********************/

    ui->prompts_treeWidget->setColumnCount(HELMPromptBrowser::PTColumnCount);
    ui->prompts_treeWidget->setHeaderLabels({"Custom Dataset ID", "Dataset name / Prompt ID"});
    for (int i = HELMPromptBrowser::PTDatasetBaseColumn; i < HELMPromptBrowser::PTColumnCount; ++i) {
        ui->prompts_treeWidget->hideColumn(i);
    }
    ui->prompts_treeWidget->setColumnWidth(HELMPromptBrowser::PTCIDColumn, 120);
    ui->prompts_treeWidget->header()->setStretchLastSection(true);
    ui->prompts_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    /*********************
     * Editing shortcuts *
     *********************/

    auto *deleteShortcut1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->prompts_treeWidget);
    connect(deleteShortcut1, SIGNAL(activated()), this, SLOT(on_delete_pushButton_clicked()));
    auto *undoShortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z),
                                          ui->prompts_treeWidget);
    connect(undoShortcut1, SIGNAL(activated()), this, SLOT(on_undo_pushButton_clicked()));
    auto *redoShortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y),
                                          ui->prompts_treeWidget);
    connect(redoShortcut1, SIGNAL(activated()), this, SLOT(on_redo_pushButton_clicked()));
    auto *redoShortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z),
                                          ui->prompts_treeWidget);
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

    const size_t taskDirscount = taskDirs.count();
    for (size_t j = 0; j < taskDirscount; ++j) {
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

void MainWindow::on_HELM_Data_pushButton_clicked()
{
    const QString path = QFileDialog::getExistingDirectory(this, "Select HELM data folder", QStandardPaths::displayName(QStandardPaths::DocumentsLocation));
    m_helmDataPath = path.isEmpty() ? m_helmDataPath : path;
    ui->HELM_Data_lineEdit->setText(m_helmDataPath);
}

void MainWindow::on_deselect_all_pushButton_clicked()
{
    for (int i = 0; i < ui->dataset_treeWidget->topLevelItemCount(); ++i) {
        ui->dataset_treeWidget->topLevelItem(i)->setCheckState(HELMPromptBrowser::DTDatasetNameColumn, Qt::Unchecked);
    }
}

void MainWindow::on_select_all_pushButton_clicked()
{
    for (int i = 0; i < ui->dataset_treeWidget->topLevelItemCount(); ++i) {
        ui->dataset_treeWidget->topLevelItem(i)->setCheckState(HELMPromptBrowser::DTDatasetNameColumn, Qt::Checked);
    }
}

void MainWindow::on_loadFromFile_pushButton_clicked()
{
    QString const fromFile = QFileDialog::getOpenFileName(
        this,
        "Select custom compilation file",
        m_importFileFolder.isEmpty() ? QStandardPaths::displayName(QStandardPaths::DocumentsLocation)
                                     : m_importFileFolder,
        "*.json"
    );

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

    const int datasetCount = ui->dataset_treeWidget->topLevelItemCount();
    for (int i = 0; i < datasetCount; ++i) {
        ui->dataset_treeWidget->topLevelItem(i)->setCheckState(HELMPromptBrowser::DTDatasetNameColumn, Qt::Unchecked);
    }

    const QJsonDocument customDataset = QJsonDocument::fromJson(jsonFile.readAll());
    const QJsonArray datasets = customDataset["datasets"].toArray();

    QMap<QString, QStringList> tree;
    QList<std::tuple<QString, QString, QList<QPair<QString, QString>>>> selectedPrompts;

    for (auto&& value : datasets) {
        const QJsonObject dataset = value.toObject();
        const QString datasetName = dataset.keys().at(0);
        const QString datasetSpec = dataset[datasetName]["dataset_spec"].toString();

        if (!tree.contains(datasetName)) {
            tree[datasetName] = {};
        }
        if (datasetSpec.isEmpty()) {
            continue;
        }
        if (datasetName == "legal_support") {
            tree[datasetName].push_back(datasetName + "," + datasetSpec);
        }
        else {
            tree[datasetName].push_back(datasetName + ":" + datasetSpec);
        }

        const QJsonObject samples = dataset[datasetName]["samples"].toObject();
        const QStringList promptIds = samples.keys();
        QList<QPair<QString, QString>> selected;
        for (const QString& promptId : promptIds) {
            const QString& cid = samples[promptId].toString();
            selected.push_back({promptId, cid});
        }
        selectedPrompts.push_back({datasetName, datasetSpec, selected});
    }

    for (auto topLevelDataset = tree.cbegin(); topLevelDataset != tree.cend(); ++topLevelDataset) {
        QList<QTreeWidgetItem *> const matches
            = ui->dataset_treeWidget->findItems(topLevelDataset.key(), Qt::MatchExactly, HELMPromptBrowser::DTDatasetNameColumn);
        const QStringList& subDatasets = topLevelDataset.value();

        if (subDatasets.isEmpty()) {
            for (QTreeWidgetItem* item: matches) {
                item->setCheckState(0, Qt::Checked);
            }
        }
        else {
            for (QTreeWidgetItem* item: matches) {
                const size_t childCount = item->childCount();
                for (int i = 0; i < childCount; ++i) {
                    QTreeWidgetItem* child = item->child(i);
                    const QString childName = child->data(HELMPromptBrowser::DTDatasetNameColumn, Qt::DisplayRole).toString();
                    if (subDatasets.contains(childName)) {
                        child->setCheckState(HELMPromptBrowser::DTDatasetNameColumn, Qt::Checked);
                    }
                }
            }
        }
    }

    /******************************
     * CORE PROMPT ADDITION LOGIC *
     ******************************/

    const QStringList datasetsToBeAdded = getSelectedDatasetNames(ui->dataset_treeWidget);
    const QStringList taskDirs = getHelmTaskDirs(datasetsToBeAdded, m_helmDataPath);
    Q_ASSERT_X(taskDirs.size() == datasetsToBeAdded.size(), "Taks directories and selected datasets have different cardinalities", "mainwindow.cpp");
    const QList<QPair<QList<QString>, QList<QString>>> queries = {{},{}};
    const bool searchIsCaseSensitive = false;
    const bool searchIsRegex = false;
    const size_t taskDirscount = taskDirs.count();

    for (size_t j = 0; j < taskDirscount; ++j) {
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
    }

    /***********************************************
     * SELECT PROMPTS AND STORE CIDs FOR COMPLETER *
     ***********************************************/

    const auto isSelected = [&](QTreeWidgetItem* item) -> void {
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

        for (const auto& tuple : selectedPrompts) {
            if (std::get<0>(tuple) == datasetBase && std::get<1>(tuple) == datasetSpec) {
                auto pair = std::ranges::find_if(std::get<2>(tuple), [&](const QPair<QString, QString>& p) { return p.first == promptId; });
                if (pair != std::get<2>(tuple).cend()) {
                    promptCId = pair->second;
                    setCID(item, promptCId);
                    setSelectedStatus(item, true);
                    if (!m_CIDList.contains(promptCId)) {
                        m_CIDList.push_back(promptCId);
                    }
                }
            }
        }
    };

    m_CIDList.clear();
    transformPromptTree(ui->prompts_treeWidget, isSelected);
    QStringListModel* model = dynamic_cast<QStringListModel*>(m_CIDCompleter->model());
    model->setStringList(m_CIDList);

    ui->delete_pushButton->setEnabled(true);
    ui->clear_pushButton->setEnabled(true);
    ui->selectPrompt_pushButton->setEnabled(true);
    ui->deselectPrompt_pushButton->setEnabled(true);
    ui->assignCID_pushButton->setEnabled(true);
    ui->clearCID_pushButton->setEnabled(true);
}


void MainWindow::on_export_pushButton_clicked()
{
    if (ui->prompts_treeWidget->topLevelItemCount() == 0) {
        Warn("Nothing to export");
        return;
    }

    // check for incompatible metrics in custom dataset
    // notify CID change is found

    while (m_outputPath.isEmpty() || m_jsonFileName.isEmpty() || m_compilationName.isEmpty() || m_helmDataJSON.isEmpty()) {
        auto *options = new ExportOptionsDialog(m_outputPath,
                                                m_jsonFileName,
                                                m_compilationName,
                                                m_helmDataJSON);
        options->setAttribute(Qt::WA_DeleteOnClose);
        int const result = options->exec();
        if (result == QDialog::Rejected) {
            return;
        }
    }

    if (!QDir(m_outputPath).exists()) {
        QDir().mkdir(m_outputPath);
    }

    QFile outputFile(m_outputPath + "/" + m_jsonFileName + ".json");

    if (!outputFile.open(QIODevice::WriteOnly)) {
        Warn("Failed to open " + m_jsonFileName +  " for writing");
        return;
    }
    
    
    // Load helm_tests.json
    
    QJsonObject const helmDataJson = loadHelmDataConfig(m_helmDataJSON);
    if (helmDataJson.isEmpty()) {
        Warn("Helm Dataset Configuration empty!\nAborting export.");
        return;
    }

    /*
     * JSON structure:
     *
     * customCompilation
     *    +- mainObject
     *         +- compilationName
     *         +- datasetArray
     */

    QJsonDocument customCompilation;
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

    QMap<QPair<QString, QString>, QStringList> datasetMetricMap;

    const size_t topLevelDatasetCount = ui->prompts_treeWidget->topLevelItemCount();
    for (size_t i = 0; i < topLevelDatasetCount; ++i) {
        QTreeWidgetItem* topLevelDataset = ui->prompts_treeWidget->topLevelItem(i);

        if (!hasSpecifications(topLevelDataset)) {
            if (!hasSelectedPrompts(topLevelDataset)) {
                continue;
            }
            const QString datasetBase = getName(topLevelDataset);
            const QString datasetSpec = {};

            QPair<QString, QString> dsPair{datasetBase, datasetSpec};
            if (!datasetMetricMap.contains(dsPair)) {
                datasetMetricMap[dsPair] = {};
            }

            const QJsonObject customDataset = getDatasetObj(topLevelDataset, datasetBase, datasetSpec, helmDataJson);
            datasetArray.append(customDataset);

            const QString metric = customDataset[datasetBase]["metric"].toString();

            if (!datasetMetricMap[dsPair].contains(metric)) {
                datasetMetricMap[dsPair].push_back(metric);
            }

            continue;
        }

        const size_t subDatasetCount = topLevelDataset->childCount();
        for (size_t i = 0; i < subDatasetCount; ++i) {
            const QTreeWidgetItem* subDataset = topLevelDataset->child(i);
            if (!hasSelectedPrompts(subDataset)) {
                continue;
            }

            const QString dataset_base = getName(topLevelDataset);
            const QString dataset_spec = getName(subDataset);

            QPair<QString, QString> dsPair{dataset_base, dataset_spec};
            if (!datasetMetricMap.contains(dsPair)) {
                datasetMetricMap[dsPair] = {};
            }

            const QJsonObject customDataset = getDatasetObj(subDataset, dataset_base, dataset_spec, helmDataJson);
            datasetArray.append(customDataset);

            const QString metric = customDataset[dataset_base]["metric"].toString();

            if (!datasetMetricMap[dsPair].contains(metric)) {
                datasetMetricMap[dsPair].push_back(metric);
            }
        }
    }

    mainObject.insert("compilationName", m_compilationName);
    mainObject.insert("datasets", datasetArray);
    customCompilation.setObject(mainObject);
    outputFile.write(customCompilation.toJson());

    PopUp("JSON exported");
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


void MainWindow::on_exportOptions_pushButton_clicked()
{
    auto *options = new ExportOptionsDialog(m_outputPath,
                                            m_jsonFileName,
                                            m_compilationName,
                                            m_helmDataJSON);
    options->setAttribute(Qt::WA_DeleteOnClose);
    options->exec();
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
    const QString prompt = getPrompt(current);
    ui->prompt_plainTextEdit->insertPlainText(prompt);
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
        if (item == nullptr) {
            return;
        }

        if (item->isHidden()) {
            return;
        }

        const bool has_enough_models = item->data(HELMPromptBrowser::DTNumberOfModels, Qt::DisplayRole).toInt() >= numberOfModels;
        bool parameters_in_range = false;
        bool tested_on_vendor = false;

        const QList<QVariant>& model_id_list_variant = item->data(HELMPromptBrowser::DTLMListColumn, Qt::DisplayRole).toList();
        for (const auto& model_id_variant : model_id_list_variant) {
            int model_id = model_id_variant.toInt();
            const auto model = std::ranges::find_if(m_Models, [&](const HELMPromptBrowser::LanguageModel& model) { return model.id() == model_id; });
            if ((m_VendorFilterList.isEmpty() || m_VendorFilterList.contains(static_cast<int>(model->vendor()))) && model->parameters() >= min && model->parameters() < max) {
                parameters_in_range = true;
                tested_on_vendor = true;
                break;
            }
        }

        if (!has_enough_models || !(parameters_in_range && tested_on_vendor)) {
            item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
            item->setHidden(true);

            if (item->parent() != nullptr) {
                QTreeWidgetItem* parent = item->parent();
                const size_t childCount = parent->childCount();
                bool all_children_disabled = true;
                for (size_t i = 0; i < childCount; ++i) {
                    if (!parent->child(i)->isHidden()) {
                        all_children_disabled = false;
                    }
                }
                if (all_children_disabled) {
                    parent->setFlags(parent->flags() & (~Qt::ItemIsUserCheckable));
                    parent->setHidden(true);
                }
            }
        }
    };

    transformDatasetTree(ui->dataset_treeWidget, filter);
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


void MainWindow::on_clearCID_pushButton_clicked()
{
    const QList<QTreeWidgetItem*> selectedPrompts = ui->prompts_treeWidget->selectedItems();
    for (QTreeWidgetItem* prompt : selectedPrompts) {
        setCID(prompt, "");
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

void MainWindow::on_filterPromptsByCID_pushButton_clicked()
{
    const QString cid = ui->filterPromptsByCID_lineEdit->text();

    const auto filter = [&](QTreeWidgetItem* item) -> void {
        if (!isPrompt(item)) {
            const size_t childCount = item->childCount();
            bool allChildrenHidden = true;
            for (size_t i = 0; i < childCount; ++i) {
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


void MainWindow::on_filterByVendor_pushButton_clicked()
{
    VendorDialog* dialog = new VendorDialog(m_VendorFilterList);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

