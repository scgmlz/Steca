//**************************************************************************************************
//*  SCG generic C++ templates
//*  (C) Joachim Wuttke: Frida 2001-, Steca 2017-
//*  License: this file is in the public domain
//**************************************************************************************************

//! \file  singleton.h
//! \brief declares and implements the mixin interface ISingleton.

#ifndef SINGLETON_H
#define SINGLETON_H

//! Mixin interface for singleton classes.

template <class T> class ISingleton {
private:
    ISingleton(const ISingleton&) = delete;
    void operator=(const ISingleton&) = delete;
protected:
    ISingleton() {}
    ~ISingleton() {}
public:
    static T* instance() {
        static T* instance = new T();
        return instance;
    }
};

#endif // SINGLETON_H
