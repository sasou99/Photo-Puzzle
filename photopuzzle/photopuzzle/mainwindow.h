#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPixmap>
#include <QMainWindow>
#include "pieceslist.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0, const int newWinSize = 480);

public slots:
    void changePuzzleSize();
    void openImage(const QString &path = QString());
    void setupPuzzle();
    void solvePuzzle();

private slots:
    void setCompleted();

private:
    void setupMenus();
    void setupWidgets();

    QPixmap puzzleImage; /* Изображение, загруженное пользователем */
    PiecesList *piecesList; /* Разбитое изображение в виде пазла */

    int puzzleSize; /* Размер разбитого изображеня (строка / столбец) */
    int windowSize; /* Размер основного окна в пикселях */
};

#endif
