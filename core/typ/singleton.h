//**************************************************************************************************
//*  FRIDA: fast reliable interactive data analysis
//*  (C) Joachim Wuttke 2001-
//*  http://apps.jcns.fz-juelich.de/frida
//**************************************************************************************************

//! \file  singleton.h
//! \brief declares and implements the mixin interface ISingleton.

#ifndef ISINGLETON_H
#define ISINGLETON_H

//! Mixin interface for singleton classes.

template <class T> class ISingleton
{
private:
    ISingleton(ISingleton const&) = delete; //!< To prevent copying
    void operator=(ISingleton const&) = delete; //!< To prevent copying
protected:
    ~ISingleton(){};
    ISingleton() {}
public:
    static T* instance()
    {
        static T* instance = new T();
        return instance;
    }
};

#endif // ISINGLETON_H
