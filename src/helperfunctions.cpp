#include "helperfunctions.hpp"

#include <QCheckBox>
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

#include "hpb_globals.hpp"

/*****************
 * QMessageBoxes *
 *****************/

int Ask(const QString& text, const QString& informativeText, bool& dontShowAgain) {
    QMessageBox msg;
    msg.setText(text);
    msg.setInformativeText(informativeText);
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    auto *checkbox = new QCheckBox();
    checkbox->setCheckState(Qt::Unchecked);
    checkbox->setText("Don't show this again");
    msg.setCheckBox(checkbox);
    const int result = msg.exec();
    dontShowAgain = checkbox->checkState();
    return result;
}
void PopUp(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::NoButton);
    const int popupDuration = 1500;
    QTimer::singleShot(popupDuration, &msgBox, &QMessageBox::accept);
    msgBox.exec();
}
void Warn(const QString& message)
{
    QMessageBox msg;
    msg.setText(message);
    msg.setIcon(QMessageBox::Warning);
    msg.exec();
}


/*******************************
 * QJson convenience functions *
 *******************************/

QJsonObject getDatasetObj(const QTreeWidgetItem* item, const QString& datasetBase, const QString& datasetSpec, const QJsonObject& helmDataJson)
{
    QString metric;
    QString split;

    QString datasetName = datasetBase;
    if (!datasetSpec.isEmpty()) {
        datasetName += ":" + datasetSpec;
    }
    for (const auto& array : helmDataJson) {
        for (auto&& obj : array.toArray()) {
            if (obj.toObject()["name"] != datasetName) {
                continue;
            }
            metric = obj.toObject()["metric"].toString();
            split = obj.toObject()["split"].toString();
        }
    }

    QJsonObject datasetSpecification;
    QJsonObject const samples = getSamples(item);
    datasetSpecification.insert("datasetSpec", datasetSpec);
    datasetSpecification.insert("metric", metric);
    datasetSpecification.insert("split", split);
    datasetSpecification.insert("samples", samples);

    QJsonObject dataset;
    dataset.insert(datasetBase, datasetSpecification);

    return dataset;
}
QJsonObject getSamples(const QTreeWidgetItem* item)
{
    QJsonObject samples;

    const int childCount = item->childCount();
    for (int i = 0; i < childCount; ++i) {
        const QTreeWidgetItem* child = item->child(i);
        if (child->data(HELMPromptBrowser::PTIsSelectedColumn, Qt::DisplayRole).toBool()) {
            samples.insert(getPID(child), getCID(child));
        }
    }

    return samples;
}
QJsonDocument getTaskInstances(const QString& taskDir, const QString& helmDataPath)
{
    QFile instancesFile(helmDataPath + "/" + taskDir + "/instances.json");
    if (!instancesFile.open(QIODevice::ReadOnly)) {
        QMessageBox msg;
        msg.setText("Failed to open instances.json from " + taskDir);
        msg.exec();
        return {};
    }
    return QJsonDocument::fromJson(instancesFile.readAll());
}
QJsonObject loadHelmDataConfig(const QString& helmDataJson)
{
    QFile helmDataJson_file(helmDataJson);
    if (!helmDataJson_file.open(QIODevice::ReadOnly)) {
        return {};
    }
    const QJsonDocument helmDatasetConfig = QJsonDocument::fromJson(helmDataJson_file.readAll());

    return helmDatasetConfig.toVariant().toJsonObject();
}
QString prettyPrint(const QJsonObject& obj, const QString& dataset)
{
    const QJsonArray references = obj["references"].toArray();
    const QString promptId = obj["id"].toString();
    QString inputText;

    if (!obj["input"].toObject()["text"].toString().isEmpty()) {
        inputText = obj["input"].toObject()["text"].toString() + "\n\n";
    }

    QString referencesText;

    if (!references.empty()) {
        referencesText += "REFERENCES:\n\n";
        for (auto&& value : references) {
            const QJsonObject reference = value.toObject();
            referencesText += "- " + reference["output"].toObject()["text"].toString();
            referencesText += " [ ";
            const QJsonArray tags = reference["tags"].toArray();
            for (auto&& tag : tags) {
                referencesText += tag.toString() + " ";
            }
            referencesText += "]\n";
        }
        referencesText += "\n";
    }

    QString subsplit;

    if (obj.contains("sub_split")) {
        subsplit += "SUB-SPLIT: " + obj["sub_split"].toString() + "\n\n";
    }

    QString perturbed;

    if (obj.contains("perturbation")) {
        perturbed += "PERTURBATION: prompt is perturbed";
    }

    QString const str = "DATASET: " + dataset + "\n" + "PROMPT ID: " + promptId + "\n\n"
                        + inputText + referencesText + subsplit + perturbed;

    return str.trimmed();
}


