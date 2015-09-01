#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QIODevice>
#include <QMessageBox>
#include <QStringListModel>

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

signals:


public slots:
    void connected();
    void disconnected();
    void readyRead();


private slots:
    void on_BT_connect_clicked();
    void on_message_returnPressed();
    void on_broadcast_returnPressed();
    void on_list_client_clicked(const QModelIndex &index);

    void on_list_client_doubleClicked(const QModelIndex &index);

private:
    Ui::Client *ui;
    QTcpSocket *socket;
    bool connecter;
    QString clientName;
};

#endif // CLIENT_H
