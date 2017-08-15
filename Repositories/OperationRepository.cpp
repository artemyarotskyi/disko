#include "OperationRepository.h"
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonArray>
#include <QJsonValue>

OperationRepository::OperationRepository(const QString &path, QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if(!db.open())
        qDebug() << "Error: connection with database failed";
    else
        qDebug() << "Database: connection ok";
}

void OperationRepository::SaveRoom(QJsonObject &room)
{
    QJsonDocument doc(room);
    QString roomContent(doc.toJson(QJsonDocument::Compact));

    QSqlQuery query;
    query.prepare(mSaveRoom);
    query.bindValue(":roomName", room["roomName"].toString());
    query.bindValue(":roomContent",roomContent);

    if(!query.exec())
        qDebug() << "Save room error:" << query.lastError();
}

void OperationRepository::UpdateRoom(QJsonObject &room, int id)
{
    QJsonDocument doc(room);
    QString roomContent(doc.toJson(QJsonDocument::Compact));

    QSqlQuery query;
    query.prepare(mUpdateRoom);
    query.bindValue(":roomName", room["roomName"].toString());
    query.bindValue(":roomContent", roomContent);
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << "Update room error:" << query.lastError();
}

void OperationRepository::DeleteRoom(int id)
{
    QSqlQuery query;
    query.prepare(mDeleteRoomById);
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << "Delete room error:" << query.lastError();
}

QJsonObject OperationRepository::GetAllRooms()
{
    QSqlQuery query;
    if(!query.exec(mGetAllRooms))
        qDebug() << "Get all rooms error:" << query.lastError();
    else
    {        
        QJsonArray roomArray;
        while (query.next())
        {
            QJsonObject currentRoomObject;
            currentRoomObject["id"]= query.value(0).toString();
            currentRoomObject["roomName"] = query.value(1).toString();
            roomArray.append(currentRoomObject);
        }
        QJsonObject resultObject;
        resultObject["rooms"] = roomArray;

        return resultObject;
    }
}

QJsonObject OperationRepository::GetCurrentRoom(int id)
{    
    QSqlQuery query;
    query.prepare(mGetRoomById);    
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << "Get room by id error:" << query.lastError();
    else
    {
        QJsonObject resultObject;

        while(query.next())
        {
            resultObject["id"] = query.value(0).toString();
            resultObject["roomName"] = query.value(1).toString();

            QJsonDocument doc = QJsonDocument::fromJson(query.value(2).toByteArray());
            QJsonObject currentRoomObject = doc.object();
            QJsonArray lampsArray = currentRoomObject["lamps"].toArray();

            resultObject["lamps"] = lampsArray;
        }

        return resultObject;
    }
}
