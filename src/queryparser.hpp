#pragma once

#include <QString>

bool checkQuery(const QString& queryStr);
QList<QPair<QList<QString>, QList<QString>>> getQueries(const QString& queryStr);
