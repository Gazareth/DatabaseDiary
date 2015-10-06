#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtSql"

class QTableView;
class QTextEdit;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void RefreshTableData(QTableView* tableView);

private slots:
    void on_tableView_activated(const QModelIndex &index);

    void on_pushButton_SwitchDiary_clicked();

    void on_pushButton_Submit_clicked();
    void on_pushButton_Quit_clicked();


private:
    Ui::MainWindow *ui;

    void InitialiseDB();
    void CreateTable();
    void AddComponents();

    void DebugIsDBOpen(QSqlDatabase database, QString from = "");
    void ExecSqlQuery(QSqlQuery& query, QString from = "", bool bShowLastError = false);

    QTableView* tableView_Diary;
    QTextEdit *textEdit_EntryText;

    QSqlDatabase db;
};

#endif // MAINWINDOW_H
