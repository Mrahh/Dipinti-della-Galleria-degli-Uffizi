#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"
#include "QDebug"
#include <QtWidgets/QWidget>
#include <QtCharts>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    firstSetup();
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::firstSetup(){
    // recupero dati e inizializzazione tabelle/grafici
    parseData();
    fillTable();
    setupSchoolGraph();
    setupDateGraph();
}


void MainWindow::updateUI(){
    setupSchoolGraph();
    setupDateGraph();
    clearTextEdits();
}


void MainWindow::parseData() {

    QFile file(":/dataset/dipinti_uffizi.csv");

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
    }

    dipinto tmp;
    // lettura fino a fine file con split di virgole facendo escape tra quelle dentro le virgolette (titoli)
    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();
        if (!line.isEmpty()) {
            QStringList fields;
            QString field;
            bool insideQuotes = false;

            for (const QChar &ch : line) {
                if (ch == '\"')
                    insideQuotes = !insideQuotes;
                else if (ch == ',' && !insideQuotes) {
                    fields.append(field.trimmed());
                    field.clear();
                } else
                    field += ch;
            }

            fields.append(field.trimmed());

            for (int i = 0; i < qMin(5, fields.size()); ++i) {
                tmp = dipinto(fields[0],fields[1],fields[2],fields[3],fields[4]);
                s1.add(tmp);
            }

        }
    }
}


void MainWindow::fillTable() {
    set<dipinto, dipinto::equal_dipinto>::const_iterator i,ie;
    auto tbl = this->ui->painting_table;

    tbl->setRowCount(0);
    tbl->setColumnCount(5);

    // non cliccabile, selezione righe, resizabile
    tbl->setSelectionMode(QAbstractItemView::SingleSelection);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    i = s1.begin();
    ie = s1.end();
    tbl->setHorizontalHeaderLabels(QStringList{i->getScuola(), i->getAutore(), i->getTitolo(), i->getData(), i->getSala()});

    // rimuovo prima riga (intestazione)
    s1.remove(dipinto(i->getScuola(), i->getAutore(), i->getTitolo(), i->getData(), i->getSala()));
    i++;

    for(; i != ie; ++i) {
        tbl->insertRow(tbl->rowCount());
        tbl->setItem(tbl->rowCount()-1, 0, new QTableWidgetItem(i->getScuola()));
        tbl->setItem(tbl->rowCount()-1, 1, new QTableWidgetItem(i->getAutore()));
        tbl->setItem(tbl->rowCount()-1, 2, new QTableWidgetItem(i->getTitolo()));
        tbl->setItem(tbl->rowCount()-1, 3, new QTableWidgetItem(i->getData()));
        tbl->setItem(tbl->rowCount()-1, 4, new QTableWidgetItem(i->getSala()));
    }
}


void MainWindow::updateTable(bool search) {
    set<dipinto, dipinto::equal_dipinto>::const_iterator i,ie;
    auto tbl = this->ui->painting_table;
    tbl->clearContents();
    tbl->model()->removeRows(0, tbl->rowCount());


    // caricamento dati normale
    if(!search) {
        i = s1.begin();
        ie = s1.end();
    } else { // cericamento dati ricerca
        i = tmp.begin();
        ie = tmp.end();
    }

    for(; i != ie; ++i) {
        tbl->insertRow(tbl->rowCount());
        tbl->setItem(tbl->rowCount()-1, 0, new QTableWidgetItem(i->getScuola()));
        tbl->setItem(tbl->rowCount()-1, 1, new QTableWidgetItem(i->getAutore()));
        tbl->setItem(tbl->rowCount()-1, 2, new QTableWidgetItem(i->getTitolo()));
        tbl->setItem(tbl->rowCount()-1, 3, new QTableWidgetItem(i->getData()));
        tbl->setItem(tbl->rowCount()-1, 4, new QTableWidgetItem(i->getSala()));
    }
}


void MainWindow::clearTextEdits() {
    ui->author_edit->clear();
    ui->date_edit->clear();
    ui->room_edit->clear();
    ui->school_edit->clear();
    ui->title_edit->clear();

}


