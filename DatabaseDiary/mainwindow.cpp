#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

#include "QDir"
#include "QFile"
#include "QDebug"

#include "QtWidgets"

#include <QLayout>


void MainWindow::DebugIsDBOpen(QSqlDatabase database, QString from)
{
    #ifndef QT_DEBUG
    return;
    #endif

    const QString debugmsg = database.isOpen() ? "DB was already open!" : "DB not open!";

    qDebug() << from << debugmsg;
}

void MainWindow::ExecSqlQuery(QSqlQuery& query, QString from, bool bShowLastError)
{
    const bool success = query.exec();

    if( from != "")
    {
        if(success)
        {
            qDebug() << from << "Query executed!";
            }
        else
        {
            qDebug() << from << "Query Failed!";
            }
        }

    if(bShowLastError)
    {
        qDebug() << "Last error: " << query.lastError();
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    delete ui->mainToolBar;
    setWindowTitle("Database Diary");

    const float ratio = 0.5;
    QDesktopWidget dw;
    int x=dw.width()*ratio;
    int y=dw.height()*ratio;
    resize(x,y);

    InitialiseDB();
    AddComponents();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::InitialiseDB()
{
    //Check for directory
    const QDir dbdir = QDir::currentPath() + "/database/";

    //Create if it doesn't exist
    if( !dbdir.exists() )
    {
        dbdir.mkdir(dbdir.absolutePath());
    }

    //Check if the database exists
    const bool dbexists = QFile::exists(dbdir.absoluteFilePath(dbname));

    //Open (a) database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbdir.absoluteFilePath(dbname));

    //If the database didn't exist
    if(!dbexists)
    {
        //It won't have a table in, so create one
        CreateTable();
        }

    if(!db.isOpen()) db.open();

    //Check if it is now open
    DebugIsDBOpen(db, "Initialize DB: ");

}


void MainWindow::CreateTable()
{
    if( !db.open() )
    {
        qDebug() << "Failed to open after creation!";
    }
    else
    {
        qDebug() << "Opened after creation. Creating table within...";
        QSqlQuery query;
        query.prepare("CREATE TABLE '"+tblname+"' "
                       "(Which INTEGER PRIMARY KEY, "
                           "[When] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, "
                             "What CHARACTER)");

        const QString from = "Initialization of table: '"+tblname+"' ";
        ExecSqlQuery(query, "CREATE TABLE: ", true);
          }
}


void MainWindow::AddComponents()
{
    /////////////////////////////////////////////////////
    //  C O M P O N E N T  C R E A T I O N
    /////////////////////////////////////////////

    //TABLE LABEL
    QLabel *labelTable = new QLabel;
    labelTable->setText("Entries: ");

    //TABLEVIEW
    tableView_Diary = new QTableView(this);
    tableView_Diary->verticalHeader()->hide();
    tableView_Diary->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableView_Diary->setSelectionBehavior(QAbstractItemView::SelectRows);

    RefreshTableData(tableView_Diary);

        //Connect table selection to slot
        connect(tableView_Diary, SIGNAL(activated(QModelIndex)), this, SLOT(on_tableView_activated(QModelIndex)));

    //NEW DIARY BUTTON
    //QPushButton *pushButton_NewDiary = new QPushButton(this);
    //pushButton_NewDiary->setText(tr("New Diary"));

    //SWITCH DIARY BUTTON
    //QPushButton *pushButton_SwitchDiary = new QPushButton(this);
    //pushButton_SwitchDiary->setText(tr("Switch Diary..."));

    //DELETE DIARY BUTTON
    //QPushButton *pushButton_DeleteDiary = new QPushButton(this);
    //pushButton_DeleteDiary->setText(tr("Delete Diary"));

    //ENTRY TEXT LABEL
    QLabel *labelEntry = new QLabel;
    labelEntry->setText("Entry: ");

    //ENTRY TEXT FIELD
    textEdit_EntryText = new QTextEdit(this);

        //Connect text field to slot
        //connect(textEdit_EntryText,SIGNAL())

    //SUBMIT ENTRY BUTTON
    QPushButton *pushButton_Submit = new QPushButton(this);
    pushButton_Submit->setText(tr("Submit Entry"));

        //Connect quit button to slot
        connect(pushButton_Submit, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_Submit_clicked()));

    //QUIT BUTTON
    QPushButton *pushButton_Quit = new QPushButton(this);
    pushButton_Quit->setText(tr("Quit"));

        //Connect quit button to slot
        connect(pushButton_Quit, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_Quit_clicked()));

    /////////////////////////////////////////////////////
    //  L A Y O U T
    ////////////////////////////////////////////

    //Make submit button stretch to fill space
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(pushButton_Submit->sizePolicy().hasHeightForWidth());
    pushButton_Submit->setSizePolicy(sizePolicy);
    pushButton_Quit->setSizePolicy(sizePolicy);

    //Grid to hold submit and quit buttons
    QGridLayout *grid1 = new QGridLayout;
    grid1->addWidget(textEdit_EntryText, 0,0,6,8);
    grid1->addWidget(pushButton_Submit, 0, 8, 4, 2);
    grid1->addWidget(pushButton_Quit, 4,8, 2,2);

    //Horizontal box to hold textedit and grid
//    QHBoxLayout *hor1 = new QHBoxLayout;
//    hor1->addWidget(textEdit_EntryText);
//    hor1->addLayout(grid1);

    //Widget to hold bottom
    QWidget *bottom = new QWidget;
    bottom->setLayout(grid1);

    //SPLITTER between table and bottom
    QSplitter *split = new QSplitter(Qt::Vertical);
    split->addWidget(tableView_Diary);
    split->addWidget(bottom);

    QList<int> sizes2;
    sizes2 << 200 << 1;
    split->setSizes(sizes2);

    //Set QWidget as the central layout for the main window
    setCentralWidget(split);

}


