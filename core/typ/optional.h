//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/optional.h
//! @brief     Defines class BadOptionalAccess and class template Optional
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <functional>
#include <array>
#include <exception>
#include <memory>

/**
 *  @brief Exception class for bad Optional accesses.
 *
 */
class BadOptionalAccess: public std::exception
{
public:
    BadOptionalAccess() _GLIBCXX_USE_NOEXCEPT {}

    const char* what() const _GLIBCXX_USE_NOEXCEPT { return "Tried to access value of an Optinal without a value."; }
};

//! denotes an empty optional
struct EmptyOptional {};

static const EmptyOptional emptyOptional = EmptyOptional();

template<typename T>
class Optional;

template<typename X>
struct flatten {
    typedef X type;
};
template<typename Y>
struct flatten<Optional<Y>> {
    typedef Y type;
};

template<typename X>
struct make_optional {
    typedef Optional<X> type;

    type operator()(EmptyOptional opt) { return opt; }
    type operator()(type opt) { return opt; }
};
template<typename Y>
struct make_optional<Optional<Y>> {
    typedef typename make_optional<Y>::type type;

    type operator()(EmptyOptional opt) { return opt; }
    type operator()(type opt) { return opt; }
};

template<>
struct make_optional<void> {
    typedef void type;

    type operator()(EmptyOptional) {}
};

//! Simple cached object.
template<typename T>
class Optional {
public:
    typedef T value_type;

    Optional() : ptr_(nullptr) { }
    Optional(EmptyOptional) : ptr_(nullptr) { }
    Optional(const T &value) : ptr_(new T {value}) { }
    //Optional(T &&value) : value_(std::move(value)), hasValue_(true) { }

    inline auto &operator=(const T &rhs) { setValue_(rhs); return *this; }

    inline auto &operator=(const Optional &rhs)
    {
        ptr_ = rhs.ptr_;
        return *this;
    }

    inline const T *operator->() const
    {
        valueMustExist();
        return &getValue_();
    }

    inline T *operator->()
    {
        valueMustExist();
        return &getValue_();
    }

    inline T &operator*(int) {
        valueMustExist();
        return getValue_();
    }

    inline const T &operator*(int) const {
        valueMustExist();
        return getValue_();
    }

    inline T get(const T &defaultValue) const
    {
        if (hasValue())
            return getValue_();
        else
            return defaultValue;
    }

    inline bool hasValue() const { return !!ptr_; }
    inline bool isEmpty() const { return !ptr_; }
    template<typename R = T>
    inline static Optional<R> empty() { return Optional(); }

    template<typename F, typename Optional>
    friend inline typename make_optional<typename std::result_of<F(typename Optional::value_type)>::type>::type apply(Optional optional, F func);
private:

    template<typename F>
    inline typename make_optional<typename std::result_of<F(T)>::type>::type apply(F func) const {
        if (isEmpty())
            return make_optional<typename std::result_of<F(T)>::type>()(emptyOptional);
        else
            return func(getValue_());
    }

    std::shared_ptr<T> ptr_;

    inline void valueMustExist() const {
        if (!hasValue()) {
            throw BadOptionalAccess();
        }
    }


    //! performs NO checks wether optional actually has  a value ot not. UNSAFE!
    inline T &getValue_() { return *ptr_; }

    //! performs NO checks wether optional actually has  a value ot not. UNSAFE!
    inline const T &getValue_() const { return *ptr_; }

    inline void setValue_(const T &value)
    {
        ptr_ = new T {value};
    }
};


template<typename F, typename Optional>
inline typename make_optional<typename std::result_of<F(typename Optional::value_type)>::type>::type apply(Optional optional, F func) {
    return optional.apply(func);
}

template<typename F, typename Optional1, typename Optional2>
inline typename make_optional<typename std::result_of<F(typename Optional1::value_type, typename Optional2::value_type)>::type>::type apply(Optional1 optional1, Optional2 optional2, F func) {
    return apply(optional1, [&](const auto &v1){return apply(optional2, [&](const auto &v2){ return func(v1, v2); }); });
}

template<typename F, typename Optional, typename... Optionals>
inline typename make_optional<typename std::result_of<F(typename Optional::value_type, typename Optionals::value_type...)>::type>::type apply(Optional optional, Optionals... optionals, F func) {
    return apply(optional, [&](const auto &v){ return apply(optionals..., [&](typename Optionals::value_type... values){ return func(v, values...); }); });
}

#define GET_SAFE(optional, member)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_.member; })
#define GET_SAFEP(optional, member)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_->member; })
#define CALL(optional, member, ...)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_.member( __VA_ARGS__); })
#define CALLP(optional, member, ...)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_->member( __VA_ARGS__); })
#define APPLY(optional, name, ...)  apply(optional, [&](const auto &name)__VA_ARGS__)
#define APPLY2(optional1, optional2, name1, name2, codeBlock)  apply(optional1, optional2, [&](const auto &name1, const auto &name2)codeBlock)
#define APPLY3(optional1, optional2, optional3, name1, name2, name3, codeBlock)  apply(optional1, optional2, optional3, [&](const auto &name1, const auto &name2, const auto &name3)codeBlock)


/// static_pointer_cast
template<typename T, typename _Tp1>
inline Optional<T> static_optional_cast(const Optional<_Tp1> &optional) noexcept
{ return APPLY(optional, v, { return static_cast<T>(v); }); }

/// const_pointer_cast
template<typename T, typename _Tp1>
inline Optional<T> const_optional_cast(const Optional<_Tp1> &optional) noexcept
{ return APPLY(optional, v, { return const_cast<T>(v); }); }

/// dynamic_pointer_cast
template<typename T, typename _Tp1>
inline Optional<T> dynamic_optional_cast(const Optional<_Tp1> &optional) noexcept
{ return APPLY(optional, v, { return dynamic_cast<T>(v); }); }


/// static_pointer_cast
template<typename T, typename _Tp1>
inline Optional<std::shared_ptr<T>> static_optional_cast(const Optional<std::shared_ptr<_Tp1>> &optional) noexcept
{ return APPLY(optional, v, { return std::static_pointer_cast<T>(v); }); }

/// const_pointer_cast
template<typename T, typename _Tp1>
inline Optional<std::shared_ptr<T>> const_optional_cast(const Optional<std::shared_ptr<_Tp1>> &optional) noexcept
{ return APPLY(optional, v, { return std::const_pointer_cast<T>(v); }); }

/// dynamic_pointer_cast
template<typename T, typename _Tp1>
inline Optional<std::shared_ptr<T>> dynamic_optional_cast(const Optional<std::shared_ptr<_Tp1>> &optional) noexcept
{ return APPLY(optional, v, { return std::dynamic_pointer_cast<T>(v); }); }


#endif // OPTIONAL_H