QString MainWindow::setupStr(QString baseStr) {
    int res;
    bool preso = false;
    QString result;
    int count = 0;
    // prende in input una data e ritorna una data "pulita" (primi n numeri < 4)

    for (const QChar &ch : baseStr) {
        if (ch.isDigit() && count <= 4) {
            result.append(ch);
            count++;
            preso = true;
        } else if(!ch.isDigit() && preso)
            break;
    }
    res = result.toInt();

    // rimuovo le ultime due cifre per categorizzare le date (1789 -> 17 secolo)
    res/=100;

    if (!preso)
        return QString("NaN");


    return QString::number(res).append("00");

}


void MainWindow::setupSchoolGraph() {
    auto tbl = this->ui->painting_table;
    int rowCount = tbl->rowCount();
    // mappa chiave valore (scuola -> numero elem con stessa scuola)
    QMap<QString, int> valueCountMap;

    for (int row = 0; row < rowCount; ++row) {
        // prendo solo la colonna della scuola
        QTableWidgetItem *item = tbl->item(row, 0);
        if (item) {
            QString cellValue = item->text();
            // se esiste nella mappa aumento il contatore dell'elemento, senno aggiungo nuovo
            if (valueCountMap.contains(cellValue)) {
                valueCountMap[cellValue]++;
            } else {
                valueCountMap.insert(cellValue, 1);
            }
        }
    }

    // Creazione dei dati per il grafico a torta
    QtCharts::QPieSeries *series = new QtCharts::QPieSeries();
    series->setPieSize(1.0f);
    this->ui->schoolGraph->chart()->setTitle("Scuole");
    this->ui->schoolGraph->chart()->legend()->setAlignment(Qt::AlignRight);


    // Percentuali
    for (auto it = valueCountMap.begin(); it != valueCountMap.end(); ++it) {
        QtCharts::QPieSlice *slice = series->append(it.key(), it.value());
        slice->setLabel(it.key() + ": " + QString::number(it.value()/double(rowCount)*100,'f',2) + "%");

    }

    // rimuove le serie precedenti e crea una nuova che conterrà la mia torta
    this->ui->schoolGraph->chart()->removeAllSeries();
    this->ui->schoolGraph->chart()->addSeries(series);

}


void MainWindow::setupDateGraph() {


    auto tbl = this->ui->painting_table;
    int rowCount = tbl->rowCount();
    // mappa chiave valore (data -> numero elem con stessa scuola)
    QMap<QString, int> valueCountMap;

    for (int row = 0; row < rowCount; ++row) {
        // prendo solo la colonna delle date
        QTableWidgetItem *item = tbl->item(row, 3);
        if (item) {
            QString cellValue = item->text().trimmed();
            cellValue = setupStr(cellValue);

            // se esiste nella mappa aumento il contatore dell'elemento, senno aggiungo nuovo
            if (valueCountMap.contains(cellValue)) {
                valueCountMap[cellValue]++;
            } else {
                valueCountMap.insert(cellValue, 1);
            }
        }
    }

    // Creazione dei dati per il grafico a torta
    QtCharts::QPieSeries *series = new QtCharts::QPieSeries();
    series->setPieSize(1.0f);


    for (auto it = valueCountMap.begin(); it != valueCountMap.end(); ++it) {
        QtCharts::QPieSlice *slice = series->append(it.key(), it.value());
        slice->setLabel(it.key() + ": " + QString::number(it.value()));

    }

    this->ui->datesGraph->chart()->setTitle("Date");
    this->ui->datesGraph->chart()->legend()->setAlignment(Qt::AlignRight);

    // rimuove le serie precedenti e crea una nuova che conterrà la mia torta
    this->ui->datesGraph->chart()->removeAllSeries();
    this->ui->datesGraph->chart()->addSeries(series);
}


void MainWindow::setRead(bool readOnly){
    // attiva/disattiva le line edit
    ui->school_edit->setReadOnly(readOnly);
    ui->author_edit->setReadOnly(readOnly);
    ui->title_edit->setReadOnly(readOnly);
    ui->date_edit->setReadOnly(readOnly);
    ui->room_edit->setReadOnly(readOnly);
}