/******************************************************
 * Dataset tree and prompt tree convenience functions *
 ******************************************************/

void addPromptsToTree(const QString& dataset,
                      const QJsonDocument& instances,
                      const QList<QPair<QStringList, QStringList>>& queries,
                      bool searchIsCaseSensitive,
                      QTreeWidget* tree)
{
    auto [datasetBase, datasetSpec] = splitDatasetName(dataset);

    QList<QTreeWidgetItem*> const baseItems = tree->findItems(datasetBase,
                                                                    Qt::MatchExactly,
                                                                    HELMPromptBrowser::PTNameIDColumn);
    QList<QTreeWidgetItem*> specItems;

    if (!datasetSpec.isEmpty()) {
        specItems = tree->findItems(datasetSpec, Qt::MatchExactly | Qt::MatchRecursive, HELMPromptBrowser::PTNameIDColumn);
    }

    QTreeWidgetItem* baseItem = nullptr;

    if (!baseItems.empty()) {
        baseItem = baseItems.at(0);
    }
    else {
        baseItem = new QTreeWidgetItem();
        baseItem->setData(HELMPromptBrowser::PTNameIDColumn, Qt::DisplayRole, datasetBase);
        baseItem->setData(HELMPromptBrowser::PTIsPromptColumn, Qt::DisplayRole, false);
        if (datasetSpec.isEmpty()) {
            baseItem->setData(HELMPromptBrowser::PTHasSpecificationsColumn, Qt::DisplayRole, false);
        }
        else {
            baseItem->setData(HELMPromptBrowser::PTHasSpecificationsColumn, Qt::DisplayRole, true);
        }
    }

    QTreeWidgetItem* specItem = nullptr;
    if (!datasetSpec.isEmpty()) {
        if (!specItems.empty()) {
            specItem = specItems.at(0);
        }
        else {
            specItem = new QTreeWidgetItem();
            specItem->setData(HELMPromptBrowser::PTNameIDColumn, Qt::DisplayRole, datasetSpec);
            specItem->setData(HELMPromptBrowser::PTIsPromptColumn, Qt::DisplayRole, false);
            specItem->setData(HELMPromptBrowser::PTHasSpecificationsColumn, Qt::DisplayRole, false);
        }
    }

    QTreeWidgetItem* parent = specItem != nullptr ? specItem : baseItem;

    const size_t instanceCount = instances.array().count();
    for (size_t i = 0; i < instanceCount; ++i) {
        const QJsonObject obj = instances.array().at(i).toObject();
        const QString prompt = obj["input"].toObject()["text"].toString();

        const bool match = matches(prompt, queries, searchIsCaseSensitive);

        if (!match) {
            continue;
        }

        const QString promptId = obj["id"].toString();

        bool promptIsInTree = false;
        const size_t numberOfPrompts = parent->childCount();
        for (int j = 0; j < numberOfPrompts; ++j) {
            QTreeWidgetItem* prompt = parent->child(j);
            if (getPID(prompt) == promptId) {
                promptIsInTree = true;
                break;
            }
        }
        if (promptIsInTree) {
            continue;
        }

        auto *child = new QTreeWidgetItem();
        child->setFlags(child->flags() | Qt::ItemIsEditable);
        child->setBackground(HELMPromptBrowser::PTCIDColumn, Qt::lightGray);
        child->setForeground(HELMPromptBrowser::PTNameIDColumn, Qt::darkGray);
        child->setData(HELMPromptBrowser::PTCIDColumn, Qt::DisplayRole, "");
        child->setData(HELMPromptBrowser::PTNameIDColumn, Qt::DisplayRole, promptId);
        child->setData(HELMPromptBrowser::PTDatasetBaseColumn, Qt::DisplayRole, datasetBase);
        child->setData(HELMPromptBrowser::PTDatasetSpecColumn, Qt::DisplayRole, datasetSpec);
        child->setData(HELMPromptBrowser::PTIsPromptColumn, Qt::DisplayRole, true);
        child->setData(HELMPromptBrowser::PTPromptContentsColumn, Qt::DisplayRole, prettyPrint(obj, dataset));
        child->setData(HELMPromptBrowser::PTHasSpecificationsColumn, Qt::DisplayRole, false);
        child->setData(HELMPromptBrowser::PTIsSelectedColumn, Qt::DisplayRole, false);
        parent->addChild(child);
    }

    if (specItem != nullptr) {
        if (specItem->childCount() > 0) {
            baseItem->addChild(parent);
        }
    }
    if (baseItem->childCount() > 0) {
        tree->addTopLevelItem(baseItem);
    }
}
void deleteDatasetFromTree(const QString& datasetName, QTreeWidget* tree)
{
    auto [datasetBase, datasetSpec] = splitDatasetName(datasetName);

    if (datasetSpec.isEmpty()) {
        QList<QTreeWidgetItem *> const deletionList = tree->findItems(datasetBase,
                                                                       Qt::MatchExactly,
                                                                       1);
        for (QTreeWidgetItem* dataset : deletionList) {
            delete dataset;
        }
    }
    else {
        QList<QTreeWidgetItem *> const parentList = tree->findItems(datasetBase,
                                                                     Qt::MatchExactly,
                                                                     1);
        for (QTreeWidgetItem* parent : parentList) {
            QTreeWidgetItem* matchingChild = nullptr;
            const int childCount = parent->childCount();
            for (int i = 0; i < childCount; ++i) {
                if (getName(parent->child(i)) == datasetSpec) {
                    matchingChild = parent->child(i);
                }
            }
            if (matchingChild != nullptr) {
                parent->removeChild(matchingChild);
                delete matchingChild;
            }
            if (parent->childCount() == 0) {
                delete parent;
            }
        }
    }
}
QStringList getFiltersFromDatasetList(const QStringList& datasetNames)
{
    const QString operatingSystem = QSysInfo::productType();
    QStringList filters;
    for (QString dataset : datasetNames) {
        filters.push_back(operatingSystem == "windows" ? dataset.replace(":", "_") + "*" : dataset + "*");
    }
    return filters;
}
QStringList getHelmTaskDirs(const QStringList& datasets, const QString& helmDataPath)
{
    const QStringList filters = getFiltersFromDatasetList(datasets);

    QStringList taskDirs;

    QDir helmDir(helmDataPath);
    for (const auto& filter : filters) {
        helmDir.setNameFilters({ filter });
        taskDirs.push_back(helmDir.entryList().at(0));
    }

    return taskDirs;
}
QStringList getSelectedDatasetNames(const QTreeWidget* tree)
{
    QStringList selectedDatasets;

    const int topLevelDatasetCount = tree->topLevelItemCount();

    // process top level datasets
    for (int i = 0; i < topLevelDatasetCount; ++i) {
        QTreeWidgetItem* parent = tree->topLevelItem(i);
        QString const parentName = parent->data(0, Qt::DisplayRole).toString();

        if (parent->checkState(0) == Qt::Unchecked) {
            continue;
        }

        // if dataset has no sub-datasets, do nothing if already in tree, add it if not
        if (parent->childCount() == 0) {
            selectedDatasets.push_back(parentName);
            continue;
        }

        // dataset has sub-datasets
        // process each sub-dataset
        for (int j = 0; j < parent->childCount(); ++j) {
            QTreeWidgetItem* child = parent->child(j);
            QString const childName = child->data(0, Qt::DisplayRole).toString();

            // if sub-dataset is unchecked, delete it from prompt tree if present, ignore it if not
            if (child->checkState(0) == Qt::Unchecked) {
                continue;
            }

            // add it if not in tree
            selectedDatasets.push_back(childName);
        }
    }
    return selectedDatasets;
}
bool hasSelectedPrompts(const QTreeWidgetItem* item) {
    bool hasSelectedPrompts = false;

    const size_t promptCount = item->childCount();
    for (size_t i = 0; i < promptCount; ++i) {
        if (isSelected(item->child(i))) {
            hasSelectedPrompts = true;
            break;
        }
    }

    return hasSelectedPrompts;
}
bool matches(const QString& prompt, const QList<QPair<QStringList, QStringList>>& queries, const bool caseSensitivity) {
    const auto promptMatchesTerm = [&](const QString& term) { return prompt.contains(term, (caseSensitivity ? Qt::CaseSensitive : Qt::CaseInsensitive));};
    bool match = false;

    for (const auto& query : queries) {
        const auto& [inclusions, exclusions] = query;
        const bool matchesAllInclusions = std::ranges::all_of(inclusions, promptMatchesTerm);
        const bool matchesSomeExclusion = std::ranges::any_of(exclusions, promptMatchesTerm);
        if (!matchesAllInclusions || matchesSomeExclusion) {
            continue;
        }
        match = true;
        break;
    }

    return match;
}
QPair<QString, QString> splitDatasetName(const QString& dataset)
{
    QString datasetBase;
    QString datasetSpec;

    if (dataset == "bold:subject=all" || dataset == "boolq:only_contrast=True" || dataset == "imdb:only_contrast=True") {
        datasetBase = dataset;
        datasetSpec = {};
    }
    else if (dataset.contains("legal_support")) {
        datasetBase = dataset.split(",").at(0);
        datasetSpec = dataset.split(",").at(1);
    }
    else if (dataset.contains(":")) {
        datasetBase = dataset.split(":").at(0);
        datasetSpec = dataset.split(":").at(1);
    }
    else {
        datasetBase = dataset;
        datasetSpec = {};
    }

    return {datasetBase, datasetSpec};
}
void transformDatasetTree(QTreeWidget* datasetTree, const std::function<void(QTreeWidgetItem*)>& transformation)
{
    const size_t datasetCount = datasetTree->topLevelItemCount();
    for (size_t i = 0; i < datasetCount; ++i) {
        QTreeWidgetItem* dataset = datasetTree->topLevelItem(i);
        if (dataset->childCount() == 0) {
            transformation(dataset);
            continue;
        }
        const size_t specificationCount = dataset->childCount();
        for (size_t j = 0; j < specificationCount; ++j) {
            transformation(dataset->child(j));
        }
    }
}
void transformPromptTree(QTreeWidget* promptTree, const std::function<void(QTreeWidgetItem*)>& transformation)
{
    const size_t datasetCount = promptTree->topLevelItemCount();
    for (size_t i = 0; i < datasetCount; ++i) {
        QTreeWidgetItem* dataset = promptTree->topLevelItem(i);
        if (!hasSpecifications(dataset)) {
            for (size_t j = 0; j < dataset->childCount(); ++j) {
                QTreeWidgetItem* prompt = dataset->child(j);
                transformation(prompt);
            }
        }
        else {
            const size_t subdatasetCount = dataset->childCount();
            for (size_t j = 0; j < subdatasetCount; ++j) {
                QTreeWidgetItem* subdataset = dataset->child(j);
                for (size_t k = 0; k < subdataset->childCount(); ++k) {
                    QTreeWidgetItem* prompt = subdataset->child(k);
                    transformation(prompt);
                }
            }
        }
    }
}


