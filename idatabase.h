#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QColor>

class IDatabase : public QObject
{
    Q_OBJECT
public:
    explicit IDatabase(QObject *parent = nullptr);
    ~IDatabase();

    // 初始化数据库
    bool initDatabase();

    // 事件相关操作
    bool addEvent(const QString& title, const QDateTime& startTime, 
                 const QDateTime& endTime, const QString& description, 
                 const QColor& color);
    
    bool updateEvent(int eventId, const QString& title, 
                    const QDateTime& startTime, const QDateTime& endTime,
                    const QString& description, const QColor& color);
    
    bool deleteEvent(int eventId);
    
    // 查询事件
    QList<QPair<int, QVariantMap>> getEventsByDate(const QDate& date);
    QList<QPair<int, QVariantMap>> getEventsByDateRange(const QDate& startDate, 
                                                       const QDate& endDate);

private:
    QSqlDatabase db;
    QString dbPath;

    bool createTables();
    bool openDatabase();
    void closeDatabase();
};

#endif // IDATABASE_H 