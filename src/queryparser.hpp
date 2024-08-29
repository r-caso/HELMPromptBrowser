#pragma once

#include <QString>

bool checkQuery(const QString& query_string);
QList<QPair<QList<QString>, QList<QString>>> getQueries(const QString& query_string);
