#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QFrame>
#include <QHBoxLayout>
#include <QInputDialog>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const int newWinSize)
    : QMainWindow(parent)
{
    /* Изначально стоит разбиение картинки на 6 кусочков в строке и столбце.
     * У пользователя есть возможность изменить размерность головоломки с помощью игрового меню. */

    puzzleSize = 6;
    windowSize = newWinSize;

    /* Настраиваем главное окно */

    setupMenus();
    setupWidgets();

    setWindowTitle("Photo Puzzle");
    setFixedSize(windowSize, windowSize);
    setWindowFlags(Qt::WindowCloseButtonHint);

    QMessageBox::information(0, "Introduction",
                              "Привет! Это Photo Puzzle\n\n"
                              "Чтобы начать, загрузи пожалуйста любое изображение. "
                              "Вам потребуется перемещать кусочки головоломки, дабы восстановить исходную картинку. "
                              "В любое время вы сможете изменить размерность головоломки или даже посмотреть решение с помощью игрового меню.\n\n"
                              "УДАЧИ!",
                              QMessageBox::Ok);
}

void MainWindow::openImage(const QString &path)
{
    QString filePath = path;

    /* Показываем диалоговое окно и фильтруем формат файлов. */

    if (filePath.isNull())
        filePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.bmp)");

    /* Если кнопка "Отмена" не нажата, тогда загружаем изображение. */

    if (!filePath.isEmpty())
    {
        QPixmap newImage;

        if (!newImage.load(filePath))
        {
            QMessageBox::warning(this,
                                  "Откройте изображение",
                                  "Невозможно загрузить выбранное изображение. Попробуйте выбрать другой файл",
                                  QMessageBox::Cancel);
            return;
        }

        puzzleImage = newImage;
        setupPuzzle();
    }
}

