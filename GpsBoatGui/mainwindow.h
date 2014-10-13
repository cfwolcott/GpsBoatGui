#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_Start_clicked();
    void GpsUpdate();

    void on_pushButton_Ard_ToggleLed_clicked();

    void on_pushButton_ServoLeft_clicked();

    void on_pushButton_ServoRight_clicked();

    void on_pushButton_ServoCenter_clicked();

private:
    Ui::MainWindow *ui;

    QTimer timer_GpsUpdate;


};

#endif // MAINWINDOW_H
