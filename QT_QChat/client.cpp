#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connecter = new bool();
    connecter = false;
    clientName = "";



    connect(socket,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

}

Client::~Client()
{
    delete ui;
    socket->destroyed();
}

void Client::connected(){
    ui->BT_connect->setText("Disconnet");
}

void Client::disconnected(){
    ui->BT_connect->setText("Connect");
    socket->close();

    ui->list_chat->clear();
    ui->list_broadcast->clear();
    ui->list_client->clear();
    ui->list_hist->clear();
    ui->list_msg->clear();
}

void Client::readyRead(){

    QByteArray text;
    QString msg;

    text = socket->read(1000);
    msg.append(text);
    if(!msg.isEmpty() && msg.size() > 4){
        QStringList list = msg.split('@');
        if(list.at(0) == "Client"){
            ui->list_client->clear();
            for(int i =1;i<list.size() ;i++){
                if(list.at(i) != ui->LE_name->text()){
                    QListWidgetItem *item = new QListWidgetItem(list.at(i));
                    ui->list_client->addItem(item);
                    if(list.at(i) == clientName){
                        item->setSelected(true);
                    }
                }
            }
        }else if(list.at(0) == "MSG"){
/*            QListWidgetItem *item = new QListWidgetItem(list.at(1));
            ui->list_msg->addItem(item);
            item->setBackgroundColor(Qt::green);
*/
            QStringList liste = list.at(1).split(':');
          /* QString str;
           if(ui->list_client->currentItem())
                str = ui->list_client->currentItem()->text();
            else
                str ="";
*/
            if(liste.at(0) == clientName){
                QListWidgetItem *item = new QListWidgetItem(liste.at(1));
                ui->list_msg->addItem(item);
                item->setBackgroundColor(Qt::green);
            }else{
                QListWidgetItem *item = new QListWidgetItem(list.at(1));
                ui->list_chat->addItem(item);
                item->setBackgroundColor(Qt::cyan);
            }
/*            QString str =list.at(1);
            //QListWidgetItem *item = ui->list_client->findItems(str, Qt::MatchWrap);
            QList<QListWidgetItem *> items  = ui->list_client->findItems(str, Qt::MatchExactly);
            foreach(QListWidgetItem *item, items){
//                if(item->text() == str){}
                item->setBackgroundColor(Qt::red);
            }
*/
        }else if(list.at(0) == "BRD"){
            ui->list_broadcast->addItem(list.at(1));
        }else if(list.at(0) == "HST"){
            QStringList liste = list.at(1).split('<');
            ui->list_hist->clear();
            for(int i =1;i<liste.size() ;i++){
                QStringList listes = liste.at(i).split('>');
                QListWidgetItem *items = new QListWidgetItem(listes.at(2));
                ui->list_hist->addItem(items);
                if(listes.at(1) == ui->LE_name->text()){
                    items->setBackground(Qt::green);
                }else{
                    items->setBackground(Qt::yellow);
                }
            }
        }
    }

}

void Client::on_BT_connect_clicked()
{
    if(connecter){
            socket->write("@EXIT@");
            socket->disconnectFromHost();
            connecter = false;

            ui->list_broadcast->clear();
            ui->list_chat->clear();
            ui->list_msg->clear();
            ui->list_client->clear();
            ui->list_hist->clear();
    }else{
        if(ui->LE_name->text().isEmpty()){
            QMessageBox::warning(this,qApp->tr("warning"),qApp->tr("Can't accept blank Name"),QMessageBox::Ok);
            return ;
        }
        if(ui->LE_ip->text().isEmpty()){
            QMessageBox::warning(this,qApp->tr("warning"),qApp->tr("Can't accept blank IP"),QMessageBox::Ok);
            return ;
        }
        if(ui->LE_port->text().isEmpty()){
            QMessageBox::warning(this,qApp->tr("warning"),qApp->tr("Can't accept blank Port"),QMessageBox::Ok);
            return ;
        }
         socket->connectToHost(ui->LE_ip->text(),ui->LE_port->text().toInt());
         if(!socket->waitForConnected(2000)){
                 QMessageBox::warning(this,qApp->tr("warning"),qApp->tr("Could Not Connecte \n Try Later"),QMessageBox::Ok);
             return;
         }
        QString msg = "I am <"+ ui->LE_name->text()+"> Hello My Dears Friends CI/GI 2015 hava a nice day ";
        QByteArray text;
        text.append(msg);

         socket->write(text);
         connecter = true;
    }

}

void Client::on_message_returnPressed()
{
    if(connecter){

        if(ui->list_client->currentItem()){
        if(clientName != ui->list_client->currentItem()->text()){
            ui->list_msg->clear();
            clientName = ui->list_client->currentItem()->text();
        }
        QString msg = "<"+ clientName+">"+ui->message->text();
        QByteArray text;
        text.append(msg);

        socket->write(text);
        //ui->list_msg->addItem(">>:"+ui->message->text());
//        QListWidgetItem *item = new QListWidgetItem(text);
        QListWidgetItem *item = new QListWidgetItem(ui->message->text());
        ui->list_msg->addItem(item);
        item->setBackgroundColor(Qt::yellow);
        ui->message->clear();
        }
    }else{
        QMessageBox::warning(this,qApp->tr("warning"),qApp->tr("you Should Connecte First"),QMessageBox::Ok);
    return;
    }
}


void Client::on_broadcast_returnPressed()
{
    if(connecter){
        QString msg = ui->broadcast->text();
        QByteArray text;
        text.append(msg);

        socket->write(text);
        ui->list_broadcast->addItem(">>:"+ui->broadcast->text());
        ui->broadcast->clear();
    }else{
        QMessageBox::warning(this,qApp->tr("warning"),qApp->tr("you Should Connecte First"),QMessageBox::Ok);
    return;
    }
}

void Client::on_list_client_clicked(const QModelIndex &index)
{
    if(clientName != ui->list_client->currentItem()->text())
        ui->list_msg->clear();
    clientName = ui->list_client->currentItem()->text();
}

void Client::on_list_client_doubleClicked(const QModelIndex &index)
{
    QString msg = ";"+ui->list_client->currentItem()->text()+":";
    QByteArray text;
    text.append(msg);
    socket->write(text);
}
