#include "helperfunctions.hpp"

#include <QCheckBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QSysInfo>
#include <QTimer>
#include <QTreeWidgetItem>

int Ask(const QString& text, const QString& informative_text, bool& dont_show_again) {
    QMessageBox msg;
    msg.setText(text);
    msg.setInformativeText(informative_text);
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QCheckBox* cb = new QCheckBox();
    cb->setCheckState(Qt::Unchecked);
    cb->setText("Don't show this again");
    msg.setCheckBox(cb);
    const int result = msg.exec();
    dont_show_again = cb->checkState();
    return result;
}

void Warn(const QString& message)
{
    QMessageBox msg;
    msg.setText(message);
    msg.setIcon(QMessageBox::Warning);
    msg.exec();
}

QString prettyPrint(const QJsonObject& obj, const QString& dataset)
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
        perturbed += "PERTURBATION: prompt is perturbed";
    }

    QString str = "DATASET: " + dataset + "\n" +
                  "PROMPT ID: " + prompt_id + "\n\n" +
                  input_text +
                  references_text +
                  subsplit +
                  perturbed;

    return str.trimmed();
}

QStringList getSelectedDatasetNames(const QTreeWidget* tree)
{
    QStringList selected_datasets;

    const int top_level_dataset_count = tree->topLevelItemCount();

    // process top level datasets
    for (int i = 0; i < top_level_dataset_count; ++i) {
        QTreeWidgetItem* parent = tree->topLevelItem(i);
        QString parent_name = parent->data(0, Qt::DisplayRole).toString();

        if (parent->checkState(0) == Qt::Unchecked) {
            continue;
        }

        // if dataset has no sub-datasets, do nothing if already in tree, add it if not
        if (parent->childCount() == 0) {
            selected_datasets.push_back(parent_name);
            continue;
        }

        // dataset has sub-datasets
        // process each sub-dataset
        for (int j = 0; j < parent->childCount(); ++j) {
            QTreeWidgetItem* child = parent->child(j);
            QString child_name = child->data(0, Qt::DisplayRole).toString();

            // if sub-dataset is unchecked, delete it from prompt tree if present, ignore it if not
            if (child->checkState(0) == Qt::Unchecked) {
                continue;
            }

            // add it if not in tree
            selected_datasets.push_back(child_name);
        }
    }
    return selected_datasets;
}

QStringList getFiltersFromDatasetList(const QStringList& dataset_names)
{
    const QString operating_system = QSysInfo::productType();
    QStringList filters;
    for (QString dataset : dataset_names) {
        filters.push_back(operating_system == "windows" ? dataset.replace(":", "_") + "*" : dataset + "*");
    }
    return filters;
}

void deleteDatasetFromTree(const QString& dataset_name, QTreeWidget* tree)
{
    auto [dataset_base, dataset_spec] = splitDatasetName(dataset_name);

    if (dataset_spec.isEmpty()) {
        QList<QTreeWidgetItem*> deletion_list = tree->findItems(dataset_base, Qt::MatchExactly, 1);
        for (QTreeWidgetItem* dataset : deletion_list) {
            delete dataset;
        }
    }
    else {
        QList<QTreeWidgetItem*> parent_list = tree->findItems(dataset_base, Qt::MatchExactly, 1);
        for (QTreeWidgetItem* parent : parent_list) {
            QTreeWidgetItem* matching_child;
            const int child_count = parent->childCount();
            for (int i = 0; i < child_count; ++i) {
                if (getName(parent->child(i)) == dataset_spec) {
                    matching_child = parent->child(i);
                }
            }
            parent->removeChild(matching_child);
            delete matching_child;
            if (parent->childCount() == 0) {
                delete parent;
            }
        }
    }
}

QStringList getHelmTaskDirs(const QStringList& datasets, const QString& helm_data_path)
{
    const QStringList filters = getFiltersFromDatasetList(datasets);

    QStringList task_dirs;

    QDir helm_dir(helm_data_path);
    for (const auto& filter : filters) {
        helm_dir.setNameFilters({ filter });
        task_dirs.push_back(helm_dir.entryList().at(0));
    }

    return task_dirs;
}


QString getCID(const QTreeWidgetItem* item)
{
    return item->data(0, Qt::DisplayRole).toString();
}

void setCID(QTreeWidgetItem* item, const QString& cid)
{
    item->setData(0, Qt::DisplayRole, cid);
}

