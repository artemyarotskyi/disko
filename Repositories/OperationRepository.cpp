#include "OperationRepository.h"

OperationRepository::OperationRepository(const QString &path, QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if(!db.open())
        qDebug() << "Error: connection with database failed";
    else
        qDebug() << "Database: connection ok";
}

void OperationRepository::SaveRoom(QJsonObject *room)
{
    QString roomName;
    QString roomContent;

    QSqlQuery query;
    query.prepare(mSaveRoom);
    query.bindValue(":roomName", roomName);
    query.bindValue(":roomContent", roomContent);
    if(!query.exec())
        qDebug() << "Save room error:" << query.lastError();
}

void OperationRepository::UpdateRoom(QJsonObject *room)
{
    QString roomName;
    QString roomContent;
    int id;

    QSqlQuery query;
    query.prepare(mSaveRoom);
    query.bindValue(":roomName", roomName);
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

QJsonObject* OperationRepository::GetAllRooms()
{
    QSqlQuery query;
    if(!query.exec(mGetAllRooms))
        qDebug() << "Get all rooms error:" << query.lastError();
    else
    {
        while (query.next())
        {
            // to json array
            QString id = query.value(0).toString(); // can to json object
            QString roomName = query.value(1).toString();
            ///
            // fill jsonobject
            ///
        }

        return &mResult;
    }
}

QJsonObject* OperationRepository::GetCurrentRoom(int id)
{
    QSqlQuery query;
    query.prepare(mGetRoomById);
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << "Get room by id error:" << query.lastError();
    else
    {
        //QJsonObject id = query.value(0).toJsonObject();
        //QJsonObject roomName = query.value(1).toJsonObject();
        mResult = query.value(2).toJsonObject();

        return &mResult;
    }
}
