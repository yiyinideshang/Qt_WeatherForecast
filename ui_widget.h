/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QWidget *widget_5;
    QLabel *lblHighCurve;
    QLabel *lblLowCurve;
    QWidget *widget_3;
    QWidget *widget_10;
    QGridLayout *gridLayout_5;
    QLabel *lblWeek0;
    QLabel *lblWeek1;
    QLabel *lblWeek2;
    QLabel *lblWeek3;
    QLabel *lblWeek4;
    QLabel *lblWeek5;
    QLabel *lblDate0;
    QLabel *lblDate1;
    QLabel *lblDate2;
    QLabel *lblDate3;
    QLabel *lblDate4;
    QLabel *lblDate5;
    QWidget *widget_11;
    QGridLayout *gridLayout_6;
    QLabel *lblTypeIcon0;
    QLabel *lblTypeIcon1;
    QLabel *lblTypeIcon2;
    QLabel *lblTypeIcon3;
    QLabel *lblTypeIcon4;
    QLabel *lblTypeIcon5;
    QLabel *lblType0;
    QLabel *lblType1;
    QLabel *lblType2;
    QLabel *lblType3;
    QLabel *lblType4;
    QLabel *lblType5;
    QWidget *widget_12;
    QHBoxLayout *horizontalLayout_4;
    QLabel *lblquality0;
    QLabel *lblquality1;
    QLabel *lblquality2;
    QLabel *lblquality3;
    QLabel *lblquality4;
    QLabel *lblquality5;
    QWidget *widget_2;
    QLabel *lblTemp;
    QLabel *lblCity;
    QLabel *lblTypeIcon;
    QLabel *lblType;
    QLabel *lblLowHigh;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *lblDate;
    QWidget *widget_4;
    QGridLayout *gridLayout;
    QLabel *lblQuality;
    QLabel *label_105;
    QLabel *label_104;
    QLabel *lblPM25;
    QLabel *lblWindFl;
    QLabel *lblShidu;
    QLabel *lblWindFx;
    QLabel *label_103;
    QLabel *label_23;
    QLabel *label_16;
    QLabel *label_22;
    QLabel *label_24;
    QLabel *lblGanmao;
    QLabel *lblNotice;
    QLabel *lblPM10;
    QLabel *lblSunrise;
    QLabel *lblSunset;
    QLabel *lblUpdateTime;
    QWidget *widget_6;
    QGridLayout *gridLayout_8;
    QLabel *lblFX0;
    QLabel *lblFX1;
    QLabel *lblFX2;
    QLabel *lblFX3;
    QLabel *lblFX4;
    QLabel *lblFX5;
    QLabel *lblFl0;
    QLabel *lblFl1;
    QLabel *lblFl2;
    QLabel *lblFl3;
    QLabel *lblFl4;
    QLabel *lblFl5;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(1000, 600);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Widget->sizePolicy().hasHeightForWidth());
        Widget->setSizePolicy(sizePolicy);
        Widget->setStyleSheet(QString::fromUtf8("#Widget{\n"
"		border-image:	url(:/res/weather_bg.jpg)\n"
"}\n"
"QWidget {\n"
"    color: white;\n"
"}\n"
"QLabel {\n"
"    border-radius: 10px;            /* \345\234\206\350\247\222\345\215\212\345\276\204 */\n"
"    /* \346\210\226\350\200\205\345\246\202\346\236\234\344\275\240\351\234\200\350\246\201\350\276\271\346\241\206\357\274\232 */\n"
"    /* border: 1px solid gray; */\n"
"}"));
        widget_5 = new QWidget(Widget);
        widget_5->setObjectName("widget_5");
        widget_5->setGeometry(QRect(640, 310, 361, 151));
        lblHighCurve = new QLabel(widget_5);
        lblHighCurve->setObjectName("lblHighCurve");
        lblHighCurve->setGeometry(QRect(0, 0, 351, 81));
        lblLowCurve = new QLabel(widget_5);
        lblLowCurve->setObjectName("lblLowCurve");
        lblLowCurve->setGeometry(QRect(0, 70, 361, 81));
        widget_3 = new QWidget(Widget);
        widget_3->setObjectName("widget_3");
        widget_3->setGeometry(QRect(640, 80, 351, 221));
        widget_10 = new QWidget(widget_3);
        widget_10->setObjectName("widget_10");
        widget_10->setGeometry(QRect(-10, 0, 361, 61));
        gridLayout_5 = new QGridLayout(widget_10);
        gridLayout_5->setObjectName("gridLayout_5");
        lblWeek0 = new QLabel(widget_10);
        lblWeek0->setObjectName("lblWeek0");

        gridLayout_5->addWidget(lblWeek0, 0, 0, 1, 1);

        lblWeek1 = new QLabel(widget_10);
        lblWeek1->setObjectName("lblWeek1");

        gridLayout_5->addWidget(lblWeek1, 0, 1, 1, 1);

        lblWeek2 = new QLabel(widget_10);
        lblWeek2->setObjectName("lblWeek2");

        gridLayout_5->addWidget(lblWeek2, 0, 2, 1, 1);

        lblWeek3 = new QLabel(widget_10);
        lblWeek3->setObjectName("lblWeek3");

        gridLayout_5->addWidget(lblWeek3, 0, 3, 1, 1);

        lblWeek4 = new QLabel(widget_10);
        lblWeek4->setObjectName("lblWeek4");

        gridLayout_5->addWidget(lblWeek4, 0, 4, 1, 1);

        lblWeek5 = new QLabel(widget_10);
        lblWeek5->setObjectName("lblWeek5");

        gridLayout_5->addWidget(lblWeek5, 0, 5, 1, 1);

        lblDate0 = new QLabel(widget_10);
        lblDate0->setObjectName("lblDate0");

        gridLayout_5->addWidget(lblDate0, 1, 0, 1, 1);

        lblDate1 = new QLabel(widget_10);
        lblDate1->setObjectName("lblDate1");

        gridLayout_5->addWidget(lblDate1, 1, 1, 1, 1);

        lblDate2 = new QLabel(widget_10);
        lblDate2->setObjectName("lblDate2");

        gridLayout_5->addWidget(lblDate2, 1, 2, 1, 1);

        lblDate3 = new QLabel(widget_10);
        lblDate3->setObjectName("lblDate3");

        gridLayout_5->addWidget(lblDate3, 1, 3, 1, 1);

        lblDate4 = new QLabel(widget_10);
        lblDate4->setObjectName("lblDate4");

        gridLayout_5->addWidget(lblDate4, 1, 4, 1, 1);

        lblDate5 = new QLabel(widget_10);
        lblDate5->setObjectName("lblDate5");

        gridLayout_5->addWidget(lblDate5, 1, 5, 1, 1);

        widget_11 = new QWidget(widget_3);
        widget_11->setObjectName("widget_11");
        widget_11->setGeometry(QRect(-10, 60, 361, 81));
        gridLayout_6 = new QGridLayout(widget_11);
        gridLayout_6->setObjectName("gridLayout_6");
        lblTypeIcon0 = new QLabel(widget_11);
        lblTypeIcon0->setObjectName("lblTypeIcon0");
        lblTypeIcon0->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/tq-1.png);"));
        lblTypeIcon0->setScaledContents(true);

        gridLayout_6->addWidget(lblTypeIcon0, 0, 0, 1, 1);

        lblTypeIcon1 = new QLabel(widget_11);
        lblTypeIcon1->setObjectName("lblTypeIcon1");
        lblTypeIcon1->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/xiaoyu.png);"));
        lblTypeIcon1->setScaledContents(true);

        gridLayout_6->addWidget(lblTypeIcon1, 0, 1, 1, 1);

        lblTypeIcon2 = new QLabel(widget_11);
        lblTypeIcon2->setObjectName("lblTypeIcon2");
        lblTypeIcon2->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/duoyun.png);"));
        lblTypeIcon2->setScaledContents(true);

        gridLayout_6->addWidget(lblTypeIcon2, 0, 2, 1, 1);

        lblTypeIcon3 = new QLabel(widget_11);
        lblTypeIcon3->setObjectName("lblTypeIcon3");
        lblTypeIcon3->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/duoyun.png);"));
        lblTypeIcon3->setScaledContents(true);

        gridLayout_6->addWidget(lblTypeIcon3, 0, 3, 1, 1);

        lblTypeIcon4 = new QLabel(widget_11);
        lblTypeIcon4->setObjectName("lblTypeIcon4");
        lblTypeIcon4->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/zhongyu.png);"));
        lblTypeIcon4->setScaledContents(true);

        gridLayout_6->addWidget(lblTypeIcon4, 0, 4, 1, 1);

        lblTypeIcon5 = new QLabel(widget_11);
        lblTypeIcon5->setObjectName("lblTypeIcon5");
        lblTypeIcon5->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/baoyu.png);"));
        lblTypeIcon5->setScaledContents(true);

        gridLayout_6->addWidget(lblTypeIcon5, 0, 5, 1, 1);

        lblType0 = new QLabel(widget_11);
        lblType0->setObjectName("lblType0");

        gridLayout_6->addWidget(lblType0, 1, 0, 1, 1);

        lblType1 = new QLabel(widget_11);
        lblType1->setObjectName("lblType1");

        gridLayout_6->addWidget(lblType1, 1, 1, 1, 1);

        lblType2 = new QLabel(widget_11);
        lblType2->setObjectName("lblType2");

        gridLayout_6->addWidget(lblType2, 1, 2, 1, 1);

        lblType3 = new QLabel(widget_11);
        lblType3->setObjectName("lblType3");

        gridLayout_6->addWidget(lblType3, 1, 3, 1, 1);

        lblType4 = new QLabel(widget_11);
        lblType4->setObjectName("lblType4");

        gridLayout_6->addWidget(lblType4, 1, 4, 1, 1);

        lblType5 = new QLabel(widget_11);
        lblType5->setObjectName("lblType5");

        gridLayout_6->addWidget(lblType5, 1, 5, 1, 1);

        widget_12 = new QWidget(widget_3);
        widget_12->setObjectName("widget_12");
        widget_12->setGeometry(QRect(-10, 150, 361, 61));
        horizontalLayout_4 = new QHBoxLayout(widget_12);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        lblquality0 = new QLabel(widget_12);
        lblquality0->setObjectName("lblquality0");
        lblquality0->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 0);"));

        horizontalLayout_4->addWidget(lblquality0);

        lblquality1 = new QLabel(widget_12);
        lblquality1->setObjectName("lblquality1");
        lblquality1->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 255);"));

        horizontalLayout_4->addWidget(lblquality1);

        lblquality2 = new QLabel(widget_12);
        lblquality2->setObjectName("lblquality2");
        lblquality2->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 170, 0);"));

        horizontalLayout_4->addWidget(lblquality2);

        lblquality3 = new QLabel(widget_12);
        lblquality3->setObjectName("lblquality3");
        lblquality3->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 85, 127);"));

        horizontalLayout_4->addWidget(lblquality3);

        lblquality4 = new QLabel(widget_12);
        lblquality4->setObjectName("lblquality4");
        lblquality4->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));

        horizontalLayout_4->addWidget(lblquality4);

        lblquality5 = new QLabel(widget_12);
        lblquality5->setObjectName("lblquality5");
        lblquality5->setStyleSheet(QString::fromUtf8("background-color: rgb(170, 0, 0);"));

        horizontalLayout_4->addWidget(lblquality5);

        widget_2 = new QWidget(Widget);
        widget_2->setObjectName("widget_2");
        widget_2->setGeometry(QRect(10, 80, 411, 121));
        lblTemp = new QLabel(widget_2);
        lblTemp->setObjectName("lblTemp");
        lblTemp->setGeometry(QRect(120, 0, 141, 91));
        lblCity = new QLabel(widget_2);
        lblCity->setObjectName("lblCity");
        lblCity->setGeometry(QRect(250, 0, 161, 61));
        lblTypeIcon = new QLabel(widget_2);
        lblTypeIcon->setObjectName("lblTypeIcon");
        lblTypeIcon->setGeometry(QRect(0, 0, 111, 111));
        lblTypeIcon->setStyleSheet(QString::fromUtf8("border-image: url(:/res/type/tq-1.png);"));
        lblTypeIcon->setScaledContents(true);
        lblType = new QLabel(widget_2);
        lblType->setObjectName("lblType");
        lblType->setGeometry(QRect(120, 90, 91, 31));
        lblLowHigh = new QLabel(widget_2);
        lblLowHigh->setObjectName("lblLowHigh");
        lblLowHigh->setGeometry(QRect(230, 90, 101, 31));
        widget = new QWidget(Widget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(0, 0, 981, 51));
        widget->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        horizontalLayout_3 = new QHBoxLayout(widget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lineEdit_2 = new QLineEdit(widget);
        lineEdit_2->setObjectName("lineEdit_2");
        sizePolicy.setHeightForWidth(lineEdit_2->sizePolicy().hasHeightForWidth());
        lineEdit_2->setSizePolicy(sizePolicy);
        lineEdit_2->setMinimumSize(QSize(300, 0));
        lineEdit_2->setMaximumSize(QSize(300, 16777215));
        lineEdit_2->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n"
"QLineEdit::placeholder { color: rgb(140, 140, 140); };\n"
"QLineEdit{border-radius: 15px;}"));

        horizontalLayout_3->addWidget(lineEdit_2);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName("pushButton_2");
        sizePolicy.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy);
        pushButton_2->setMinimumSize(QSize(30, 30));
        pushButton_2->setMaximumSize(QSize(30, 30));
        pushButton_2->setStyleSheet(QString::fromUtf8("border-image: url(:/res/search.png);"));
        pushButton_2->setAutoDefault(false);
        pushButton_2->setFlat(false);

        horizontalLayout_3->addWidget(pushButton_2);

        horizontalSpacer_2 = new QSpacerItem(50, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        lblDate = new QLabel(widget);
        lblDate->setObjectName("lblDate");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblDate->sizePolicy().hasHeightForWidth());
        lblDate->setSizePolicy(sizePolicy1);
        lblDate->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
