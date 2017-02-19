/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef APPDEBUGMESSAGEHANDLER_H
#define APPDEBUGMESSAGEHANDLER_H

#include <cstdlib>
#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QMessageLogContext>
#include <QObject>
#include <QRegularExpression>
#include <QString>

/*
 * Custom debug/message handler class to work in conjunction with qDebug() family of functions.
 * Formats all stderr output generated by the application in a consistent manner.
 * It can also be used by other event listeners to intercept messages they may be interested in (connect to messageOutput() signal).
 *
 * Include this header to use qInfo() safely with Qt < v5.5 (qInfo becomes alias for qDebug).
 *
 * Note that the default message format can still be overriden by setting QT_MESSAGE_PATTERN environment variable at _runtime_.
 *   see: https://doc.qt.io/qt-5/qtglobal.html#qSetMessagePattern
 *
 * This is an app-wide "global" thread-safe singleton class, use it with AppDebugMessageHandler::instance().
 *   For example, at start of application:
 *
 *     QApplication app(argc, argv);
 *     AppDebugMessageHandler::instance()->installAppMessageHandler();
 *
 *  To connect to a signal:
 *
 *    connect(AppDebugMessageHandler::instance(), &AppDebugMessageHandler::messageOutput, this, &DebugOutput::onAppDebugMessage);
 *
 * This singleton will self-destruct upon QApplication termination.
 *
 */

#if defined(QT_DEBUG) || defined(DEBUG)
  #define APP_DBG_HANDLER_IS_DBG_BUILD          1
#else
  #define APP_DBG_HANDLER_IS_DBG_BUILD          0
#endif

// Enable/disable this custom handler handler entirely
#ifndef APP_DBG_HANDLER_ENABLE
  #define APP_DBG_HANDLER_ENABLE                1
#endif

// Default log level (can also be set with setAppDebugOutputLevel()):
//   0 = debug+; 1 = info+; 2 = warn+; 3 = critical+; 4 = fatal only
#ifndef APP_DBG_HANDLER_DEFAULT_LEVEL
  #define APP_DBG_HANDLER_DEFAULT_LEVEL         0
#endif

// Include source file path/name in output (filtered with APP_DBG_HANDLER_SRC_PATH)
#ifndef APP_DBG_HANDLER_SHOW_SRC_PATH
  #define APP_DBG_HANDLER_SHOW_SRC_PATH         APP_DBG_HANDLER_IS_DBG_BUILD
#endif

// Show just the function Class::name vs. full declaration with return and param types
#ifndef APP_DBG_HANDLER_SHOW_FUNCTION_DECL
  #define APP_DBG_HANDLER_SHOW_FUNCTION_DECL    0
#endif

// base path for source file name filter, everything after this is kept; RegEx, non-greedy
#ifndef APP_DBG_HANDLER_SRC_PATH
  #define APP_DBG_HANDLER_SRC_PATH              ".*src"
#endif

// Make sure to include this header if using qInfo()
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
  #if defined(QT_NO_INFO_OUTPUT)
    #define qInfo      QT_NO_QDEBUG_MACRO
  #else
    #define qInfo      qDebug
  #endif
  #define QtInfoMsg    4
#endif

class AppDebugMessageHandler : public QObject
{
  Q_OBJECT

  public:
    // This class implements the singleton design pattern and is therefore publicly accessible only through instance().
    static AppDebugMessageHandler * instance();
    AppDebugMessageHandler(AppDebugMessageHandler const &) = delete;
    void operator = (AppDebugMessageHandler const &)       = delete;

    void setAppDebugOutputLevel(const quint8 & appDebugOutputLevel);
    void setShowSourcePath(bool showSourcePath);
    void setShowFunctionDeclarations(bool showFunctionDeclarations);

    void installAppMessageHandler();
    void messageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg);

  private:
    // This class implements the singleton design pattern and therefore has only a private constructor.
    explicit AppDebugMessageHandler(QObject * parent = Q_NULLPTR);
    ~AppDebugMessageHandler() { }

    QtMessageHandler m_defaultHandler;
    QRegularExpression m_srcPathFilter;
    QRegularExpression m_functionFilter;
    quint8 m_appDebugOutputLevel;
    bool m_showSourcePath;
    bool m_showFunctionDeclarations;

  signals:
    void messageOutput(quint8 level, const QString & msg, const QMessageLogContext & context);
};

// Message handler which is installed using qInstallMessageHandler. This needs to be global.
// Use AppDebugMessageHandler::instance()->installAppMessageHandler();
//  instead of installing this function directly (it will verify this handler is enabled, etc).
void g_appDebugMessageHandler(QtMsgType, const QMessageLogContext &, const QString &);

#endif // APPDEBUGMESSAGEHANDLER_H
