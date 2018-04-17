#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QProcess>
#include <QFileDialog>
#include <windows.h>
#include <QTimer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_Button_SetApp_clicked();
    void on_Button_StartTimer_clicked();
    void on_Button_CleanTimer_clicked();
    void on_Button_Kill_clicked();
    void on_CheckBox_SetPos_stateChanged(int state);
    void on_Timer();
    void processStateChanged(QProcess::ProcessState newState);

private:
    void mouseMove(int x,int y);
    void mouseClick(int x,int y);
    void mouseDoubleClick(int x,int y);
    void mouseRightClick(int x,int y);
    void autoAction();
    void delay(qint64 second);
    void startApp();
    void killApp();

protected:
    void keyReleaseEvent(QKeyEvent *event);

private:
    const static int K_CLICKNUMBER=2;
    const static int K_MOUSEEVENTF_MOVE=0x0001;        //鼠标移动
    const static int K_MOUSEEVENTF_LEFTDOWN=0x0002;	 //鼠标左键按下
    const static int K_MOUSEEVENTF_LEFTUP=0x0004;	     //鼠标左键松开
    const static int K_MOUSEEVENTF_RIGHTDOWN=0x0008;  //鼠标右键按下
    const static int K_MOUSEEVENTF_RIGHTUP=0x0010;	//鼠标右键松开
    const static int K_MOUSEEVENTF_MIDDLEDOWN=0x0020;	//鼠标中键按下
    const static int K_MOUSEEVENTF_MIDDLEUP=0x0040;	//鼠标中键松开
    const static int K_MOUSEEVENTF_WHEEL=0x0080;  	//鼠标轮被滚动，如果鼠标有一个滚轮。滚动的数量由dwData给出
    const static int K_MOUSEEVENTF_ABSOLUTE=0x8000;   //The dx and dy parameters contain normalized absolute coordinates

private:
    Ui::Dialog *m_ui;
    int m_clickXs[K_CLICKNUMBER];
    int m_clickYs[K_CLICKNUMBER];
    int m_actTypes[K_CLICKNUMBER];//0:left click,1 double click,2 right click
    QString m_appname;
    QString m_appdir;
    QProcess *m_process;
    QTimer *m_timer;
    bool m_app_running;
    bool m_timer_stop;
};


#endif // DIALOG_H
