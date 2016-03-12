#ifndef CORE_PRIORITY_CHAIN_H
#define CORE_PRIORITY_CHAIN_H

#include <QSharedPointer>

namespace core {
//------------------------------------------------------------------------------

template<typename T>
class ChainLink {
public:
  virtual ~ChainLink();

  virtual uint getPriority() const = 0;

  QSharedPointer<T> getNext();
  bool hasNext() const;
  void setNext(QSharedPointer<T> link);
  void setNextSilent(QSharedPointer<T> link);
  void setPrevious(QSharedPointer<T> link);

protected:
  QSharedPointer<T> next;
  QSharedPointer<T> previous;
  virtual void nextChangedImpl() = 0;

private:
  void nextChanged();
};

template<typename T>
ChainLink<T>::~ChainLink() = default;

template<typename T>
QSharedPointer<T> ChainLink<T>::getNext() {
  return next;
}

template<typename T>
bool ChainLink<T>::hasNext() const {
  return next != nullptr;
}

template<typename T>
void ChainLink<T>::setNext(QSharedPointer<T> link) {
  setNextSilent(link);
  nextChanged();
}

template<typename T>
void ChainLink<T>::setNextSilent(QSharedPointer<T> link) {
  next = link;
}

template<typename T>
void ChainLink<T>::setPrevious(QSharedPointer<T> link) {
  previous = link;
}

template<typename T>
void ChainLink<T>::nextChanged() {
  nextChangedImpl();
  if (previous) previous->nextChanged();
}

//------------------------------------------------------------------------------

template<typename T>
QSharedPointer<T>& operator <<(QSharedPointer<T>& firstLink,
                               QSharedPointer<T> link) {
  if (!link) return firstLink;
  if (!firstLink) {
    link->setPrevious(QSharedPointer<T>());
    link->setNext(QSharedPointer<T>());
    firstLink = link;
    return firstLink;
  }

  if (link->getPriority() < firstLink->getPriority()) {
    TR("Insert as first");
    // Insert as first.
    firstLink.swap(link);
    link->setPrevious(firstLink);
    firstLink->setPrevious(QSharedPointer<T>());
    firstLink->setNext(link);
    return firstLink;
  }

  if (link->getPriority() == firstLink->getPriority()) {
    // Replace first.
    firstLink.swap(link);
    firstLink->setPrevious(QSharedPointer<T>());
    firstLink->setNext(link->getNext());
    return firstLink;
  }

  auto previousLink = firstLink;
  auto currentLink = firstLink->getNext();
  while (currentLink) {
    if (link->getPriority() == currentLink->getPriority()) {
      // Replace link.
      previousLink->setNextSilent(link);
      link->setPrevious(previousLink);
      link->setNext(currentLink->getNext());
      return firstLink;
    }
    if (link->getPriority() < currentLink->getPriority()) {
      // Insert link inbetween.
      link->setPrevious(previousLink);
      previousLink->setNextSilent(link);
      link->setNext(currentLink);
      return firstLink;
    }
    previousLink = currentLink;
    currentLink = currentLink->getNext();
  }
  // Push link last.
  previousLink->setNextSilent(link);
  link->setPrevious(previousLink);
  link->setNext(QSharedPointer<T>());
  return firstLink;
}

//------------------------------------------------------------------------------
}
#endif