void MainWindow::setupPuzzle()
{
    /* Если размерность разбиения изменить перед загрузкой, то puzzleImage будет равна null.
     * Эта функция сработает после того, как пользователь загрузит изображение. */

    if (puzzleImage.isNull())
        return;

    /* Некоторые изображения могут иметь прямоугольный формат,
     * программа обрежет квадрат максимального размера, расположенный в центре исходного изображения.
     * Изображение должно быть немного меньше, чем размер главного окна.
     * Также учитываем, что QHBoxLayout, поля которого не равны нулю относительно основного окна. */

    int imageSize = qMin(puzzleImage.width(), puzzleImage.height());

    QPixmap puzzleImageResized = puzzleImage.copy((puzzleImage.width() - imageSize) / 2,
                                                 (puzzleImage.height() - imageSize) / 2,
                                                 imageSize, imageSize)
                             .scaled(windowSize - 40, windowSize - 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    /* Размер кусочка пазла определяется путём деления ширины (или высоты) на кол-во кусочков в строчке (или столбце)*/

    int pieceSize = qFloor(puzzleImageResized.width() / puzzleSize);

    /* Чистим список головоломок и добавляем новые кусочки */

    piecesList->clearList();

    for (int y = 0; y < puzzleSize; y++)
        for (int x = 0; x < puzzleSize; x++)
        {
            QPixmap pieceImage = puzzleImageResized.copy(x * pieceSize, y * pieceSize, pieceSize, pieceSize);
            piecesList->addPiece(pieceImage);
        }

    /* Как только кусочки добавляются в список, метод генерирует случайные индексы (i и j) между 0 и
     * кол-вом частей головоломки - 1 и меняет местами кусочки, размещённые с индексом i и j.
     * Также, чем больше кусочков головоломки, тем больше шанс, что пазл не останется на своём исходном месте.*/

    qsrand(QCursor::pos().x() ^ QCursor::pos().y());

    for (int i = 0; i < piecesList->count(); i++)
    {
        int randomIndex1 = int((double) qrand() / RAND_MAX  * puzzleSize * puzzleSize);
        int randomIndex2 = int((double) qrand() / RAND_MAX  * puzzleSize * puzzleSize);
        piecesList->swapPieces(randomIndex1, randomIndex2);
    }
}

void MainWindow::setCompleted()
{
    /* Если пазл собран, то предлагаем сыграть ещё одну игру */

    int userResponse = QMessageBox::question(this,
                                              "Пазл собран!",
                                              "Поздравляем! Вы успешно справились с этой задачей!\n"
                                              "Не хотите ли сыграть ещё раз?",
                                              QMessageBox::Yes | QMessageBox::No);

    /* Начать новую игру или выйти */

    if (userResponse == QMessageBox::Yes)
        setupPuzzle();
    else
        close();
}

void MainWindow::setupMenus()
{
    /* Создаём строку меню */

    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *openAction = fileMenu->addAction("&Load Image");
    openAction->setShortcuts(QKeySequence::Open);

    QAction *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcuts(QKeySequence::Quit);

    QMenu *gameMenu = menuBar()->addMenu("&Game");
    QAction *shuffleAction = gameMenu->addAction("S&huffle");
    QAction *changePuzzleSizeAction = gameMenu->addAction("Change &Size");
    QAction *solveAction = gameMenu->addAction("Sol&ve");

    /* Подключаем сигналы, генерируемые нажатием на кнопки меню */

    connect(openAction, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(changePuzzleSizeAction, SIGNAL(triggered()), this, SLOT(changePuzzleSize()));
    connect(shuffleAction, SIGNAL(triggered()), this, SLOT(setupPuzzle()));
    connect(solveAction, SIGNAL(triggered()), this, SLOT(solvePuzzle()));
}

void MainWindow::setupWidgets()
{
    /* Создаём рамку и макет объекта, который будет хранить нашу доску головоломки */

    QFrame *frame = new QFrame;
    QHBoxLayout *frameLayout = new QHBoxLayout(frame);

    /* Создаём доску головоломки; размер кусочка будет равен ширине (или высоте)
     * изменённой версии изображения, загруженная пользователем, делённая на кол-во в строке*/

    piecesList = new PiecesList(this, qFloor((windowSize - 40) / puzzleSize) );

    /* Подключите сигнал puzzleCompleted(), которые будет его обрабывать */

    connect(piecesList, SIGNAL(puzzleCompleted()), this, SLOT(setCompleted()), Qt::QueuedConnection);

    /* Добавьте список кусочков в объектов Add piecesList to the layout object and the latter to the main window */

    frameLayout->addWidget(piecesList);
    setCentralWidget(frame);
}

void MainWindow::changePuzzleSize()
{
    bool pressedOk;

    /* Запросить ввод пользователя */

    QString userInput = QInputDialog::getText(this, "Puzzle Size",
                        QString("Пожалуйста выберите размер головоломки. Он должен составлять от 3 до 6: ").arg(puzzleSize),
                        QLineEdit::Normal, QString::number(puzzleSize), &pressedOk);
    int newPuzzleSize;

    if (pressedOk)
    {
        /* Убедимся, что новый размер составляет от 3 до 6 */

        if ((newPuzzleSize = userInput.toInt()) && newPuzzleSize >= 3 && newPuzzleSize <= 6)
        {
            puzzleSize = newPuzzleSize;
            setWindowTitle(QString("Photo Puzzle - %1x%1").arg(puzzleSize));

            /* Запускаем заново */

            setupPuzzle();
        }
        else
            QMessageBox::warning(this, "Puzzle Size",
                                  "Размер окна должен быть от 3 до 6 включительно",
                                  QMessageBox::Ok);
    }
}

void MainWindow::solvePuzzle()
{
    if (puzzleImage.isNull())
        return;

    /* Когда пазл собран, piecesLocations[i] = i. Для каждого местоположения l1
     * ищем кусок головоломки, индекс которого равен l1 и определяем местоположение l2. Меняем позиции l1
     * и l2 и теперь правильный кусок головоломки будет расположен на l1. Повторяем процесс для остальных. */

    for (int i = 0; i < puzzleSize * puzzleSize; i++)
    {
        int pieceIndex = piecesList->getPieceIndex(i);

        if (pieceIndex == i)
            continue;

        int pieceLocation = piecesList->getPieceLocation(i);
        piecesList->swapPieces(i, pieceLocation);
    }
}