void MainWindow::on_add_button_clicked() {
    QMessageBox msgBox;
    QString scuola, autore, titolo, data, sala;
    scuola = ui->school_edit->text().trimmed();
    autore = ui->author_edit->text().trimmed(),
    titolo = ui->title_edit->text().trimmed();
    data   = ui->date_edit->text().trimmed();
    sala   = ui->room_edit->text().trimmed();

    if (scuola.isEmpty() || autore.isEmpty() || titolo.isEmpty() || data.isEmpty() || sala.isEmpty()) {
        msgBox.setWindowTitle("Campo dati vuoto");
        msgBox.setText("Uno dei campi è vuoto, inserire i dati.");
        msgBox.exec();
        return;
    }

    dipinto p1;
    int row = ui->painting_table->rowCount();

    p1 = dipinto(scuola, autore, titolo, data, sala);

    // se ho aggiunto allora aggiorno la tabella che si sta visualizzando
    // se sono in modalita ricerca controllo che il nuovo inserito soddisfi o meno il filtro
    if (s1.add(p1)) {
        if (!search || dipinto::ricerca_titolo(ultimaRicerca)(p1)) {
            ui->painting_table->insertRow(row);
            ui->painting_table->setItem(row, 0, new QTableWidgetItem(ui->school_edit->text().trimmed()));
            ui->painting_table->setItem(row, 1, new QTableWidgetItem(ui->author_edit->text().trimmed()));
            ui->painting_table->setItem(row, 2, new QTableWidgetItem(ui->title_edit->text().trimmed()));
            ui->painting_table->setItem(row, 3, new QTableWidgetItem(ui->date_edit->text().trimmed()));
            ui->painting_table->setItem(row, 4, new QTableWidgetItem(ui->room_edit->text().trimmed()));
        }
        updateUI();
    } else {
        msgBox.setWindowTitle("Il dipinto inserito esiste già");
        msgBox.setText("Inserire un dipinto non esistente");
        msgBox.exec();
    }

}


void MainWindow::on_remove_button_clicked() {
    QMessageBox msgBox;
    QString scuola, autore, titolo, data, sala;
    scuola = ui->school_edit->text().trimmed();
    autore = ui->author_edit->text().trimmed(),
    titolo = ui->title_edit->text().trimmed();
    data   = ui->date_edit->text().trimmed();
    sala   = ui->room_edit->text().trimmed();

    if (scuola.isEmpty() || autore.isEmpty() || titolo.isEmpty() || data.isEmpty() || sala.isEmpty()) {
        msgBox.setWindowTitle("Campo dati vuoto");
        msgBox.setText("Uno dei campi è vuoto, inserire i dati.");
        msgBox.exec();
        return;
    }

    dipinto p1;
    p1 = dipinto(scuola, autore, titolo, data, sala);

    if (s1.remove(p1)) {
        if (ui->painting_table->rowCount()==1) {
            ui->search_edit->setText("");
            search = false;
            updateTable(search);
        } else
            ui->painting_table->removeRow(selRow);

        ui->painting_table->clearSelection();
        updateUI();
        setRead(false);
    } else {
        msgBox.setWindowTitle("Il dipinto inserito non esiste");
        msgBox.setText("Inserire un dipinto esistente");
        msgBox.exec();
    }
}


void MainWindow::on_search_button_clicked() {

    QString title = ui->search_edit->text().trimmed();
    if (title != "") {
        search = true;
        tmp = filter_out(s1,dipinto::ricerca_titolo(title));
        ultimaRicerca = title;

        updateTable(search);
        setRead(false);
    } else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Campo ricerca vuoto");
        msgBox.setText("Inserire dati nel campo ricerca.");
        msgBox.exec();
        updateTable(false);
    }
    updateUI();

}


void MainWindow::on_painting_table_itemSelectionChanged() {
    // Prendo riga selezionata e prendo le colonne
    auto tbl = ui->painting_table;
    int selectedRow = tbl->currentRow();
    ui->school_edit->setText(tbl->item(selectedRow, 0)->text());
    ui->author_edit->setText(tbl->item(selectedRow, 1)->text());
    ui->title_edit->setText(tbl->item(selectedRow, 2)->text());
    ui->date_edit->setText(tbl->item(selectedRow, 3)->text());
    ui->room_edit->setText(tbl->item(selectedRow, 4)->text());

    setRead(true);
    selRow = selectedRow;
}


void MainWindow::on_clear_button_clicked() {
    ui->painting_table->clearSelection();
    ui->painting_table->clearContents();
    ui->painting_table->model()->removeRows(0, ui->painting_table->rowCount());
    search = false;
    updateTable(false);
    setRead(false);
    ui->search_edit->setText("");
    updateUI();
}

