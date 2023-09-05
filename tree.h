#ifndef TREE_H
#define TREE_H

#include <QMainWindow>
#include"QTreeWidgetItem"
#include"QFileInfoList"
#include"QTranslator"

QT_BEGIN_NAMESPACE
namespace Ui { class Tree; }
QT_END_NAMESPACE

class Tree : public QMainWindow
{
    Q_OBJECT

public:
    Tree(QWidget *parent = nullptr);
    ~Tree();

private:
    QTreeWidget *treeWidget;

    QFileInfoList allfile(QTreeWidgetItem *parent, QString path);
};
#endif // MAINWINDOW_H

