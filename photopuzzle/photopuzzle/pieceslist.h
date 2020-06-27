#ifndef PIECESLIST_H
#define PIECESLIST_H

#include <QListWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QVector>

class PiecesList : public QListWidget
{
    Q_OBJECT

public:
    PiecesList(QWidget *parent = 0, const int pieceSize = 0);

    void addPiece(QPixmap pixmap);
    void swapPieces(const int index1, const int index2);
    void clearList();

    int getPieceIndex(const int locIndex) const;
    int getPieceLocation(const int pieceIndex) const;

signals:
    void puzzleCompleted();

protected:
    void dropEvent(QDropEvent *event);

private:
    QVector< int > piecesLocations;
};

#endif
