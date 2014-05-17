#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMessageBox>
#include <QMainWindow>
#include "../minerc/minerc.h"

namespace Ui {
class MainWindow;
}

class thread_mining : public QThread
{
    Q_OBJECT

public:
    thread_mining(const std::string& _address1, const std::string& _address2, size_t _threads, const std::string& _wallet1, const std::string& _wallet2)
        : bOk(true), bStarted(false), address1(_address1), address2(_address2), threads(_threads), wallet1(_wallet1), wallet2(_wallet2) {
    }

    void StopMe() {
        mh.StopMining();
    }

    bool isOk() {
        return bOk;
    }

    bool hasStarted() {
        return bStarted;
    }

    double getHashRate() {
        return mh.GetHashRate();
    }

protected:
    void run() {
        if (!mh.StartMining(address1, address2, threads, wallet1, wallet2))
            bOk = false;
        bStarted = true;
    }

    MinerHandle mh;

    bool bOk, bStarted;

    std::string address1;
    std::string address2;
    size_t threads;
    std::string wallet1;
    std::string wallet2;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButtonStart_clicked();
    void updateMinerState();

    void on_radioButtonBMR_clicked();

    void on_radioButtonBCN_clicked();

    void on_checkBoxFCN_clicked();

    void on_radioButtonQCN_clicked();

private:
    Ui::MainWindow *ui;
    thread_mining *pThreadMining;
};

#endif // MAINWINDOW_H
