#ifndef AppUI_H
#define AppUI_H

#include <QMainWindow>

class AppController;

class CameraWidget;

class AppUI : public QMainWindow
{
    Q_OBJECT
public:
    explicit AppUI(QWidget *parent = 0);
  private:
    AppController* mAppController;
};

#endif // AppUI_H
