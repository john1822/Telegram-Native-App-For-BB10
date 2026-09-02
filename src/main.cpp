#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <QLocale>
#include <QTranslator>

#include "DiagnosticController.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv) {
    Application app(argc, argv);

    // Instantiate native diagnostic controller
    Telegram::Controllers::DiagnosticController diagnosticController;

    // Load QML Document
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(&app);
    qml->setContextProperty("diagnostic", &diagnosticController);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app.setScene(root);

    // Automatically initiate MTProto 2.0 connection
    diagnosticController.startConnection();

    return Application::exec();
}
