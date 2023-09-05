#include <Qsci/qsciscintilla.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tree.h"
#include <QMenuBar>
#include <QToolBar>
#include <QIcon>
#include <QLabel>
#include <QFontComboBox>
#include <QComboBox>
#include <QToolButton>
#include <QStatusBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCursor>
#include <QClipboard>
#include <QLineEdit>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QFontDialog>
#include <QTextCodec>   // 字符编码转换头文件
#include <QDebug>
#include <string.h>
#include<QSplitter>

QString path;                               // 定义一个全局变量存放地址
QTextCodec *codec;                          // 字符编码指针
QsciScintilla *curScintilla = NULL;         // 当前的代码对象
QsciLexer* globalLexer = NULL;              // 全局代码编辑器

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化文件为未保存状态
    isUnititled = true;
    // 初始化文件名为".c"
    curFile = tr("IDE");
    // 初始化窗口标题为文件名
    setWindowTitle(curFile);

    //设置窗口图标
    this->setWindowIcon(QIcon(":/icon/icon/editor.png"));
    //设置窗口大小
    this->resize(1200,600);

    tcf = new QTextCharFormat;

    codec = QTextCodec::codecForName("GBK");

    createMenu();
    createTool();
    connectImpl();

    /****************************** 设置全局词法器 *******************************/

    //创建全局词法编辑器
    globalLexer = new QsciLexerCPP;
    //配置全局词法编辑器的颜色
    globalLexer->setColor(QColor("#008000"),QsciLexerCPP::Comment);
    globalLexer->setColor(QColor("#ff0000"),QsciLexerCPP::Number);
    globalLexer->setColor(QColor("#008000"),QsciLexerCPP::CommentLineDoc);
    globalLexer->setColor(QColor("#008000"),QsciLexerCPP::DoubleQuotedString);
    globalLexer->setColor(QColor("#ff00ff"),QsciLexerCPP::SingleQuotedString);
    globalLexer->setColor(QColor("#0055ff"),QsciLexerCPP::Keyword);
    globalLexer->setColor(QColor("#0055ff"),QsciLexerCPP::PreProcessor);
    //代码提示
    QsciAPIs *apis = new QsciAPIs(globalLexer);
    /*
    foreach (const QString &keyword, CppAutocomplete::cppAutocompleteList) {
        apis->add(keyword);
    }
    */
    apis->prepare();

     /************ 主体：文本编辑框 ************/
    textEdit = new QsciScintilla;
    textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    textEdit->setMinimumWidth(0);

    // 创建标签页用来存储页面
    // 创建标签页用来存储页面
    tabWidget = new QTabWidget;

    // 添加一个按钮到标签栏的最右侧
    QPushButton* addButton = new QPushButton("➕", tabWidget);
    addButton->setFixedSize(30, 30); // 设置按钮大小

    // 设置标签栏的大小策略，以便按钮有足够的空间
    tabWidget->tabBar()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 连接按钮的点击事件到新建标签页的槽函数
    connect(addButton, &QPushButton::clicked, this, &MainWindow::createTab);

    // 在标签栏上创建一个标签，并将按钮放在标签的左侧
    tabWidget->addTab(new QWidget(), "");
    tabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide, addButton);


    createTab();

    tree = new Tree; // 使用您创建的树形部件类名


    // 创建地图文本框
    QTextEdit *graphicText = new QTextEdit;
    graphicText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建一个垂直布局管理器
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // 创建一个水平布局管理器，用于容纳文件树和右边所有
    QHBoxLayout *horizontalLayout = new QHBoxLayout;

    // 创建一个小的QSplitter，用于容纳地图代码框和代码文件框
    QSplitter *smallsplitter = new QSplitter(Qt::Horizontal);;

    smallsplitter->addWidget(tabWidget);

    smallsplitter->addWidget(graphicText);

//    QList<int> smallsizes;
//    smallsizes << 4 << 1;
//    smallsplitter->setSizes(smallsizes);
    smallsplitter->setStretchFactor(0, 7); // 第一个部分（textEdit）的伸缩因子为7
    smallsplitter->setStretchFactor(1, 1); // 第二个部分（graphicText）的伸缩因子为1

    // 将树形部件添加到水平布局中
    horizontalLayout->addWidget(tree,1);

    // 创建一个QSplitter来容纳地图和代码编辑器的整体和输出结果文本框
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    // 添加smallQSplitter到QSplitter
    splitter->addWidget(smallsplitter);


    // 创建输出结果文本框
    QTextEdit *outputText = new QTextEdit;
    outputText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);



    // 添加输出结果文本框到QSplitter
    splitter->addWidget(outputText);


