#include "AppUI.h"
#include "AppController.h"

#include <QStatusBar>

namespace
{
    QString const & kInitializing = "Initializing Application...";
}

AppUI::AppUI(QWidget *parent)
:
    QMainWindow(parent),
    mAppController( nullptr )
{
  setStatusBar( new QStatusBar() );
  statusBar()->showMessage( kInitializing );
  mAppController = new AppController( *this );
}