QString getPID(const QTreeWidgetItem* item)
{
    return item->data(1, Qt::DisplayRole).toString();
}

QString getName(const QTreeWidgetItem* item)
{
    return item->data(1, Qt::DisplayRole).toString();
}

QString getDatasetBase(const QTreeWidgetItem* item)
{
    return item->data(2, Qt::DisplayRole).toString();
}

QString getDatasetSpec(const QTreeWidgetItem* item)
{
    return item->data(3, Qt::DisplayRole).toString();
}

bool isPrompt(const QTreeWidgetItem* item)
{
    return item->data(4, Qt::DisplayRole).toBool();
}

QString getPrompt(const QTreeWidgetItem* item)
{
    return item->data(5, Qt::DisplayRole).toString();
}

bool hasSpecifications(const QTreeWidgetItem* item)
{
    return item->data(6, Qt::DisplayRole).toBool();
}

bool isSelected(const QTreeWidgetItem* item)
{
    return item->data(7, Qt::DisplayRole).toBool();
}

void setSelectedStatus(QTreeWidgetItem* item, bool status)
{
    item->setData(7, Qt::DisplayRole, status);
    if (status) {
        item->setForeground(1, Qt::black);
        item->setBackground(0, Qt::blue);
        item->setForeground(0, Qt::white);
    }
    else {
        item->setForeground(1, Qt::darkGray);
        item->setBackground(0, Qt::lightGray);
        item->setForeground(0, Qt::black);
    }
}

QJsonObject getSamples(const QTreeWidgetItem* item)
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

QJsonObject getDatasetObj(const QTreeWidgetItem* item, const QString& dataset_base, const QString& dataset_spec, const QJsonObject& helm_data_json)
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

QJsonObject loadHelmDataConfig(const QString& helm_data_json)
{
    QFile helm_data_json_file(helm_data_json);
    if (!helm_data_json_file.open(QIODevice::ReadOnly)) {
        return {};
    }
    const QJsonDocument helm_dataset_config = QJsonDocument::fromJson(helm_data_json_file.readAll());

    return helm_dataset_config.toVariant().toJsonObject();
}

QJsonDocument getTaskInstances(const QString& task_dir, const QString& helm_data_path)
{
    QFile instances_file(helm_data_path + "/" + task_dir + "/instances.json");
    if (!instances_file.open(QIODevice::ReadOnly)) {
        QMessageBox msg;
        msg.setText("Failed to open instances.json from " + task_dir);
        msg.exec();
        return {};
    }
    return QJsonDocument::fromJson(instances_file.readAll());
}

QPair<QString, QString> splitDatasetName(const QString& dataset)
{
    QString dataset_base;
    QString dataset_spec;

    if (dataset == "bold:subject=all" || dataset == "boolq:only_contrast=True" || dataset == "imdb:only_contrast=True")
    {
        dataset_base = dataset;
        dataset_spec = {};
    }
    else if (dataset.contains("legal_support")) {
        dataset_base = dataset.split(",").at(0);
        dataset_spec = dataset.split(",").at(1);
    }
    else if (dataset.contains(":")) {
        dataset_base = dataset.split(":").at(0);
        dataset_spec = dataset.split(":").at(1);
    }
    else {
        dataset_base = dataset;
        dataset_spec = {};
    }

    return {dataset_base, dataset_spec};
}

void addPromptsToTree(const QString& dataset,
                      const QJsonDocument& instances,
                      const QList<QPair<QStringList, QStringList>>& queries,
                      bool search_is_case_sensitive,
                      QTreeWidget* tree)
{
    // if prompt in prompt tree, skip adding
    auto [dataset_base, dataset_spec] = splitDatasetName(dataset);

    QList<QTreeWidgetItem*> base_item_match = tree->findItems(dataset_base, Qt::MatchExactly, 1);
    QList<QTreeWidgetItem*> spec_item_match;

    if (!dataset_spec.isEmpty()) {
        spec_item_match = tree->findItems(dataset_spec, Qt::MatchExactly | Qt::MatchRecursive, 1);
    }

    QTreeWidgetItem* base_item = nullptr;

    if (!base_item_match.empty()) {
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
        if (!spec_item_match.empty()) {
            spec_item = spec_item_match.at(0);
        }
        else {
            spec_item = new QTreeWidgetItem();
            spec_item->setData(1, Qt::DisplayRole, dataset_spec);
            spec_item->setData(4, Qt::DisplayRole, false);
            spec_item->setData(6, Qt::DisplayRole, false);
        }
    }

    QTreeWidgetItem* parent = spec_item != nullptr ? spec_item : base_item;

    const int instance_count = instances.array().count();
    for (int i = 0; i < instance_count; ++i) {
        const QJsonObject obj = instances.array().at(i).toObject();
        const QString prompt = obj["input"].toObject()["text"].toString();

        bool match = matches(prompt, queries, search_is_case_sensitive);

        if (!match) {
            continue;
        }

        const QString prompt_id = obj["id"].toString();

        bool prompt_is_in_tree = false;
        const size_t number_of_prompts = parent->childCount();
        for (int j = 0; j < number_of_prompts; ++j) {
            QTreeWidgetItem* prompt = parent->child(j);
            if (getPID(prompt) == prompt_id) {
                prompt_is_in_tree = true;
                break;
            }
        }
        if (prompt_is_in_tree) {
            continue;
        }

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
    tree->addTopLevelItem(base_item);
}

void PopUp(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::NoButton);
    QTimer::singleShot(1250, &msgBox, &QMessageBox::accept);
    msgBox.exec();
}

