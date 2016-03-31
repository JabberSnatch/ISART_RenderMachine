/********************************************************************************
** Form generated from reading UI file 'RenderMachine.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENDERMACHINE_H
#define UI_RENDERMACHINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "Qt_OGL_widget.h"

QT_BEGIN_NAMESPACE

class Ui_RenderMachineClass
{
public:
    QWidget *centralWidget;
    Qt_OGL_Widget *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuYolo;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RenderMachineClass)
    {
        if (RenderMachineClass->objectName().isEmpty())
            RenderMachineClass->setObjectName(QStringLiteral("RenderMachineClass"));
        RenderMachineClass->resize(600, 400);
        centralWidget = new QWidget(RenderMachineClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new Qt_OGL_Widget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 471, 311));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy);
        openGLWidget->setMaximumSize(QSize(16777215, 311));
        RenderMachineClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RenderMachineClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menuYolo = new QMenu(menuBar);
        menuYolo->setObjectName(QStringLiteral("menuYolo"));
        RenderMachineClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RenderMachineClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        RenderMachineClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(RenderMachineClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        RenderMachineClass->setStatusBar(statusBar);

        menuBar->addAction(menuYolo->menuAction());

        retranslateUi(RenderMachineClass);

        QMetaObject::connectSlotsByName(RenderMachineClass);
    } // setupUi

    void retranslateUi(QMainWindow *RenderMachineClass)
    {
        RenderMachineClass->setWindowTitle(QApplication::translate("RenderMachineClass", "RenderMachine", 0));
        menuYolo->setTitle(QApplication::translate("RenderMachineClass", "yolo", 0));
    } // retranslateUi

};

namespace Ui {
    class RenderMachineClass: public Ui_RenderMachineClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENDERMACHINE_H
