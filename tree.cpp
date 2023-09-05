#include "tree.h"
#include"QTreeWidgetItem"
#include"QFileInfoList"
#include"QDir"
#include"QDebug"
#include <QHBoxLayout>

Tree::Tree(QWidget *parent)
    : QMainWindow(parent)
{
    treeWidget = new QTreeWidget(this);

    QStringList headers;
    headers << "Name";
    treeWidget->setHeaderLabels(headers);

    // 设置扩展策略
    treeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 设置最小最大宽度
    treeWidget->setMinimumWidth(350);
    treeWidget->setMaximumWidth(550);

     treeWidget->setMinimumHeight(950);
    QTreeWidgetItem* root = new QTreeWidgetItem(treeWidget);

    root->setText(0, "D:/QQ");

    QString rootPath = "D:/QQ";
    QFileInfoList fileLists = allfile(root, rootPath);


}



Tree::~Tree()
{

}

QFileInfoList Tree::allfile(QTreeWidgetItem *root,QString path)         //参数为主函数中添加的item和路径名
{
    QDir dir(path);          //遍历各级子目录 "D:/first"
    QDir dir_file(path);    //遍历子目录中所有文件
//    /*添加path路径文件*/
//    QDir dir(path);          //遍历各级子目录 "D:/first"
//    QDir dir_file(path);    //遍历子目录中所有文件
//    dir_file.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);        //获取当前所有文件
//    dir_file.setSorting(QDir::Name | QDir::Reversed);
//    QFileInfoList list_file = dir_file.entryInfoList();
//    for (int i = 0; i < list_file.size(); ++i) {       //将当前目录中所有文件添加到treewidget中
//        QFileInfo fileInfo = list_file.at(i);
//        QString name2=fileInfo.fileName();
//        QTreeWidgetItem* child = new QTreeWidgetItem(QStringList()<<name2);
//        child->setIcon(0, QIcon("D:/11.png"));
//        //child->setCheckState(1, Qt::Checked);
//        child->setText(0,name2);
//        root->addChild(child);

//    }
    //先遍历文件夹 添加进widget
    QFileInfoList file_list=dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录 QDir::Dirs 0x001 列出目录；


    for(int i = 0; i != folder_list.size(); i++)         //自动递归添加各目录到上一级目录
    {

             QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
             QFileInfo folderinfo= folder_list.at(i);
             QString name=folderinfo.fileName();      //获取目录名
             QTreeWidgetItem* childroot = new QTreeWidgetItem(QStringList()<<name);
             root->addChild(childroot);
             //childroot->setIcon(0, QIcon("D:/22.png"));  //这里只是用了D盘下一张图片 可以随意更改 注意斜杠/
             //childroot->setCheckState(1, Qt::Checked);
             childroot->setText(0,name);
             root->addChild(childroot);              //将当前目录添加成path的子项
             QFileInfoList child_file_list = allfile(childroot,namepath);          //进行递归 递归这个文件夹
             file_list.append(child_file_list);
             file_list.append(name);

     }
    /*添加path路径文件*/

    dir_file.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);        //获取当前所有文件
    dir_file.setSorting(QDir::Name);//
    QFileInfoList list_file = dir_file.entryInfoList();
    for (int i = 0; i < list_file.size(); ++i) {       //将当前目录中所有文件添加到treewidget中
        QFileInfo fileInfo = list_file.at(i);
        QString name2=fileInfo.fileName();
        QTreeWidgetItem* child = new QTreeWidgetItem(QStringList()<<name2);
        child->setIcon(0, QIcon("D:/11.png"));	//这里只是用了D盘下一张图片 可以随意更改
        //child->setCheckState(1, Qt::Checked);
        child->setText(0,name2);
        root->addChild(child);  //allfile传入的root下面加入

    }
    return file_list;
}

