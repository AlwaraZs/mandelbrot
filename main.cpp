#include "RenderWindow.h"
#include <qapplication.h>
#include <qpushbutton.h>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    RenderWindow window{};
    window.show();
    return a.exec();
}