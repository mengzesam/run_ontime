#include "dialog.h"
#include "ui_dialog.h"
#include <QTime>
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Dialog),
    m_appname(QString("")),
    m_appdir(QString("")),
    m_process(new QProcess),
    m_timer(new QTimer),
    m_app_running(false),
    m_timer_setting(false)
{
    m_ui->setupUi(this);
    for(int i=0;i<K_CLICKNUMBER;i++){
        m_ui->ComboBox_ClickNo->addItem(QString("%1").arg(i+1));
        m_actTypes[i]=1;
    }
    m_clickXs[0]=1180;
    m_clickYs[0]=45;
    connect(m_process,&QProcess::stateChanged,this,&Dialog::processStateChanged);
    m_timer->setParent(this);
    connect(m_timer,&QTimer::timeout,this,&Dialog::on_Timer);
    setWindowTitle(tr("@RunOntime"));
}

Dialog::~Dialog()
{
    delete m_process;
    delete m_ui;
}

void Dialog::on_Timer()
{
    m_timer->stop();
    if(m_app_running)
        m_process->kill();
    //delay(1);
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" ontime kill "+m_appname+"\n");
    QFileInfo fileinfo=QFileInfo(m_appdir+m_appname);
    if(!fileinfo.isExecutable())
        return;
    QDir::setCurrent(m_appdir);
    m_process->start(m_appname);
    m_app_running=true;
    int waiting=m_ui->SpinBox_Waiting->value();
    //delay(waiting);
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" ontime start "+m_appname+"\n");
    autoAction();
    m_timer->start();
}

void Dialog::processStateChanged(QProcess::ProcessState newState)
{
    if(newState==QProcess::ProcessState::Running){
        m_app_running=true;
        qDebug()<<"process state:running";
    }else if(newState==QProcess::ProcessState::NotRunning){
        m_app_running=false;
        qDebug()<<"process state:not running";
    }
}

void Dialog::on_Button_SetApp_clicked()
{
    QString file=QFileDialog::getOpenFileName(0, tr("Select appliction to run"),"/",
                               tr("Execute App(*.exe *.bat);;All File(*.*)"));
    if(file.isEmpty() || !QFileInfo::exists(file))
         return;
    QFileInfo fileinfo=QFileInfo(file);
    if(!fileinfo.isExecutable())
        return;
    m_appname=fileinfo.fileName();
    m_appdir=fileinfo.path()+"/";
    QDir::setCurrent(m_appdir);
    m_process->start(m_appname);
    setWindowTitle(m_appname+tr("@RunOntime"));
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" run "+m_appname+"\n");

}

void Dialog::on_Button_StartTimer_clicked()
{
    if(!m_app_running){
        QFileInfo fileinfo=QFileInfo(m_appdir+m_appname);
        if(!fileinfo.isExecutable())
            return;
        QDir::setCurrent(m_appdir);
        m_process->start(m_appname);
        m_app_running=true;
    }
    qint64 runtime=m_ui->SpinBox_RunTime->value();
    m_timer->setInterval(runtime);
    m_timer->start();
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" startTimer "+m_appname+"\n");
}

void Dialog::on_Button_CleanTimer_clicked()
{
    m_timer->stop();
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" stopTimer "+m_appname+"\n");
}

void Dialog::on_Button_Kill_clicked()
{
    if(m_app_running)
        m_process->kill();
    setWindowTitle(tr("@RunOntime"));
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" kill "+m_appname+"\n");
}

void Dialog::on_CheckBox_SetPos_stateChanged(int state)//state:0 unchecked,2 checked
{
    if(state==2)//checked
    {
        m_ui->ComboBox_ClickNo->setEnabled(true);
    }else{
        m_ui->ComboBox_ClickNo->setEnabled(false);
    }
}

void Dialog::mouseMove(int x, int y)
{
    int width=::GetSystemMetrics(SM_CXSCREEN);
    int height=::GetSystemMetrics(SM_CYSCREEN);
    int d100=65535;
    qint64 dx=x*d100/width;
    qint64 dy=y*d100/height;
    ::mouse_event(K_MOUSEEVENTF_ABSOLUTE | K_MOUSEEVENTF_MOVE,dx,dy,0,0);
}

void Dialog::mouseClick(int x, int y)
{
    int width=::GetSystemMetrics(SM_CXSCREEN);
    int height=::GetSystemMetrics(SM_CYSCREEN);
    int d100=65535;
    qint64 dx=x*d100/width;
    qint64 dy=y*d100/height;
    ::mouse_event(K_MOUSEEVENTF_ABSOLUTE | K_MOUSEEVENTF_LEFTDOWN |K_MOUSEEVENTF_LEFTUP,dx,dy,0,0);
}
void Dialog::mouseDoubleClick(int x, int y)
{
    int width=::GetSystemMetrics(SM_CXSCREEN);
    int height=::GetSystemMetrics(SM_CYSCREEN);
    int d100=65535;
    qint64 dx=x*d100/width;
    qint64 dy=y*d100/height;
    ::mouse_event(K_MOUSEEVENTF_ABSOLUTE | K_MOUSEEVENTF_LEFTDOWN |K_MOUSEEVENTF_LEFTUP,dx,dy,0,0);
    ::mouse_event(K_MOUSEEVENTF_ABSOLUTE | K_MOUSEEVENTF_LEFTDOWN |K_MOUSEEVENTF_LEFTUP,dx,dy,0,0);
}

void Dialog::mouseRightClick(int x,int y)
{
    int width=::GetSystemMetrics(SM_CXSCREEN);
    int height=::GetSystemMetrics(SM_CYSCREEN);
    int d100=65535;
    qint64 dx=x*d100/width;
    qint64 dy=y*d100/height;
    ::mouse_event(K_MOUSEEVENTF_ABSOLUTE | K_MOUSEEVENTF_RIGHTDOWN |K_MOUSEEVENTF_RIGHTUP,dx,dy,0,0);
}

void Dialog::autoAction()
{
    qint64 act_interval=m_ui->SpinBox_ClickInterval->value();
    for(int i=0;i<K_CLICKNUMBER;i++){
        if( m_actTypes[i]==0){//left click
            mouseMove(m_clickXs[i],m_clickYs[i]);
            mouseClick(m_clickXs[i],m_clickYs[i]);
        }else if(m_actTypes[i]==1){//double click
            mouseMove(m_clickXs[i],m_clickYs[i]);
            mouseDoubleClick(m_clickXs[i],m_clickYs[i]);
        }else{//right click
            mouseMove(m_clickXs[i],m_clickYs[i]);
            mouseRightClick(m_clickXs[i],m_clickYs[i]);
        }
        if(i<K_CLICKNUMBER-1)
            delay(act_interval);
    }
}

void Dialog::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_F2 && m_ui->CheckBox_SetPos->checkState()==2){
        POINT point;
        ::GetCursorPos(&point);
        int click_index=m_ui->ComboBox_ClickNo->currentIndex();
        m_clickXs[click_index]=point.x;
        m_clickYs[click_index]=point.y;
        m_ui->TextEdit_Info->appendHtml(QString("Click%1 (x,y):(%2,%3)").arg(click_index+1)
                                        .arg(point.x).arg(point.y));

    }else{
        Dialog::keyPressEvent(event);
    }
}

void Dialog::delay(qint64 second)
{
    ::Sleep(second*1000);
}