void MainWindow::RefreshTableData(QTableView* tableView)
{
    DebugIsDBOpen(db,"REFRESH: ");

    QSqlQueryModel *db_model = new QSqlQueryModel();

    QSqlQuery query;

    query.prepare("SELECT * FROM '"+tblname+"'");

    ExecSqlQuery(query, "REFRESH: ");

    db_model->setQuery(query);
    tableView->setModel(db_model);

    tableView_Diary->resizeColumnToContents(0);
    tableView_Diary->resizeColumnToContents(1);
    tableView_Diary->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    //Set uniform column width to fit tableView space


    //Hide row numbering
    //tableView->verticalHeader()->hide();

    //qDebug() << "Table: " << tableView_Diary-> << "Col0: " << tableView->columnWidth(0) << "Col1: " << tableView->columnWidth(1);
}


////////////////////////////////////////

///////////////////////////////////////


void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    QString val = tableView_Diary->model()->data(index).toString();

    QSqlQuery qry;
    qry.prepare("SELECT * FROM '"+tblname+"' WHERE Which='"+val+"' OR [When]='"+val+"' OR What='"+val+"' ");

   ExecSqlQuery(qry, "", true);

   while(qry.next())
   {
        textEdit_EntryText->setText(qry.value(2).toString());
   }


//qry.value(2).toString()

    //textEdit_EntryText->setText(val);
}

void MainWindow::on_pushButton_SwitchDiary_clicked()
{

}

void MainWindow::on_pushButton_Submit_clicked()
{
    //SUBMIT ENTRY

    //Get text from text field
    const QString entered_text = textEdit_EntryText->toPlainText();
    const QString warningmsg = "You must enter text here!";

    QSqlQuery query;

    query.prepare( "INSERT INTO '"+tblname+"' "
                    "(What) "
                            "VALUES (:entrytext)");

    query.bindValue(":entrytext", entered_text);

    if(entered_text == "" || entered_text == warningmsg)
    {
        textEdit_EntryText->setText(warningmsg);
    }
    else
    {
        ExecSqlQuery(query, "SUBMIT BUTTON: ");
        RefreshTableData(tableView_Diary);
        }
    }


void MainWindow::on_pushButton_Quit_clicked()
{
    qDebug() << "Released!!";
    close();
}
