#ifndef OPERATIONREPOSITORY_H
#define OPERATIONREPOSITORY_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>

class OperationRepository : public QObject
{
    Q_OBJECT
public:
    explicit OperationRepository(const QString &path, QObject *parent = 0);

    void SaveRoom(QJsonObject &room);
    void UpdateRoom(QJsonObject &room, int id);
    void DeleteRoom(int id);
    QJsonObject GetAllRooms();
    QJsonObject GetCurrentRoom(int id);

private:
    QSqlDatabase db;

    const QString mSaveRoom = "INSERT INTO room (roomName, roomContent) VALUES (:roomName, :roomContent)";
    const QString mUpdateRoom = "UPDATE room SET roomName = (:roomName), roomContent = (:roomContent) WHERE id = (:id)";
    const QString mDeleteRoomById = "DELETE FROM room WHERE id = (:id)";
    const QString mGetAllRooms = "SELECT id, roomName FROM room";
    const QString mGetRoomById = "SELECT * FROM room WHERE id = (:id)";
};

#endif // OPERATIONREPOSITORY_H
