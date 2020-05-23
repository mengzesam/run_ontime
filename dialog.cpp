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
    m_timer_stop(true)
{
    m_ui->setupUi(this);
    m_ui->SpinBox_ActionNumber->setMaximum(K_MAXACTION);
    m_clickXs[0]=1180;
    m_clickYs[0]=45;
    m_clickXs[1]=580;
    m_clickYs[1]=155;
    int action_number=m_ui->SpinBox_ActionNumber->value();
    updateActionNumber(action_number);
    setWindowTitle(tr("@RunOntime"));
    connect(m_process,&QProcess::stateChanged,this,&Dialog::processStateChanged);
    m_timer->setParent(this);
    connect(m_timer,&QTimer::timeout,this,&Dialog::on_Timer);
    setFixedSize(480,540);
}

Dialog::~Dialog()
{
    delete m_timer;
    delete m_process;
    delete m_ui;
}

void Dialog::resizeEvent(QResizeEvent *){
    ;
}

void Dialog::on_SpinBox_ActionNumber_valueChanged(int arg1)
{
    updateActionNumber(arg1);
}

void Dialog::updateActionNumber(int action_number)
{
    if(action_number>K_MAXACTION ||action_number<1)
        return;
    int current_number=m_ui->ComboBox_ClickNo->count();
    for(int i=current_number;i>action_number;i--){
        m_ui->ComboBox_ClickNo->removeItem(i-1);
    }
    for(int i=current_number;i<action_number;i++){
        m_ui->ComboBox_ClickNo->addItem(QString("%1").arg(i+1));
        m_actTypes[i]=1;
    }
}

void Dialog::on_Timer()
{
    if(m_timer_stop)
        return;
    if(m_app_running)
        killApp();
    delay(2);
    startApp();
    m_app_running=true;
    autoAction();
    qint64 duration=m_ui->SpinBox_RunTime->value();
    QTimer::singleShot(duration*1000,this,SLOT(on_Timer()));
}

void Dialog::processStateChanged(QProcess::ProcessState newState)
{
    if(newState==QProcess::ProcessState::Running){
        m_app_running=true;
        m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                        +" Running "+m_appname+"\n");
    }else if(newState==QProcess::ProcessState::NotRunning){
        m_app_running=false;
        m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                        +"Not Running "+m_appname+"\n");
    }else{
        ;
    }
}

void Dialog::on_Button_SetApp_clicked()
{
    QString file=QFileDialog::getOpenFileName(0, tr("Select appliction to run"),"/",
                               tr("Execute App(*.exe *.bat);;All File(*.*)"));
    if(file.isEmpty() || !QFileInfo::exists(file))
         return;
    QFileInfo fileinfo=QFileInfo(file);
    m_appname=fileinfo.fileName();
    m_appdir=fileinfo.path()+"/";
    startApp();
    m_app_running=true;
    m_ui->Button_SetApp->setEnabled(false);
    m_ui->Button_StartTimer->setEnabled(true);
    m_ui->Button_CleanTimer->setEnabled(false);
    m_ui->Button_Kill->setEnabled(true);
    setWindowTitle(m_appname+tr("@RunOntime"));
    autoAction();
}

void Dialog::on_Button_StartTimer_clicked()
{
    if(!m_app_running){
        startApp();
        m_app_running=true;
    }
    m_timer_stop=false;
    m_ui->Button_SetApp->setEnabled(false);
    m_ui->Button_StartTimer->setEnabled(false);
    m_ui->Button_CleanTimer->setEnabled(true);
    m_ui->Button_Kill->setEnabled(true);
    qint64 duration=m_ui->SpinBox_RunTime->value();
    QTimer::singleShot(duration*1000,this,SLOT(on_Timer()));
}

void Dialog::on_Button_CleanTimer_clicked()
{
    m_timer_stop=true;
    m_ui->Button_SetApp->setEnabled(false);
    m_ui->Button_StartTimer->setEnabled(true);
    m_ui->Button_CleanTimer->setEnabled(false);
    m_ui->Button_Kill->setEnabled(true);
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" stopTimer "+m_appname+"\n");
}

void Dialog::on_Button_Kill_clicked()
{
    if(m_app_running)
        killApp();
    m_app_running=false;
    m_timer_stop=true;
    m_ui->Button_SetApp->setEnabled(true);
    m_ui->Button_StartTimer->setEnabled(false);
    m_ui->Button_CleanTimer->setEnabled(false);
    m_ui->Button_Kill->setEnabled(false);
    setWindowTitle(tr("@RunOntime"));
}

void Dialog::on_CheckBox_SetPos_stateChanged(int state)//state:0 unchecked,2 checked
{
    if(state==2)//checked
    {
        m_ui->ComboBox_ClickNo->setEnabled(true);
        m_ui->SpinBox_ActionNumber->setEnabled(true);
    }else{
        m_ui->ComboBox_ClickNo->setEnabled(false);
        m_ui->SpinBox_ActionNumber->setEnabled(false);
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
    int act_interval=m_ui->SpinBox_ClickInterval->value();
    int waiting=m_ui->SpinBox_Waiting->value();
    delay(waiting);
    int action_number=m_ui->ComboBox_ClickNo->count();
    for(int i=0;i<action_number;i++){
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
        if(i<action_number-1)
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

void Dialog::startApp()
{
    QFileInfo fileinfo=QFileInfo(m_appdir+m_appname);
    if(!fileinfo.isExecutable())
        return;
    //m_process->start(m_appname);
    QDir::setCurrent(m_appdir);
    ::ShellExecuteW(0,QString("open").toStdWString().c_str(),m_appname.toStdWString().c_str(),
                    QString("").toStdWString().c_str(),m_appdir.toStdWString().c_str(),1);
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" start "+m_appname+"\n");

}

void Dialog::killApp()
{
    //::system(QString("taskkill /F /IM "+m_appname).toLatin1());
   ::ShellExecuteW(0,QString("open").toStdWString().c_str(),QString("taskkill").toStdWString().c_str(),
                  QString(" /F /IM "+m_appname).toStdWString().c_str(),m_appdir.toStdWString().c_str(),0);
    m_ui->TextEdit_Info->appendHtml((QTime::currentTime()).toString("hh:mm:ss")
                                    +" kill "+m_appname+"\n");
}
