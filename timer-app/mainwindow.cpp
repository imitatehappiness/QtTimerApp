#include "mainwindow.h"
#include "popup.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QtMultimedia/QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(updateTimerLabel()));

    mPopUp = new PopUp(this);
    mPopUp->setObjectName("popup");

    mRemain = 0;
    mProgress = 0;
    mStatus = status::none;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_sB_reps_value_valueChanged(int arg1){
    mReps.total = arg1;
    mTotalTime.total = mWorkTime.total * mReps.total + mRestTime.total * (mReps.total - 1);

    calculateHMSForTotalTime();
    setTotalTimeInWidget();
}

void MainWindow::on_tE_work_time_value_userTimeChanged(const QTime &time){
    calculateTotalTime(mWorkTime, time);
    calculateHMSForTotalTime();
    setTotalTimeInWidget();
}

void MainWindow::on_tE_rest_time_value_userTimeChanged(const QTime &time){
    calculateTotalTime(mRestTime, time);
    calculateHMSForTotalTime();
    setTotalTimeInWidget();
}

void MainWindow::on_pB_start_pause_resume_clicked(){
    bool skip = false;
    uint bntStatus = ui->pB_start_pause_resume->property("status").toUInt();
    if(bntStatus == status::none){
        if(mWorkTime.total <= 0){
            QMessageBox mBox;
            mBox.setWindowIcon(QIcon(":/resources/icons/timer.png"));
            mBox.setIcon(QMessageBox::Information);
            mBox.setText("Work time must be great that zero!");
            mBox.setButtonText(QMessageBox::Ok, "Ok");
            mBox.exec();
            skip = true;
        }
        if(!skip){
            mReps.current = 1;
            mRemain = mTotalTime.total;
            mTimer->start(1000);
            ui->pB_start_pause_resume->setProperty("status", status::work);
            ui->pB_start_pause_resume->setText("PAUSE");
            ui->l_off_work->setText("WORK " + QString::number(mReps.current) + "/" + QString::number(mReps.total));
            mStatus = status::work;
            setEnabledWidgets(false);
            playSound("resources/sound/notify.mp3");
            mPopUp->setPopupText("The timer is running!");
            mPopUp->show();
        }

    }else if(bntStatus == status::work){
        ui->pB_start_pause_resume->setProperty("status", status::paused);
        ui->pB_start_pause_resume->setText("RESUME");
        ui->l_off_work->setText("PAUSED " + QString::number(mReps.current) + "/" + QString::number(mReps.total));
        mTimer->stop();
    }else if(bntStatus == status::paused){
        ui->pB_start_pause_resume->setProperty("status", status::work);
        ui->pB_start_pause_resume->setText("PAUSE");
        ui->l_off_work->setText("WORK " + QString::number(mReps.current) + "/" + QString::number(mReps.total));
        mTimer->start(1000);
    }
    if(!skip){
        ui->l_total_remaining->setText("REMAINING");
    }
}

void MainWindow::on_pB_stop_clicked(){
    ui->l_total_remaining->setText("TOTAL");
    ui->pB_start_pause_resume->setProperty("status", status::none);
    ui->pB_start_pause_resume->setText("START");
    ui->l_off_work->setText("OFF");
    ui->l_timer_value->setText("00:00:00");
    setEnabledWidgets(true);
    mTimer->stop();
    mProgress = 0;
    mReps.current = 1;
    mRemain = mTotalTime.total;
    setTotalTimeInWidget();
    mStatus = status::none;
}

void MainWindow::updateTimerLabel(){
    mRemain--;
    mProgress++;

    if(mRemain <= 0){
        ui->l_total_remaining->setText("TOTAL");
        ui->pB_start_pause_resume->setProperty("status", status::none);
        ui->pB_start_pause_resume->setText("START");
        ui->l_off_work->setText("OFF");
        setEnabledWidgets(true);
        mTimer->stop();
    }

    QString hour, minute, second;

    convertSStoHMS(mRemain, hour, minute, second);
    ui->l_total_remaining_value->setText(hour + ":" + minute + ":" + second);

    if(mProgress >= mWorkTime.total && mStatus == status::work){
        mProgress = 0;
        mStatus = status::rest;

        if (mReps.total == mReps.current){
            ui->l_off_work->setText("OFF");
            mPopUp->setPopupText("The work is over!");
        }else{
            ui->l_off_work->setText("REST");
            mPopUp->setPopupText("It's time to rest!");
        }

        playSound("resources/sound/notify.mp3");
        mPopUp->show();

    }else if(mProgress > mRestTime.total && mStatus == status::rest){
        mProgress = 1;
        mStatus = status::work;
        mReps.current++;
        ui->l_off_work->setText("WORK " + QString::number(mReps.current) + "/" + QString::number(mReps.total));

        playSound("resources/sound/notify.mp3");

        mPopUp->setPopupText("The break is over, it's time to work");
        mPopUp->show();
    }

    convertSStoHMS(mProgress, hour, minute, second);
    ui->l_timer_value->setText(hour + ":" + minute + ":" + second);
}

void MainWindow::calculateTotalTime(sTime& data, const QTime &time){
    data.hh = time.hour();
    data.mm = time.minute();
    data.ss = time.second();
    data.total = data.hh*3600 + data.mm*60 + data.ss;
}

void MainWindow::calculateHMSForTotalTime(){
    mTotalTime.total = mWorkTime.total * mReps.total + mRestTime.total * (mReps.total - 1);

    QString hour, minute, second;
    convertSStoHMS(mTotalTime.total, hour, minute, second);
    mTotalTime.hh = hour.toUInt();
    mTotalTime.mm = minute.toUInt();
    mTotalTime.ss = second.toUInt();
}

void MainWindow::setTotalTimeInWidget(){
    QString hour, minute, second;
    convertSStoHMS(mTotalTime.total, hour, minute, second);
    ui->l_total_remaining_value->setText(hour + ":" + minute + ":" + second);
}

void MainWindow::setEnabledWidgets(const bool enabled){
    if(enabled){
        ui->sB_reps_value->setEnabled(true);
        ui->tE_work_time_value->setEnabled(true);
        ui->tE_rest_time_value->setEnabled(true);
    }else{
        ui->sB_reps_value->setEnabled(false);
        ui->tE_work_time_value->setEnabled(false);
        ui->tE_rest_time_value->setEnabled(false);
    }
}

void MainWindow::convertSStoHMS(uint ss, QString &hour, QString &minute, QString &second){
    hour   = (ss / 3600) < 10 ? "0" + QString::number(ss / 3600) : QString::number(ss / 3600);
    minute = ((ss % 3600) / 60) < 10 ? "0" + QString::number((ss % 3600) / 60) : QString::number((ss % 3600) / 60);
    second = ((ss % 3600) % 60)  < 10? "0" + QString::number((ss % 3600) % 60) : QString::number((ss % 3600) % 60);
}

void MainWindow::playSound(const QString &path){
    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(path));
    player->setVolume(30);
    player->play();
}