""));
        lblDate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(lblDate);

        widget_4 = new QWidget(Widget);
        widget_4->setObjectName("widget_4");
        widget_4->setGeometry(QRect(20, 420, 421, 161));
        widget_4->setStyleSheet(QString::fromUtf8("QWidget { background-color:rgb(170, 170, 255);; border-radius: 10px; }\n"
""));
        gridLayout = new QGridLayout(widget_4);
        gridLayout->setObjectName("gridLayout");
        lblQuality = new QLabel(widget_4);
        lblQuality->setObjectName("lblQuality");

        gridLayout->addWidget(lblQuality, 3, 3, 1, 1);

        label_105 = new QLabel(widget_4);
        label_105->setObjectName("label_105");

        gridLayout->addWidget(label_105, 2, 3, 1, 1);

        label_104 = new QLabel(widget_4);
        label_104->setObjectName("label_104");

        gridLayout->addWidget(label_104, 2, 1, 1, 1);

        lblPM25 = new QLabel(widget_4);
        lblPM25->setObjectName("lblPM25");

        gridLayout->addWidget(lblPM25, 1, 3, 1, 1);

        lblWindFl = new QLabel(widget_4);
        lblWindFl->setObjectName("lblWindFl");

        gridLayout->addWidget(lblWindFl, 1, 1, 1, 1);

        lblShidu = new QLabel(widget_4);
        lblShidu->setObjectName("lblShidu");

        gridLayout->addWidget(lblShidu, 3, 1, 1, 1);

        lblWindFx = new QLabel(widget_4);
        lblWindFx->setObjectName("lblWindFx");

        gridLayout->addWidget(lblWindFx, 0, 1, 1, 1);

        label_103 = new QLabel(widget_4);
        label_103->setObjectName("label_103");

        gridLayout->addWidget(label_103, 0, 3, 1, 1);

        label_23 = new QLabel(widget_4);
        label_23->setObjectName("label_23");
        label_23->setStyleSheet(QString::fromUtf8("border-image: url(:/res/humidity.png);"));

        gridLayout->addWidget(label_23, 2, 0, 2, 1);

        label_16 = new QLabel(widget_4);
        label_16->setObjectName("label_16");
        label_16->setStyleSheet(QString::fromUtf8("border-image: url(:/res/wind.png);"));

        gridLayout->addWidget(label_16, 0, 0, 2, 1);

        label_22 = new QLabel(widget_4);
        label_22->setObjectName("label_22");
        label_22->setStyleSheet(QString::fromUtf8("border-image: url(:/res/pm2.5.png);"));

        gridLayout->addWidget(label_22, 0, 2, 2, 1);

        label_24 = new QLabel(widget_4);
        label_24->setObjectName("label_24");
        label_24->setStyleSheet(QString::fromUtf8("border-image: url(:/res/kognqiquality.png);"));

        gridLayout->addWidget(label_24, 2, 2, 2, 1);

        lblGanmao = new QLabel(Widget);
        lblGanmao->setObjectName("lblGanmao");
        lblGanmao->setGeometry(QRect(10, 210, 411, 55));
        lblGanmao->setStyleSheet(QString::fromUtf8("font-size:16pt;"));
        lblGanmao->setWordWrap(true);
        lblNotice = new QLabel(Widget);
        lblNotice->setObjectName("lblNotice");
        lblNotice->setGeometry(QRect(10, 310, 411, 35));
        lblNotice->setStyleSheet(QString::fromUtf8("font-size:14pt;"));
        lblNotice->setWordWrap(true);
        lblPM10 = new QLabel(Widget);
        lblPM10->setObjectName("lblPM10");
        lblPM10->setGeometry(QRect(10, 365, 130, 25));
        lblPM10->setStyleSheet(QString::fromUtf8("font-size:11pt; color: #e0e0e0;"));
        lblSunrise = new QLabel(Widget);
        lblSunrise->setObjectName("lblSunrise");
        lblSunrise->setGeometry(QRect(150, 365, 120, 25));
        lblSunrise->setStyleSheet(QString::fromUtf8("font-size:11pt; color: #e0e0e0;"));
        lblSunset = new QLabel(Widget);
        lblSunset->setObjectName("lblSunset");
        lblSunset->setGeometry(QRect(280, 365, 130, 25));
        lblSunset->setStyleSheet(QString::fromUtf8("font-size:11pt; color: #e0e0e0;"));
        lblUpdateTime = new QLabel(Widget);
        lblUpdateTime->setObjectName("lblUpdateTime");
        lblUpdateTime->setGeometry(QRect(750, 583, 230, 16));
        lblUpdateTime->setStyleSheet(QString::fromUtf8("font-size:10pt; color: #e0e0e0;"));
        widget_6 = new QWidget(Widget);
        widget_6->setObjectName("widget_6");
        widget_6->setGeometry(QRect(630, 480, 361, 101));
        widget_6->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    border-radius: 8px;            /* \345\234\206\350\247\222\345\215\212\345\276\204 */\n"
