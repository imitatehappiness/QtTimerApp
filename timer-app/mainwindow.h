#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

class PopUp;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/// Статусы состояния таймера
enum status{
    none = 0,
    work,
    paused,
    rest,
};

/// Стуруктура для хранения времени
struct sTime{
    uint hh,mm,ss;
    uint total;
    sTime(){
        hh = mm = ss = total = 0;
    }
};

/// Стурктура для работы с кругами, храним общее и текущее кол-во кругов
struct sReps{
    uint current, total;
    sReps(){
        current = total = 1;
    }
};

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    /// Переопределенный метод
    void show();
private slots:
    void on_sB_reps_value_valueChanged(int arg1);
    void on_tE_work_time_value_userTimeChanged(const QTime &time);
    void on_tE_rest_time_value_userTimeChanged(const QTime &time);
    void on_pB_start_pause_resume_clicked();
    void on_pB_stop_clicked();
    /// Обновление прогресса
    void updateTimerLabel();
private:
    Ui::MainWindow *ui;
    /// Время на перерыв
    sTime mRestTime;
    /// Время на работу
    sTime mWorkTime;
    /// Всего времени
    sTime mTotalTime;
    /// Кол-во перерывов
    sReps mReps;
    /// Объект всплывающего окна с уведомлением
    PopUp *mPopUp;
    /// Объект будущей иконки приложения для трея
    QSystemTrayIcon* mTrayIcon;
    /// Таймер на обновление
    QTimer *mTimer;
    /// Переменная хранящая сколько секунд осталось до конца таймер
    uint mRemain;
    /// Статусы таймера
    status mStatus;
    /// Переменная хранящая сколько времени прошло на текущем этапе или перерыве
    uint mProgress;
private:
    /// Метод для расчета времени в секундах для поля total в структуре sTime
    void calculateTotalTime(sTime &data, const QTime &time);
    /// Метод для вычисления h m s для mTotalTime
    void calculateHMSForTotalTime();
    /// Установка времени в правильном формате в l_total_remaining_value
    void setTotalTimeInWidget();
    /// Вкл/Откл виджеты настройки тамера
    void setEnabledWidgets(const bool enabled);
    /// Инициализация Tray Icon
    void trayInit();
    /// Метод для расчета h m s через общее кол-во секунд
    void convertSStoHMS(uint ss, QString& h, QString& m, QString& second);
    /// Метод для воспроизведения звука
    void playSound(const QString& path);
};
