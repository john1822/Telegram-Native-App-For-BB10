#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QDeclarativeError>

#include "core/MTProtoSession.h"
#include "storage/SessionStorage.h"
#include "controllers/DiagnosticController.h"
#include "controllers/AuthController.h"
#include "controllers/ChatListController.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv) {
    Application app(argc, argv);

    // Core session and persistent storage
    Telegram::Core::MTProtoSession session;
    Telegram::Storage::SessionStorage sessionStorage;

    // Controllers
    Telegram::Controllers::DiagnosticController diagnosticController(&session);
    Telegram::Controllers::AuthController authController(&session, &sessionStorage);
    Telegram::Controllers::ChatListController chatListController(&session, &sessionStorage);

    // Load QML Document
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(&app);
    qml->setContextProperty("diagnostic", &diagnosticController);
    qml->setContextProperty("auth", &authController);
    qml->setContextProperty("chatList", &chatListController);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    if (!root || qml->hasErrors()) {
        QFile errFile("data/app_log.txt");
        if (errFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&errFile);
            out << "[QML ERROR] Failed to load main.qml root object:\n";
            QList<QDeclarativeError> errs = qml->errors();
            for (int i = 0; i < errs.size(); ++i) {
                out << "   Line " << errs.at(i).line() << ": " << errs.at(i).description() << "\n";
            }
            errFile.close();
        }
    }
    app.setScene(root);

    // Start Authentication & MTProto Session Lifecycle
    authController.start();

    return Application::exec();
}