/****************************
 * Prompt-related functions *
 ***************************/

QString getCID(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTCIDColumn, Qt::DisplayRole).toString();
}
QString getDatasetBase(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTDatasetBaseColumn, Qt::DisplayRole).toString();
}
QString getDatasetSpec(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTDatasetSpecColumn, Qt::DisplayRole).toString();
}
QString getName(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTNameIDColumn, Qt::DisplayRole).toString();
}
QString getPID(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTNameIDColumn, Qt::DisplayRole).toString();
}
QString getPrompt(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTPromptContentsColumn, Qt::DisplayRole).toString();
}
QString getReferences(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTReferencesColumn, Qt::DisplayRole).toString();
}
bool hasSpecifications(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTHasSpecificationsColumn, Qt::DisplayRole).toBool();
}
bool isPrompt(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTIsPromptColumn, Qt::DisplayRole).toBool();
}
bool isSelected(const QTreeWidgetItem* item)
{
    return item->data(HELMPromptBrowser::PTIsSelectedColumn, Qt::DisplayRole).toBool();
}
void setCID(QTreeWidgetItem* item, const QString& cid)
{
    item->setData(HELMPromptBrowser::PTCIDColumn, Qt::DisplayRole, cid);
}
void setSelectedStatus(QTreeWidgetItem* item, bool status)
{
    item->setData(HELMPromptBrowser::PTIsSelectedColumn, Qt::DisplayRole, status);
    if (status) {
        item->setForeground(HELMPromptBrowser::PTNameIDColumn, Qt::black);
        item->setBackground(HELMPromptBrowser::PTCIDColumn, Qt::blue);
        item->setForeground(HELMPromptBrowser::PTCIDColumn, Qt::white);
    }
    else {
        item->setForeground(HELMPromptBrowser::PTNameIDColumn, Qt::darkGray);
        item->setBackground(HELMPromptBrowser::PTCIDColumn, Qt::lightGray);
        item->setForeground(HELMPromptBrowser::PTCIDColumn, Qt::black);
    }
}
