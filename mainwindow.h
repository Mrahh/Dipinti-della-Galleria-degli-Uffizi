#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "set.hpp"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class dipinto {
  QString _scuola, _autore, _titolo, _data, _sala;

public:

  dipinto() : _scuola(""), _autore(""), _titolo(""), _data(""), _sala("") {}

  dipinto(QString scuola, QString autore, QString titolo, QString data, QString sala) : _scuola(scuola), _autore(autore), _titolo(titolo), _data(data), _sala(sala) {}

  QString getScuola() const{
      return _scuola;
  }

  QString getTitolo() const {
      return _titolo;
  }

  QString getAutore()const {
      return _autore;
  }

  QString getData() const {
      return _data;
  }

  QString getSala() const {
      return _sala;
  }

  struct ricerca_titolo {
    QString title;

    ricerca_titolo(const QString& titolo) : title(titolo) {}

    bool operator()(dipinto d1) const {
        return d1._titolo.toLower().contains(title.toLower());
    }
  };


  struct equal_dipinto {
    bool operator()(dipinto d1, dipinto d2) const {
      return d1._titolo == d2._titolo && d1._autore == d2._autore && d1._scuola == d2._scuola && d1._data == d2._data && d1._sala == d2._sala;
    }
  };
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void firstSetup();
    void parseData();
    void fillTable();
    void setupSchoolGraph();
    void updateUI();
    void setupDateGraph();
    void clearTextEdits();
    QString setupStr(QString baseStr);

    void updateTable(bool search);
    void setRead(bool readOnly);
    ~MainWindow();

private slots:
    void on_add_button_clicked();
    void on_remove_button_clicked();
    void on_search_button_clicked();
    void on_painting_table_itemSelectionChanged();
    void on_clear_button_clicked();

private:
    Ui::MainWindow *ui;
    set<dipinto, dipinto::equal_dipinto> s1;
    set<dipinto, dipinto::equal_dipinto> tmp;
    bool search = false;
    QString ultimaRicerca = "";
    int selRow = 0;
};
#endif // MAINWINDOW_H