//    QList<int> sizes;
//    sizes << 1 <<6;
//    splitter->setSizes(sizes);
    splitter->setStretchFactor(0, 11); // 第一个部分（textEdit）的伸缩因子为7
    splitter->setStretchFactor(1, 1); // 第二个部分（graphicText）的伸缩因子为1


    // 将QSplitter添加到水平布局中
    horizontalLayout->addWidget(splitter,4);

    // 创建一个占位的小部件到垂直布局中，以确保输出结果文本框位于底部
    mainLayout->addLayout(horizontalLayout);

    // 创建一个中央部件（QWidget）来容纳布局
    QWidget *centralWidget = new QWidget;
    centralWidget->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(mainLayout);

    // 将中央部件设置为主窗口的中央部件
    setCentralWidget(centralWidget);

    //设置状态栏
    QStatusBar* statusBar = this->statusBar();

    QLabel* authorLabel = new QLabel("作者：不会取名小组");
    authorLabel->setAlignment(Qt::AlignLeft);
    statusBar->addPermanentWidget(authorLabel);

    //点击查找后显示的窗口
    findDlg = new QDialog(this);
    findDlg->setWindowTitle(tr("查找"));
    findLineEdit = new QLineEdit(findDlg);
    QPushButton *btn1= new QPushButton(tr("查找上一个"), findDlg);
    //垂直布局
    QVBoxLayout *layout1= new QVBoxLayout(findDlg);
    layout1->addWidget(findLineEdit);
    layout1->addWidget(btn1);
    //将“查找下一个”按钮与自定义showFindText槽函数连接
    connect(btn1, SIGNAL(clicked()), this, SLOT(showFindText()));

}

