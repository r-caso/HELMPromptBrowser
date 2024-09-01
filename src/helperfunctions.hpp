#pragma once

#include <functional>

#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

int Ask(const QString& text, const QString& informative_text, bool& dont_show_again);
void Warn(const QString& message);
void PopUp(const QString& message);

QJsonObject getSamples(const QTreeWidgetItem* item);
QJsonObject getDatasetObj(const QTreeWidgetItem* item, const QString& dataset_base, const QString& dataset_spec, const QJsonObject& helm_data_json);
QJsonObject loadHelmDataConfig(const QString& helm_data_json);
QJsonDocument getTaskInstances(const QString& task_dir, const QString& helm_data_path);
QString prettyPrint(const QJsonObject& obj, const QString& dataset);

QStringList getSelectedDatasetNames(const QTreeWidget* tree);
QStringList getFiltersFromDatasetList(const QStringList& dataset_names);
void deleteDatasetFromTree(const QString& dataset_name, QTreeWidget* tree);
void addPromptsToTree(const QString& dataset,
                      const QJsonDocument& instances,
                      const QList<QPair<QStringList, QStringList>>& queries,
                      bool search_is_case_sensitive,
                      QTreeWidget* tree);
void transformPromptTree(QTreeWidget* prompt_tree, std::function<void(QTreeWidgetItem*)> transformation);
void transformDatasetTree(QTreeWidget* prompt_tree, std::function<void(QTreeWidgetItem*)> transformation);
QStringList getHelmTaskDirs(const QStringList& datasets, const QString& helm_data_path);
QPair<QString, QString> splitDatasetName(const QString& dataset);
QStringList getDatasetsToAdd(QTreeWidget* source, QTreeWidget* destination, QStringList& previous_selection);

QString getCID(const QTreeWidgetItem* item);
void setCID(QTreeWidgetItem* item, const QString& cid);
QString getPID(const QTreeWidgetItem* item);
QString getName(const QTreeWidgetItem* item);
QString getDatasetBase(const QTreeWidgetItem* item);
QString getDatasetSpec(const QTreeWidgetItem* item);
bool isPrompt(const QTreeWidgetItem* item);
QString getPrompt(const QTreeWidgetItem* item);
bool hasSpecifications(const QTreeWidgetItem* item);
bool isSelected(const QTreeWidgetItem* item);
void setSelectedStatus(QTreeWidgetItem* item, bool status);
