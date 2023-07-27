#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class PopUp : public QWidget{
    Q_OBJECT
    /// Свойство полупрозрачности
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)
    /// Установка значения прозрачности
    void setPopupOpacity(float opacity);
    /// Получение значения прозрачности
    float getPopupOpacity() const;
public:
    /// Конструктор
    explicit PopUp(QWidget *parent = 0);
protected:
    /// Фон будет отрисовываться через метод перерисовки
    void paintEvent(QPaintEvent *event);
public slots:
    /// Установка текста в уведомление
    void setPopupText(const QString& text);
    ///  Собственный метод показа виджета
    ///  Необходимо для преварительной настройки анимации
    void show();
private slots:
    /// Слот для запуска анимации скрытия
    void hideAnimation();
    /// По окончании анимации, в данном слоте делается проверка,
    /// виден ли виджет, или его необходимо скрыть
    void hide();
private:
    /// Label с сообщением
    QLabel mLabel;
    /// Размещение для лейбла
    QGridLayout mLayout;
    /// Свойство анимации для всплывающего сообщения
    QPropertyAnimation mAnimation;
    /// Свойства полупрозрачности виджета
    float mPopupOpacity;
    /// Таймер, по которому виджет будет скрыт
    QTimer *mTimer;
};

#endif // POPUP_H
