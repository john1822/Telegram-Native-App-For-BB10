#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <QLocale>
#include <QTranslator>

#include "core/MTProtoSession.h"
#include "storage/SessionStorage.h"
#include "controllers/DiagnosticController.h"
#include "controllers/AuthController.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv) {
    Application app(argc, argv);

    // Core session and persistent storage
    Telegram::Core::MTProtoSession session;
    Telegram::Storage::SessionStorage sessionStorage;

    // Controllers
    Telegram::Controllers::DiagnosticController diagnosticController(&session);
    Telegram::Controllers::AuthController authController(&session, &sessionStorage);

    // Load QML Document
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(&app);
    qml->setContextProperty("diagnostic", &diagnosticController);
    qml->setContextProperty("auth", &authController);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app.setScene(root);

    // Start Authentication & MTProto Session Lifecycle
    authController.start();

    return Application::exec();
}