void MainWindow::createMenu()
{
    //基本控件设置
    //获取菜单栏
    QMenuBar *menuBar = this->menuBar();

    //定义文件菜单和编辑菜单
    QMenu* fileMenu = menuBar->addMenu("文件");
    QMenu* EditMenu =menuBar->addMenu("编辑");
    QMenu* CompileMenu =menuBar->addMenu("编译");
    QMenu* filenew =menuBar->addMenu("新建窗口");
    QAction *file_new = new QAction(QIcon(":/.png"),"新建窗口");
    filenew->addAction(file_new);

    te = new QTextEdit;
    this->setCentralWidget(te);
    this->setMinimumSize(400,300);      //最小大小
    this->resize(800,600);              //默认大小
    connect(file_new,SIGNAL(triggered(bool)),this,SLOT(FileNew()));



    //向文件菜单中添加行为
    newfile = fileMenu->addAction(QIcon(":/icon/icon/newfile.png"),"新建");
    openfile = fileMenu->addAction(QIcon(":/icon/icon/openfile.png"),"打开");
    savefile = fileMenu->addAction(QIcon(":/icon/icon/savefile.png"),"保存");
    saveasfile = fileMenu->addAction(QIcon(":/icon/icon/saveas.png"),"另存为");

    //向编辑菜单中添加行为
    copyText = EditMenu->addAction(QIcon(":/icon/icon/copy.png"),"复制");
    cutText = EditMenu->addAction(QIcon(":/icon/icon/cut.png"),"剪切");
    pasteText = EditMenu->addAction(QIcon(":/icon/icon/paste.png"),"粘贴");
    EditMenu->addSeparator();
    seekText = EditMenu->addAction(QIcon(":/icon/icon/seek.png"),"查找文本");
    EditMenu->addSeparator();
    fontSet = EditMenu->addAction(QIcon(":/icon/icon/fontSet.png"),"字体设置");
    undoe = EditMenu->addAction(QIcon(":/.png"),"撤销");
    redoe = EditMenu->addAction(QIcon(":/.png"),"重做");

    //向编译菜单中添加行为
    compilefile = CompileMenu->addAction(QIcon(":/icon/icon/copy.png"),"编译");



}
void MainWindow::createTool()
{
    //添加工具栏
    QToolBar* toolBar = this->addToolBar("tool");

    //工具栏添加新建、打开、保存、另存为
    toolBar->addAction(newfile);
    toolBar->addSeparator();
    toolBar->addAction(openfile);
    toolBar->addSeparator();
    toolBar->addAction(savefile);
    toolBar->addSeparator();
    toolBar->addAction(saveasfile);
    toolBar->addSeparator();

    //工具栏添加复制、剪切、粘贴、查找、字体设置
    toolBar->addAction(copyText);
    toolBar->addSeparator();
    toolBar->addAction(cutText);
    toolBar->addSeparator();
    toolBar->addAction(pasteText);
    toolBar->addSeparator();
    toolBar->addAction(seekText);
    toolBar->addSeparator();



    /************ 工具栏的字体设置 ************/

    //设置“字体”标签
    fontTypeLabel = new QLabel("字体：");
    toolBar->addWidget(fontTypeLabel);
    //添加字体框
    fontTypeCmb = new QFontComboBox;
    toolBar->addWidget(fontTypeCmb);

    toolBar->addSeparator();

    //设置“字号”标签
    fontSizeLabel = new QLabel("字号：");
    toolBar->addWidget(fontSizeLabel);
    //添加字号框
    fontSizeCmb = new QComboBox;
    toolBar->addWidget(fontSizeCmb);
    //字号框中添加字号选项
    for(int i=1;i<=72;i++){
        fontSizeCmb->addItem(QString::number(i));
    }

    toolBar->addSeparator();

    //添加加粗按钮
    boldBtn = new QToolButton();
    boldBtn->setToolTip("加粗");
    boldBtn->setIcon(QIcon(":/icon/icon/bold.png"));
    toolBar->addWidget(boldBtn);

    toolBar->addSeparator();

    //添加下划线按钮
    underlineBtn = new QToolButton();
    underlineBtn->setToolTip("下划线");
    underlineBtn->setIcon(QIcon(":/icon/icon/underline.png"));
    toolBar->addWidget(underlineBtn);

//    qDebug() << "addwidget";


//    qDebug() << "Highligher";

}
void MainWindow::createTab(){
    if (tabWidget->count() < 50) {

        // 创建新的对象
        QsciScintilla* newScintilla = new QsciScintilla; // 创建一个新的 QsciScintilla 对象
        newScintilla->setLexer(globalLexer); // 设置词法分析器（可能是您在其他地方定义的）
        tabWidget->addTab(newScintilla, "New Tab"); // 将新的 QsciScintilla 对象添加到标签页控件中，并设置标签页标题为 "New Tab"

        // 添加关闭按钮到新标签页
        QWidget* tabWidgetContainer = new QWidget();
        QHBoxLayout* tabLayout = new QHBoxLayout(tabWidgetContainer);
        QLabel* tabLabel = new QLabel("New Tab");
        QPushButton* closeButton = new QPushButton("✖"); // 使用✖作为关闭按钮的文本，可以替换为图标

        // 设置关闭按钮的固定大小（例如：20x20 像素）
        closeButton->setFixedSize(20, 20);

        tabLayout->addWidget(tabLabel);
        tabLayout->addWidget(closeButton);
        tabLayout->setContentsMargins(0, 0, 0, 0);
        tabLayout->setSpacing(0);

        tabWidget->setTabText(tabWidget->count() - 1, "");
        tabWidget->tabBar()->setTabButton(tabWidget->count() - 1, QTabBar::LeftSide, tabWidgetContainer);

        // 连接按钮的点击事件到关闭标签页的槽函数
        connect(closeButton, &QPushButton::clicked, [this, newScintilla]() {
            // 获取当前标签页的索引
            int currentIndex = tabWidget->indexOf(newScintilla);

            // 关闭标签页
            tabWidget->removeTab(currentIndex);

            // 删除相应的 QsciScintilla 对象
            delete newScintilla;
        });

        // 切换到最新的这个 tab
        tabWidget->setCurrentIndex(tabWidget->count() - 1);

        // 设置自动补全，自动补全所有地方出现的
        newScintilla->setAutoCompletionSource(QsciScintilla::AcsAPIs);
        newScintilla->setAutoCompletionThreshold(2);

         // 设置严格的括号匹配
        newScintilla->setBraceMatching(QsciScintilla::StrictBraceMatch);

        // 设置为UTF-8编码
        newScintilla->setUtf8(true);

        // 设置自动补全大小写敏感
        newScintilla->setAutoCompletionCaseSensitivity(true);

        // 设置自动缩进
        newScintilla->setAutoIndent(true);

        // 启用显示缩进引导线
        newScintilla->setIndentationGuides(true);

        // 设置当前行的光标宽度和颜色
        newScintilla->setCaretWidth(2); // 光标宽度，0表示不显示光标
        newScintilla->setCaretForegroundColor(QColor("darkCyan")); // 光标颜色
        newScintilla->setCaretLineVisible(true); // 是否高亮显示光标所在行
        newScintilla->setCaretLineBackgroundColor(Qt::lightGray); // 光标所在行背景颜色

        // 设置选中文本的背景和前景颜色
        newScintilla->setSelectionBackgroundColor(Qt::black);
        newScintilla->setSelectionForegroundColor(Qt::white);

        // 设置未匹配括号的颜色
        newScintilla->setUnmatchedBraceForegroundColor(Qt::blue);
        newScintilla->setBraceMatching(QsciScintilla::SloppyBraceMatch);

        // 设置左侧行号栏的宽度等
        QFont font("Courier", 10, QFont::Normal);
        QFontMetrics fontmetrics = QFontMetrics(font);
        newScintilla->setMarginWidth(0, fontmetrics.width("0000"));
        newScintilla->setMarginLineNumbers(0, true);
        newScintilla->setBraceMatching(QsciScintilla::SloppyBraceMatch); // 括号匹配
        newScintilla->setTabWidth(4);
        QFont margin_font;
        margin_font.setFamily("SimSun");
        margin_font.setPointSize(11);
        newScintilla->setMarginsFont(margin_font); // 设置页边字体
        newScintilla->setMarginType(0, QsciScintilla::NumberMargin); // 设置标号为0的页边显示行号
        newScintilla->setMarginsBackgroundColor(Qt::gray); // 显示行号背景颜色
        newScintilla->setMarginsForegroundColor(Qt::white);

        // 设置折叠样式和折叠栏颜色
        newScintilla->setFolding(QsciScintilla::BoxedTreeFoldStyle);
        newScintilla->setFoldMarginColors(Qt::gray, Qt::lightGray);

    }
}

