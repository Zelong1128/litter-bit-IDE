# IDE开发计划（2023/09/04）
## 已办部分：
### 一. 完成基础文本编辑器设计，实现以下基本功能：  
    1.支持键盘/鼠标操作，必要的菜单操作
    2.建立文件，保存/打开/另存/关闭
    3.对字符/串的插入/修改/删除，查找/替换
    4.对文本块的复制、粘贴、剪切
    5.实现对字体大小等的改变
    6.边界栏的自动行号拓展
### 二. 在文本编辑器的基础上进行编译运行功能的开发调试，实现以下基本功能：
    1.能够进行编译、链接、反馈编译结果
    2.对通过编译的程序可投入运行并给出执行结果
### 三. 已经实现的拓展功能
    1.已实现关键词高亮
    2.实现括号的自动匹配
    3.对于所在行的高亮显示

## 待完成部分
### 一. 完善ui方面各功能跳转
    1.更加直观的文件阅读与参考界面
    2.更加直观的编译结果反馈界面
    3.分配更加合理的功能栏栏位设置
### 二. 在*力所能及*的范围内增加各拓展功能
    1.想要实现函数以及循环/判断语句折叠
    2.想要实现断点等编译功能的完善
    3.想要实现chatgpt的接口连接，利用chat帮助IDE使用者进行代码纠错等
    4.想要实现多编译器的可选择性

## 网址集成
github地址：https://github.com/BITShengMinglei/IDE.git  
qt应用程序集成编译：https://www.cnblogs.com/luoxiang/p/13447513.html  
icon资源获取：https://www.iconfinder.com/  
所在行高亮以及绘制行号官方文档：https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html  
关键字高亮官方文档：https://doc.qt.io/qt-6/qtwidgets-richtext-syntaxhighlighter-example.html  
关键字高亮相关注解：https://blog.csdn.net/hitzsf/article/details/109019495  
Qt TextEdit取消换行：https://blog.csdn.net/qq_45662588/article/details/120682996  
Scintilla开源库使用指南：https://blog.csdn.net/mikasoi/article/details/87544502  
QScintilla的安装和部分使用：https://www.cnblogs.com/tuilk/archive/2022/03/12/15995711.html  
QScintilla文档参考：https://qscintilla.com/#home  
 



