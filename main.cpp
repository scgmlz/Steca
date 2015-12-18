#include "app.h"
#include "mainwin.h"

int main(int argc, char *argv[])
{
    App a(argc, argv);
    MainWin w;
    w.show();

    return a.exec();
}
