#include "pieceslist.h"
#include "mainwindow.h"

PiecesList::PiecesList(QWidget *parent, const int pieceSize)
    : QListWidget(parent)
{
    /* Устанавливаем свойства внешнего вида */

    setDragEnabled(true);
    setAcceptDrops(true);
    setAutoScroll(false);
    setSpacing(-3);

    setViewMode(QListView::IconMode);
    setIconSize(QSize(pieceSize, pieceSize));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setStyleSheet("QListWidget::item { border-style:none }");
}

void PiecesList::addPiece(QPixmap pixmap)
{    
    /* Добавляем новый кусочек головоломки */

    QListWidgetItem *pieceItem = new QListWidgetItem(this);

    pieceItem->setIcon(QIcon(pixmap));
    pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled);

    /* Добавляем новый индекс кусочка головоломки) */

    piecesLocations.append(this->count() - 1);
}

void PiecesList::swapPieces(const int index1, const int index2)
{
    /* Замена двух кусочков головоломки, расположенных в index1 и index2 */

    QListWidgetItem *item1 = this->item(index1);
    QListWidgetItem *item2 = this->item(index2);

    QIcon iconTemp = item1->icon();
    item1->setIcon(item2->icon());
    item2->setIcon(iconTemp);

    int tempLoc = piecesLocations.at(index1);

    piecesLocations.replace(index1, piecesLocations.at(index2));
    piecesLocations.replace(index2, tempLoc);
}

void PiecesList::clearList()
{
    if (!piecesLocations.isEmpty())
        piecesLocations.clear();

    clear();
}

int PiecesList::getPieceIndex(const int index) const
{
    return piecesLocations.at(index);
}

int PiecesList::getPieceLocation(const int piece_index) const
{
    return piecesLocations.indexOf(piece_index);
}

void PiecesList::dropEvent(QDropEvent *event)
{
    if (event->source() == this)
        event->accept();
    else
        event->ignore();

    /* Находим элемент курсором мыши */

    QListWidgetItem *itemMouseOver = this->itemAt(event->pos());

    if (itemMouseOver)
    {
        /* Получаем идентификатор двух элементов: перетаскиваемого и заменяемого */

        int itemIdFrom = this->row(currentItem());
        int itemIdTo = this->row(itemMouseOver);

        /* Свапаем айди двух айтемов */

        swapPieces(itemIdFrom, itemIdTo);
    }

    /* Пазл собран? Если да, то piecesLocations[i] = i для всех i и
     * Даём сигнал собранной картинки */

    for (int i = 0; i < piecesLocations.count(); i++)
        if (piecesLocations.at(i) != i)
            return;

    emit puzzleCompleted();
}