/************* 信号与槽的连接 *************/

void MainWindow::connectImpl()
{

    //信号与槽-新建文件
    connect(newfile,QAction::triggered,this,MainWindow::newFile);

    //信号与槽-打开文件 //C++11特性：lambda表达式 匿名函数
    connect(openfile,QAction::triggered,[=]{
        if(maybeSave()){
            QString fileName = QFileDialog::getOpenFileName(this);

            if(!fileName.isEmpty()){
                openFile(fileName);
            }
        }
    });

    //信号与槽-保存文件
    connect(savefile,QAction::triggered,this,MainWindow::save);

    //信号与槽-文件另存为
    connect(saveasfile,QAction::triggered,this,MainWindow::saveasFile);

    // 信号与槽-复制
//    connect(copyText, &QAction::triggered, textEdit, &QsciScintilla::copy);

    // 连接按钮的点击信号到槽函数
    connect(copyText, &QAction::triggered, this, [=]() {
        // 执行QScintilla的复制操作
        textEdit->copy();
    });

    // 信号与槽-剪切
//    connect(cutText, &QAction::triggered, textEditor, &QsciScintilla::cut);

    connect(cutText, &QAction::triggered, this, [=]() {
        // 执行QScintilla的复制操作
        textEdit->cut();
    });

    // 信号与槽-粘贴
//    connect(pasteText, &QAction::triggered, textEditor, &QsciScintilla::paste);

    connect(pasteText, &QAction::triggered, this, [=]() {
        // 执行QScintilla的复制操作
        textEdit->paste();
    });

    //信号与槽-搜索
    connect(seekText,QAction::triggered,[=]{
        findDlg->show();
    });

    //信号与槽-字体设置
    connect(fontSet,QAction::triggered,[=]{
      bool fontSelected;
      QFont font = QFontDialog::getFont(&fontSelected,this);
      if(fontSelected){
          textEdit->setFont(font);
      }
    });

    /************ 工具栏的字体设置 ************/

    //信号与槽-字体改变
    connect(fontTypeCmb,QFontComboBox::currentFontChanged,this,MainWindow::setFont);

    //信号与槽-字号改变
    void (QComboBox::*p)(int)=QComboBox::currentIndexChanged;
    connect(fontSizeCmb,p,this,MainWindow::setFontSize);

    //信号与槽-字体加粗
    connect(boldBtn,QToolButton::clicked,this,MainWindow::setBold);

    //信号与槽-字体下划线
    connect(underlineBtn,QToolButton::clicked,this,MainWindow::setUnderline);

    //信号与槽-文件编译
    connect(compilefile,QAction::triggered,this,MainWindow::compile_file);
    connect(undoe,QAction::triggered,this,MainWindow::undo);
    connect(redoe,QAction::triggered,this,MainWindow::redo);
}

    /*************  自定义槽函数的实现 *************/

