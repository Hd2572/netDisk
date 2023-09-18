#ifndef BOOK_H
#define BOOK_H

#include <QWidget>

/*
    图书
*/

class Book : public QWidget
{
    Q_OBJECT
  public:
    explicit Book(QWidget* parent = nullptr);

  signals:

  public slots:
};

#endif  // BOOK_H
