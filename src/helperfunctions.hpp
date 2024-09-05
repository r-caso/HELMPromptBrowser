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

int Ask(const QString& text, const QString& informative_text, bool& dont_show_again);
void PopUp(const QString& message);
void Warn(const QString& message);

/*******************************
 * QJson convenience functions *
 *******************************/

QJsonObject getDatasetObj(const QTreeWidgetItem* item, const QString& dataset_base, const QString& dataset_spec, const QJsonObject& helm_data_json);
QJsonObject getSamples(const QTreeWidgetItem* item);
QJsonDocument getTaskInstances(const QString& task_dir, const QString& helm_data_path);
QJsonObject loadHelmDataConfig(const QString& helm_data_json);
QString prettyPrint(const QJsonObject& obj, const QString& dataset);

/******************************************************
 * Dataset tree and prompt tree convenience functions *
 ******************************************************/

void addPromptsToTree(const QString& dataset,
                      const QJsonDocument& instances,
                      const QList<QPair<QStringList, QStringList>>& queries,
                      bool search_is_case_sensitive,
                      QTreeWidget* tree);
void deleteDatasetFromTree(const QString& dataset_name, QTreeWidget* tree);
QStringList getFiltersFromDatasetList(const QStringList& dataset_names);
QStringList getHelmTaskDirs(const QStringList& datasets, const QString& helm_data_path);
QStringList getSelectedDatasetNames(const QTreeWidget* tree);
bool has_selected_prompts(const QTreeWidgetItem* item);
bool matches(const QString& prompt, const QList<QPair<QStringList, QStringList>>& queries, const bool case_sensitivity);
QPair<QString, QString> splitDatasetName(const QString& dataset);
void transformDatasetTree(QTreeWidget* dataset_tree, const std::function<void(QTreeWidgetItem*)>& transformation);
void transformPromptTree(QTreeWidget* prompt_tree, const std::function<void(QTreeWidgetItem*)>& transformation);

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
void setCID(QTreeWidgetItem* item, const QString& cid);
void setSelectedStatus(QTreeWidgetItem* item, bool status);