// Delete if responsiblity for dataset deletion is handled to delete button exclusively
QStringList getDatasetsToAdd(QTreeWidget* source, QTreeWidget* destination, QStringList& previous_selection)
{
    QStringList selected_datasets = getSelectedDatasetNames(source);

    /*
    QStringList to_be_unregistered;
    for (const QString& current_dataset : previous_selection) {
        if (!selected_datasets.contains(current_dataset)) {
            deleteDatasetFromTree(current_dataset, destination);
            to_be_unregistered.push_back(current_dataset);
        }
    }
    for (const QString& dataset : to_be_unregistered) {
        previous_selection.removeAll(dataset);
    }
    */

    /*QStringList datasets_to_be_added;
    std::ranges::copy_if(selected_datasets, std::back_inserter(datasets_to_be_added),
                         [&](const QString& dataset) -> bool { return !previous_selection.contains(dataset); });

    return datasets_to_be_added;*/
    return selected_datasets;
}


void transformPromptTree(QTreeWidget* prompt_tree, std::function<void(QTreeWidgetItem*)> transformation)
{
    const size_t dataset_count = prompt_tree->topLevelItemCount();
    for (size_t i = 0; i < dataset_count; ++i) {
        QTreeWidgetItem* dataset = prompt_tree->topLevelItem(i);
        if (!hasSpecifications(dataset)) {
            const size_t prompt_count = dataset->childCount();
            for (size_t j = 0; j < prompt_count; ++j) {
                QTreeWidgetItem* prompt = dataset->child(j);
                transformation(prompt);
            }
        }
        else {
            const size_t subdataset_count = dataset->childCount();
            for (size_t j = 0; j < subdataset_count; ++j) {
                QTreeWidgetItem* subdataset = dataset->child(j);
                const size_t prompt_count = subdataset->childCount();
                for (size_t k = 0; k < prompt_count; ++k) {
                    QTreeWidgetItem* prompt = subdataset->child(k);
                    transformation(prompt);
                }
            }
        }
    }
}

void transformDatasetTree(QTreeWidget* dataset_tree, std::function<void(QTreeWidgetItem*)> transformation)
{
    const size_t dataset_count = dataset_tree->topLevelItemCount();
    for (size_t i = 0; i < dataset_count; ++i) {
        QTreeWidgetItem* dataset = dataset_tree->topLevelItem(i);
        if (dataset->childCount() == 0) {
            transformation(dataset);
            continue;
        }
        const size_t specification_count = dataset->childCount();
        for (size_t j = 0; j < specification_count; ++j) {
            transformation(dataset->child(j));
        }
    }
}

bool matches(const QString& prompt, const QList<QPair<QStringList, QStringList>>& queries, const bool case_sensitivity) {
    const auto prompt_matches_term = [&](const QString& term) { return prompt.contains(term, (case_sensitivity ? Qt::CaseSensitive : Qt::CaseInsensitive));};
    bool match = false;

    for (const auto& query : queries) {
        const auto& [inclusions, exclusions] = query;
        const bool matches_all_inclusions = std::ranges::all_of(inclusions, prompt_matches_term);
        const bool matches_some_exclusion = std::ranges::any_of(exclusions, prompt_matches_term);
        if (!matches_all_inclusions || matches_some_exclusion) {
            continue;
        }
        else {
            match = true;
            break;
        }
    }

    return match;
}
