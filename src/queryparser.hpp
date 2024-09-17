#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>

bool checkQuery(const QString& queryStr);
QList<QPair<QStringList, QStringList>> getQueries(const QString& queryStr);
