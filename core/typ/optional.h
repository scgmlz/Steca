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

/**
*  @brief Simple Optional object.
*  @author Joachim Coenen
* Any instance of Optional<T> at any given point in time either contains a value or does not contain a value (isEmpty).
*/
template<typename T>
class Optional {
public:
    typedef T value_type;

    Optional() : ptr_(nullptr) { }
    Optional(EmptyOptional) : ptr_(nullptr) { }
    Optional(const T &value) : ptr_(new T {value}) { }
    Optional(const T &&value) : ptr_(new T(std::move(value))) { }
    Optional(T *value) : ptr_(value) { }

    inline auto &operator=(const T &rhs) { setValue_(rhs); return *this; }
    inline auto &operator=(const T &&rhs) { setValue_(std::move(rhs)); return *this; }

    inline auto &operator=(const Optional &rhs)
    {
        ptr_ = rhs.ptr_; // ptr_ is std::shared_ptr<T>
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

    //! performs NO checks wether optional actually has a value ot not. UNSAFE!
    inline T &getValue_() { return *ptr_; }

    //! performs NO checks wether optional actually has a value ot not. UNSAFE!
    inline const T &getValue_() const { return *ptr_; }

    inline void setValue_(const T &value)
    {
        ptr_ = new T {value};
    }
};


//! applies a function func to the value of the optional, if the optional isn't empty, and returns the result.
//! pseudo code: return optional.isEmpty() ? Empty : Optional(func(*optional))
template<typename F, typename Optional>
inline typename make_optional<typename std::result_of<F(typename Optional::value_type)>::type>::type apply(Optional optional, F func) {
    return optional.apply(func);
}

//! applies a function to the values of multiple optionals, if none of the optionals is empty, and returns the result.
//! pseudo code:
//! if optional1.isEmpty() or optional2.isEmpty() or... :
//!     return Empty
//! else:
//!     return Optional(func(*optional1, *optional2, ...))
template<typename F, typename Optional1, typename Optional2>
inline typename make_optional<typename std::result_of<F(typename Optional1::value_type, typename Optional2::value_type)>::type>::type apply(Optional1 optional1, Optional2 optional2, F func) {
    return apply(optional1, [&](const auto &v1){return apply(optional2, [&](const auto &v2){ return func(v1, v2); }); });
}

//! applies a function to the values of multiple optionals, if none of the optionals is empty, and returns the result.
//! pseudo code:
//! if optional1.isEmpty() or optional2.isEmpty() or... :
//!     return Empty
//! else:
//!     return Optional(func(*optional1, *optional2, ...))
template<typename F, typename Optional, typename... Optionals>
inline typename make_optional<typename std::result_of<F(typename Optional::value_type, typename Optionals::value_type...)>::type>::type apply(Optional optional, Optionals... optionals, F func) {
    return apply(optional, [&](const auto &v){ return apply(optionals..., [&](typename Optionals::value_type... values){ return func(v, values...); }); });
}

//! returns a member of the optional value, or an empty optional if it was empty (Empty->Empty; Value->Value.member)
#define GET_SAFE(optional, member)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_.member; })
//! returns a member of the optional pointer value, or an empty optional if it was empty (Empty->Empty; Value->Value.member)
#define GET_SAFEP(optional, member) apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_->member; })
//! calls a member of the optional value, if the optional isn't empty. If the optional is empty, it returns an empty Optional, otherwise the return value wrapped in an Optional.
//! pseudo code: return optional.isEmpty() ? Empty : Optional(optional->member(args...))
#define CALL(optional, member, ...)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_.member( __VA_ARGS__); })
//! calls a member of the optional value pointer, if the optional isn't empty. If the optional is empty, it returns an empty Optional, otherwise the return value wrapped in an Optional.
//! pseudo code: return optional.isEmpty() ? Empty : Optional(optional->member(args...))
#define CALLP(optional, member, ...)  apply(optional, [&](const auto &value_9hnm0cw_){ return value_9hnm0cw_->member( __VA_ARGS__); })

//! applies a function to the value of the optional, if the optional isn't empty, and returns the result.
//! see apply() for more info
#define APPLY(optional, name, ...)  apply(optional, [&](const auto &name)__VA_ARGS__) // __VA_ARGS__ required, due to the limitaions of macros
//! applies a function to the values of multiple optionals, if none of the optionals is empty, and returns the result.
//! see apply() for more info
#define APPLY2(optional1, optional2, name1, name2, ...))  apply(optional1, optional2, [&](const auto &name1, const auto &name2)__VA_ARGS__)
//! applies a function to the values of multiple optionals, if none of the optionals is empty, and returns the result.
//! see apply() for more info
#define APPLY3(optional1, optional2, optional3, name1, name2, name3, ...))  apply(optional1, optional2, optional3, [&](const auto &name1, const auto &name2, const auto &name3)__VA_ARGS__)

//! static_cast for Optinal<>
template<typename T, typename _Tp1>
inline Optional<T> static_optional_cast(const Optional<_Tp1> &optional) noexcept
{ return APPLY(optional, v, { return static_cast<T>(v); }); }

/// const_cast for Optinal<>
template<typename T, typename _Tp1>
inline Optional<T> const_optional_cast(const Optional<_Tp1> &optional) noexcept
{ return APPLY(optional, v, { return const_cast<T>(v); }); }

/// dynamic_cast for Optinal<>
template<typename T, typename _Tp1>
inline Optional<T> dynamic_optional_cast(const Optional<_Tp1> &optional) noexcept
{ return APPLY(optional, v, { return dynamic_cast<T>(v); }); }


/// static_cast for Otional of std::shared_ptr
template<typename T, typename _Tp1>
inline Optional<std::shared_ptr<T>> static_optional_cast(const Optional<std::shared_ptr<_Tp1>> &optional) noexcept
{ return APPLY(optional, v, { return std::static_pointer_cast<T>(v); }); }

/// const_cast for Otional of std::shared_ptr
template<typename T, typename _Tp1>
inline Optional<std::shared_ptr<T>> const_optional_cast(const Optional<std::shared_ptr<_Tp1>> &optional) noexcept
{ return APPLY(optional, v, { return std::const_pointer_cast<T>(v); }); }

/// dynamic_cast for Otional of std::shared_ptr
template<typename T, typename _Tp1>
inline Optional<std::shared_ptr<T>> dynamic_optional_cast(const Optional<std::shared_ptr<_Tp1>> &optional) noexcept
{ return APPLY(optional, v, { return std::dynamic_pointer_cast<T>(v); }); }


#endif // OPTIONAL_H
