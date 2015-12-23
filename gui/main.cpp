#include "app.h"
#include "mainwin.h"

int main(int argc, char *argv[]) {
    App app(argc, argv);

    MainWin mainWin;
    mainWin.show();

    return app.exec();
}

// eof