void MainWindow::undo()
{
    textEdit->undo();
}

void MainWindow::redo()
{
    textEdit->redo();
}

//新建窗口
void MainWindow::FileNew()
{
    qDebug()<<"新窗口";
    x = 0;
    y = 0;
    x = this->geometry().x() + 25;
    y = this->geometry().y() + 25;
    MainWindow *bbq;               //新窗口
    bbq = new MainWindow;
    bbq->setWindowTitle("IDE");
    bbq->move(x,y);
    bbq->show();
}

//槽函数实现-新建文件
void MainWindow::newFile(){
  if(maybeSave()){
      isUnititled = true;
      curFile = tr(".c");
      setWindowTitle(curFile);
      textEdit->clear();
  }
}

//判断是否保存过
bool MainWindow::maybeSave(){
    //如果文档被更改了
    if(textEdit->isModified()){
        //自定义一个警告对话框
        QMessageBox box;
        box.setWindowTitle(tr("警告"));
        box.setIcon(QMessageBox::Warning);
        box.setText(curFile+tr(" 尚未保存，是否保存？"));
        QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        QPushButton *cancelBtn = box.addButton(tr("取消"),QMessageBox::RejectRole);
        box.exec();
        if(box.clickedButton()==yesBtn)
            return save();
        else if(box.clickedButton()==cancelBtn)
            return false;
    }
    //如果文档没被修改，则返回true
    return true;
}

//Todo: bug写入数据保存文件之后再点击保存按钮会提示未保存！！！！！！
//如果文档以前没有保存过，那么执行另存为操作saveasFile()
//如果已经保存过，那么调用saveFile()执行文件保存操作。
bool MainWindow::save(){
    if(isUnititled){
        return saveasFile();
    }else {
        return saveFile(curFile);
    }
}

//槽函数实现-另存为操作，传入文件名调用真正的保存文件操作
bool MainWindow::saveasFile(){
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为"),curFile);
    if(fileName.isEmpty())
        return false;
    path=fileName;
    return saveFile(fileName);
}

//槽函数实现-真正的保存文件操作
bool MainWindow::saveFile(const QString &fileName){
    QFile f(fileName);

    if(f.open(QIODevice::ReadWrite)){
           QTextStream fin(&f);
           fin<<textEdit->text();
     }else{
           qDebug()<<"save file failed!";
           return false;
    }
    isUnititled = false;
    //获得文件的标准路径
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
     path=curFile;
    return true;
}

//槽函数实现-打开文件
bool MainWindow::openFile(const QString &fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadWrite)){
           QTextStream in(&f);
           QString fcontent = in.readAll();
           textEdit->setText(fcontent);
           f.close();
     }else{
           qDebug()<<"open file failed!";
           return false;
    }
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    return true;

}

//槽函数实现-显示查找到的文本
void MainWindow::showFindText() {
    // 获取查找文本
    QString findText = findLineEdit->text();

}

//槽函数实现-字体改变
void MainWindow::setFont(const QFont &font)
{

}

//槽函数实现-字号改变
void MainWindow::setFontSize(int index)
{

}

//槽函数实现-字体加粗
void MainWindow::setBold()
{
  QFont font = textEdit->font();

  if(font.bold()) {
    font.setBold(false);
  } else {
    font.setBold(true);
  }

  textEdit->setFont(font);
}

//槽函数实现-字体下划线
void MainWindow::setUnderline()
{
  QFont font = textEdit->font();

  if(font.underline()) {
    font.setUnderline(false);
  } else {
    font.setUnderline(true);
  }

  textEdit->setFont(font);
}
void MainWindow::compile_file()
{
    if(path.isEmpty())
    {
        // 如果没有路径则需要保存一下才能运行
        this->saveasFile();
    }

    QString demo = path;

    // 生成的目标文件名
    demo.replace(".c", "");

    // gcc filename.c -o filename
    QString cmd = QString("gcc %1 -o %2").arg(path).arg(demo);

    // system执行成返回0
    int ret = system(codec->fromUnicode(cmd).data());
    if(ret != 0)
    {
        // cmd /k 停留在终端
        cmd = QString("cmd /k gcc %1 -o %2").arg(path).arg(demo);
        system(codec->fromUnicode(cmd).data());
        return;
    }
    QString target = QString("cmd /k %1").arg(demo);
    system(codec->fromUnicode(target).data());
}


MainWindow::~MainWindow()
{
    delete ui;
}

