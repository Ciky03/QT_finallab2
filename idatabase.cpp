#include "idatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>

IDatabase::IDatabase(QObject *parent) : QObject(parent)
{
    dbPath = "E:/develop/QtProject/FinalLab2/lab3.db";
}

IDatabase::~IDatabase()
{
    closeDatabase();
}

bool IDatabase::initDatabase()
{
    if (!openDatabase()) {
        return false;
    }
    return createTables();
}

bool IDatabase::openDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }
    return true;
}

void IDatabase::closeDatabase()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool IDatabase::createTables()
{
    QSqlQuery query;
    bool success = query.exec(
                       "CREATE TABLE IF NOT EXISTS Events ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "title TEXT NOT NULL,"
                       "start_time DATETIME NOT NULL,"
                       "end_time DATETIME NOT NULL,"
                       "description TEXT,"
                       "color TEXT NOT NULL,"
                       "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                       ")"
                   );

    if (!success) {
        qDebug() << "Error creating tables:" << query.lastError().text();
        return false;
    }
    return true;
}

bool IDatabase::addEvent(const QString& title, const QDateTime& startTime,
                         const QDateTime& endTime, const QString& description,
                         const QColor& color)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Events (title, start_time, end_time, description, color) "
        "VALUES (:title, :start_time, :end_time, :description, :color)"
    );

    query.bindValue(":title", title);
    query.bindValue(":start_time", startTime.toString(Qt::ISODate));
    query.bindValue(":end_time", endTime.toString(Qt::ISODate));
    query.bindValue(":description", description);
    query.bindValue(":color", color.name());

    if (!query.exec()) {
        qDebug() << "Error adding event:" << query.lastError().text();
        return false;
    }
    return true;
}

bool IDatabase::updateEvent(int eventId, const QString& title,
                            const QDateTime& startTime, const QDateTime& endTime,
                            const QString& description, const QColor& color)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE Events SET title = :title, start_time = :start_time, "
        "end_time = :end_time, description = :description, color = :color "
        "WHERE id = :id"
    );

    query.bindValue(":id", eventId);
    query.bindValue(":title", title);
    query.bindValue(":start_time", startTime.toString(Qt::ISODate));
    query.bindValue(":end_time", endTime.toString(Qt::ISODate));
    query.bindValue(":description", description);
    query.bindValue(":color", color.name());

    if (!query.exec()) {
        qDebug() << "Error updating event:" << query.lastError().text();
        return false;
    }
    return true;
}

bool IDatabase::deleteEvent(int eventId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM events WHERE id = :id");
    query.bindValue(":id", eventId);
    
    if (!query.exec()) {
        qDebug() << "Failed to delete event:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

QList<QPair<int, QVariantMap>> IDatabase::getEventsByDate(const QDate& date)
{
    QList<QPair<int, QVariantMap>> events;
    QSqlQuery query;
    query.prepare(
        "SELECT id, title, start_time, end_time, description, color "
        "FROM Events "
        "WHERE date(start_time) = :date "
        "ORDER BY start_time"
    );

    query.bindValue(":date", date.toString(Qt::ISODate));

    if (query.exec()) {
        while (query.next()) {
            QVariantMap event;
            int id = query.value("id").toInt();
            event["title"] = query.value("title").toString();
            event["start_time"] = QDateTime::fromString(query.value("start_time").toString(), Qt::ISODate);
            event["end_time"] = QDateTime::fromString(query.value("end_time").toString(), Qt::ISODate);
            event["description"] = query.value("description").toString();
            event["color"] = QColor(query.value("color").toString());
            events.append({id, event});
        }
    } else {
        qDebug() << "Error querying events:" << query.lastError().text();
    }
    return events;
}

QList<QPair<int, QVariantMap>> IDatabase::getEventsByDateRange(const QDate& startDate,
        const QDate& endDate)
{
    QList<QPair<int, QVariantMap>> events;
    QSqlQuery query;
    query.prepare(
        "SELECT id, title, start_time, end_time, description, color "
        "FROM Events "
        "WHERE date(start_time) BETWEEN :start_date AND :end_date "
        "ORDER BY start_time"
    );

    query.bindValue(":start_date", startDate.toString(Qt::ISODate));
    query.bindValue(":end_date", endDate.toString(Qt::ISODate));

    if (query.exec()) {
        while (query.next()) {
            QVariantMap event;
            int id = query.value("id").toInt();
            event["title"] = query.value("title").toString();
            event["start_time"] = QDateTime::fromString(query.value("start_time").toString(), Qt::ISODate);
            event["end_time"] = QDateTime::fromString(query.value("end_time").toString(), Qt::ISODate);
            event["description"] = query.value("description").toString();
            event["color"] = QColor(query.value("color").toString());
            events.append({id, event});
        }
    } else {
        qDebug() << "Error querying events:" << query.lastError().text();
    }
    return events;
}