"	background-color: rgb(128, 128, 128);\n"
"    /* \346\210\226\350\200\205\345\246\202\346\236\234\344\275\240\351\234\200\350\246\201\350\276\271\346\241\206\357\274\232 */\n"
"    /* border: 1px solid gray; */\n"
"}"));
        gridLayout_8 = new QGridLayout(widget_6);
        gridLayout_8->setObjectName("gridLayout_8");
        lblFX0 = new QLabel(widget_6);
        lblFX0->setObjectName("lblFX0");

        gridLayout_8->addWidget(lblFX0, 0, 0, 1, 1);

        lblFX1 = new QLabel(widget_6);
        lblFX1->setObjectName("lblFX1");

        gridLayout_8->addWidget(lblFX1, 0, 1, 1, 1);

        lblFX2 = new QLabel(widget_6);
        lblFX2->setObjectName("lblFX2");

        gridLayout_8->addWidget(lblFX2, 0, 2, 1, 1);

        lblFX3 = new QLabel(widget_6);
        lblFX3->setObjectName("lblFX3");

        gridLayout_8->addWidget(lblFX3, 0, 3, 1, 1);

        lblFX4 = new QLabel(widget_6);
        lblFX4->setObjectName("lblFX4");

        gridLayout_8->addWidget(lblFX4, 0, 4, 1, 1);

        lblFX5 = new QLabel(widget_6);
        lblFX5->setObjectName("lblFX5");

        gridLayout_8->addWidget(lblFX5, 0, 5, 1, 1);

        lblFl0 = new QLabel(widget_6);
        lblFl0->setObjectName("lblFl0");

        gridLayout_8->addWidget(lblFl0, 1, 0, 1, 1);

        lblFl1 = new QLabel(widget_6);
        lblFl1->setObjectName("lblFl1");

        gridLayout_8->addWidget(lblFl1, 1, 1, 1, 1);

        lblFl2 = new QLabel(widget_6);
        lblFl2->setObjectName("lblFl2");

        gridLayout_8->addWidget(lblFl2, 1, 2, 1, 1);

        lblFl3 = new QLabel(widget_6);
        lblFl3->setObjectName("lblFl3");

        gridLayout_8->addWidget(lblFl3, 1, 3, 1, 1);

        lblFl4 = new QLabel(widget_6);
        lblFl4->setObjectName("lblFl4");

        gridLayout_8->addWidget(lblFl4, 1, 4, 1, 1);

        lblFl5 = new QLabel(widget_6);
        lblFl5->setObjectName("lblFl5");

        gridLayout_8->addWidget(lblFl5, 1, 5, 1, 1);


        retranslateUi(Widget);

        pushButton_2->setDefault(true);


        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Form", nullptr));
        lblHighCurve->setText(QString());
        lblLowCurve->setText(QString());
        lblWeek0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\346\230\250\345\244\251</span></p></body></html>", nullptr));
        lblWeek1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\273\212\345\244\251</span></p></body></html>", nullptr));
        lblWeek2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\346\230\216\345\244\251</span></p></body></html>", nullptr));
        lblWeek3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\345\221\250\345\233\233</span></p></body></html>", nullptr));
        lblWeek4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\345\221\250\344\272\224</span></p></body></html>", nullptr));
        lblWeek5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\345\221\250\345\205\255</span></p></body></html>", nullptr));
        lblDate0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">01/01</span></p></body></html>", nullptr));
        lblDate1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">01/02</span></p></body></html>", nullptr));
        lblDate2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">01/03</span></p></body></html>", nullptr));
        lblDate3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">01/04</span></p></body></html>", nullptr));
        lblDate4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">01/05</span></p></body></html>", nullptr));
        lblDate5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">01/06</span></p></body></html>", nullptr));
        lblTypeIcon0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
        lblTypeIcon1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
        lblTypeIcon2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
        lblTypeIcon3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
        lblTypeIcon4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
        lblTypeIcon5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><br/></p></body></html>", nullptr));
        lblType0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\346\231\264</span></p></body></html>", nullptr));
        lblType1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\345\260\217\351\233\250</span></p></body></html>", nullptr));
        lblType2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\345\244\232\344\272\221</span></p></body></html>", nullptr));
        lblType3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\345\244\232\344\272\221</span></p></body></html>", nullptr));
        lblType4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\270\255\351\233\250</span></p></body></html>", nullptr));
        lblType5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\346\232\264\351\233\250</span></p></body></html>", nullptr));
        lblquality0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\274\230</span></p></body></html>", nullptr));
        lblquality1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\211\257</span></p></body></html>", nullptr));
        lblquality2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\275\273\345\272\246</span></p></body></html>", nullptr));
        lblquality3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\270\255\345\272\246</span></p></body></html>", nullptr));
        lblquality4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\351\207\215\345\272\246</span></p></body></html>", nullptr));
        lblquality5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\270\245\351\207\215</span></p></body></html>", nullptr));
        lblTemp->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:48pt; font-weight:700;\">32\342\204\203</span></p></body></html>", nullptr));
        lblCity->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:18pt;\">\345\214\227\344\272\254</span></p></body></html>", nullptr));
        lblTypeIcon->setText(QString());
        lblType->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:14pt;\">\346\231\264\350\275\254\345\244\232\344\272\221</span></p></body></html>", nullptr));
        lblLowHigh->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:14pt;\">19\342\204\203~32\342\204\203</span></p></body></html>", nullptr));
        lineEdit_2->setText(QString());
        lineEdit_2->setPlaceholderText(QCoreApplication::translate("Widget", "\350\257\267\350\276\223\345\205\245\345\237\216\345\270\202\345\220\215\347\247\260...", nullptr));
        pushButton_2->setText(QString());
        lblDate->setText(QCoreApplication::translate("Widget", "<html><head/><body><p><span style=\" font-size:16pt; font-weight:700;\">2026/05/21 \346\230\237\346\234\237\344\270\211</span></p></body></html>", nullptr));
        lblQuality->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\274\230</span></p></body></html>", nullptr));
        label_105->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\347\251\272\346\260\224\350\264\250\351\207\217</span></p></body></html>", nullptr));
        label_104->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\346\271\277\345\272\246</span></p></body></html>", nullptr));
        lblPM25->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">10</span></p></body></html>", nullptr));
        lblWindFl->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">3\347\272\247</span></p></body></html>", nullptr));
        lblShidu->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">60%</span></p></body></html>", nullptr));
        lblWindFx->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\245\277\345\214\227\351\243\216</span></p></body></html>", nullptr));
        label_103->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">PM2.5</span></p></body></html>", nullptr));
        label_23->setText(QString());
        label_16->setText(QString());
        label_22->setText(QString());
        label_24->setText(QString());
        lblGanmao->setText(QCoreApplication::translate("Widget", "\346\204\237\345\206\222\346\214\207\346\225\260: --", nullptr));
        lblNotice->setText(QCoreApplication::translate("Widget", "\346\270\251\351\246\250\346\217\220\347\244\272: --", nullptr));
        lblPM10->setText(QCoreApplication::translate("Widget", "PM10: --", nullptr));
        lblSunrise->setText(QCoreApplication::translate("Widget", "\346\227\245\345\207\272 --:--", nullptr));
        lblSunset->setText(QCoreApplication::translate("Widget", "\346\227\245\350\220\275 --:--", nullptr));
        lblUpdateTime->setText(QCoreApplication::translate("Widget", "\346\225\260\346\215\256\346\233\264\346\226\260: --:--", nullptr));
        lblFX0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\270\234\351\243\216</span></p></body></html>", nullptr));
        lblFX1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\344\270\234\345\214\227\351\243\216</span></p></body></html>", nullptr));
        lblFX2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\245\277\345\214\227\351\243\216</span></p></body></html>", nullptr));
        lblFX3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\245\277\345\214\227\351\243\216</span></p></body></html>", nullptr));
        lblFX4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\245\277\351\243\216</span></p></body></html>", nullptr));
        lblFX5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\245\277\351\243\216</span></p></body></html>", nullptr));
        lblFl0->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">2\347\272\247</span></p></body></html>", nullptr));
        lblFl1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">3\347\272\247</span></p></body></html>", nullptr));
        lblFl2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">2\347\272\247</span></p></body></html>", nullptr));
        lblFl3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">1\347\272\247</span></p></body></html>", nullptr));
        lblFl4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">2\347\272\247</span></p></body></html>", nullptr));
        lblFl5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">2\347\272\247</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
