#pragma once

#include <functional>

#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

/*****************
 * QMessageBoxes *
 *****************/

int Ask(const QString& text, const QString& informativeText, bool& dontShowAgain);
void PopUp(const QString& message);
void Warn(const QString& message);

/*******************************
 * QJson convenience functions *
 *******************************/

QJsonObject getDatasetObj(const QTreeWidgetItem* item, const QString& datasetBase, const QString& datasetSpec, const QJsonObject& helmDataJson);
QJsonObject getSamples(const QTreeWidgetItem* item);
QJsonDocument getTaskInstances(const QString& task_dir, const QString& helmDataPath);
QJsonObject loadHelmDataConfig(const QString& helmDataJson);
QString prettyPrint(const QJsonObject& obj, const QString& dataset);

/**************************************
 * Dataset tree convenience functions *
 **************************************/

QStringList getFiltersFromDatasetList(const QStringList& datasetNames);
const QList<int>& getModelList(const QTreeWidgetItem*);
QStringList getSelectedDatasetNames(const QTreeWidget* tree);
void transformDatasetTree(QTreeWidget* datasetTree, const std::function<void(QTreeWidgetItem*)>& transformation);

/************************************************
 * Prompt and prompt tree convenience functions *
 ************************************************/

void addPromptsToTree(const QString& dataset,
                      const QJsonDocument& instances,
                      const QList<QPair<QStringList, QStringList>>& queries,
                      bool searchIsCaseSensitive,
                      QTreeWidget* tree);
void deleteDatasetFromTree(const QString& dataset_name, QTreeWidget* tree);
bool hasSelectedPrompts(const QTreeWidgetItem* item);
void transformPromptTree(QTreeWidget* promptTree, const std::function<void(QTreeWidgetItem*)>& transformation);

QStringList getHelmTaskDirs(const QStringList& datasets, const QString& helmDataPath);
QPair<QString, QString> splitDatasetName(const QString& dataset);


/****************************
 * Prompt-related functions *
 ****************************/

QString getCID(const QTreeWidgetItem* item);
QString getDatasetBase(const QTreeWidgetItem* item);
QString getDatasetSpec(const QTreeWidgetItem* item);
QString getName(const QTreeWidgetItem* item);
QString getPID(const QTreeWidgetItem* item);
QString getPrompt(const QTreeWidgetItem* item);
QString getReferences(const QTreeWidgetItem* item);
bool hasSpecifications(const QTreeWidgetItem* item);
bool isPrompt(const QTreeWidgetItem* item);
bool isSelected(const QTreeWidgetItem* item);
bool matches(const QString& prompt, const QList<QPair<QStringList, QStringList>>& queries, const bool caseSensitivity);
void setCID(QTreeWidgetItem* item, const QString& cid);
void setSelectedStatus(QTreeWidgetItem* item, bool status);
