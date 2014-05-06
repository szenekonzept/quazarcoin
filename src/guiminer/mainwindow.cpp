#include <sstream>
#include <iostream>
#include <QMessageBox>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pThreadMining(NULL)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMinerState()));
    timer->start(2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMinerState()
{
    if (NULL != pThreadMining) {
        std::stringstream ss;
        ss << pThreadMining->getHashRate() << " h/s | Stop";
        ui->pushButtonStart->setText(ss.str().c_str());
    } else {
        ui->pushButtonStart->setText("Start");
    }
}

void MainWindow::on_pushButtonStart_clicked()
{
    std::string donorNode, donorAddress;
    std::string Node, Address;

    if (ui->radioButtonBCN->isChecked()) {
        donorNode = "127.0.0.1:8081";
        donorAddress = ui->lineEditBCN->text().toStdString();
    } else {
        donorNode = "127.0.0.1:18081";
        donorAddress = ui->lineEditBMR->text().toStdString();
    }

    Node = "127.0.0.1:24081";
    Address = ui->lineEditFCN->text().toStdString();

    if (NULL == pThreadMining) {
        pThreadMining = new thread_mining(donorNode, Node, ui->spinBoxThreads->value(), donorAddress, Address);
        pThreadMining->start(QThread::IdlePriority);
        QThread::msleep(500);
        if (!pThreadMining->isOk()) {
            QMessageBox msgbox;
            msgbox.setText("Start mining failed");
            int ret = msgbox.exec();
            delete pThreadMining;
            pThreadMining = NULL;
        } else {
            ui->pushButtonStart->setText("Stop");
        }
    } else {
        pThreadMining->StopMe();
        while (pThreadMining->isRunning()) {
            std::cout << "Waiting ..." << std::endl;
            QThread::msleep(200);
        }
        delete pThreadMining;
        pThreadMining = NULL;
        ui->pushButtonStart->setText("Start");
    }
}

void MainWindow::on_radioButtonBMR_clicked()
{
    if (ui->radioButtonBMR->isChecked()) {
        ui->lineEditBMR->setEnabled(true);
        ui->lineEditBCN->setEnabled(false);
    } else {
        ui->lineEditBMR->setEnabled(false);
        ui->lineEditBCN->setEnabled(true);
    }
}

void MainWindow::on_radioButtonBCN_clicked()
{
    if (ui->radioButtonBCN->isChecked()) {
        ui->lineEditBCN->setEnabled(true);
        ui->lineEditBMR->setEnabled(false);
    } else {
        ui->lineEditBCN->setEnabled(false);
        ui->lineEditBMR->setEnabled(true);
    }
}

void MainWindow::on_checkBoxFCN_clicked()
{
    if (ui->checkBoxFCN->isChecked()) {
        ui->lineEditFCN->setEnabled(true);
        ui->pushButtonStart->setEnabled(true);
    } else {
        ui->lineEditFCN->setEnabled(false);
        ui->pushButtonStart->setEnabled(false);
    }
}
