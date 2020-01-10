# BSCJZ8

#### 开发环境

1. QT：本项目采用qt-opensource-windows-x86-android-5.4.0，也可以使用QT其他WIN版本进行开发
2. QWT: QWT采用QWT6.1.0，可以采用其他版本的QWT

#### QWT编译

![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174405_4d4e32ec_789940.png "qwt.png")

1. 将QWT源码解压到C:\QT目录下
2. 右键点击src目录编译QWT库，生成的QWT库见下图:
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174429_f560fe6b_789940.png "qwtlib.png")

#### BSCJZ8工程编译
1. 用QT Creater打开BSCJZ8工程
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174442_3e792ada_789940.png "bs1.png")
2. 右键点击工程执行编译

主要工程文件简介
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174452_4cb0d10c_789940.png "bs2.png")
1. protocol/qprotocol.cpp
    与下位机通信协议，详细协议规范见《8要素测定仪通信协议V1.0.4》
2. dbopt/dbopt.cpp
    数据库接口封装，实现实现数据库的初始化。
3. export/exportobject.cpp
    数据库导出接口封装，目前仅实现数据库的xml格式导出
4. log/qlog.cpp
    BSCJZ8软件运行LOG接口封装
5. print/qprintobject.cpp
    打印接口封装
6. comcfg.cpp
    串口配置接口，引用UI文件comcfg.ui
7. deviceinfo.cpp
    实现设备信息的增删改查
8. devparamconfig.cpp
    设备参数配置，设备基础信息来自于deviceinfo中配置的设备
9. disassaydata.cpp
    数据显示接口，引用UI文件disasaydata.ui

#### 多语言适配

软件开发过程中用户界面采用英文，对应的中文显示使用QT的Linguist工具进行翻译，翻译生成的文件放到软件运行目录的language目录即可
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174504_9007890c_789940.png "lang.png")

#### 软件运行界面

1. 主窗口：默认在数据采集标签页
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174517_e04cfebd_789940.png "ui1.png")
2. 数据显示窗口
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174526_eca5cfde_789940.png "ui2.png")
3. 数据图形显示界面
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174535_5ae7ab1b_789940.png "ui3.png")
4. 图形显示参数配置界面
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174548_e9956c07_789940.png "ui4.png")
5. 设备信息增删改查界面
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174558_9ea33e02_789940.png "ui5.png")
6. 设备参数设置界面
![输入图片说明](https://images.gitee.com/uploads/images/2020/0110/174609_227ffb38_789940.png "ui6.png")


1. [https://www.qt.io/download/](https://www.qt.io/download/)
2. [https://sourceforge.net/projects/qwt/files/qwt/](https://sourceforge.net/projects/qwt/files/qwt/)
3. www.setupfactory.com 