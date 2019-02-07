/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.7
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_POINT_H
#define KGE_POINT_H

#include <cmath>

#ifdef USED_FOR_KGE_TESTS
#define kgeTestPrint(x) std::cout << x << std::endl
#else
#define kgeTestPrint(x)
#endif

namespace kge {

template <typename T> class point {
  static_assert(std::is_arithmetic<T>::value,
                "must be arithmetic type / useful fundamental type");

public:
  point<T> operator=(const point<T> &arg1) {
    kgeTestPrint("called operator =");
    return point<T>((this->x = arg1.x), (this->y = arg1.y));
  }

  point<T> &operator+=(const point<T> &arg1) {
    kgeTestPrint("called operator +=");
    this->x += arg1.x;
    this->y += arg1.y;
    return *this;
  }

  point<T> &operator+=(const T &arg1) {
    kgeTestPrint("called operator += for type");
    this->x += arg1;
    this->y += arg1;
    return *this;
  }

  point<T> &operator*=(const point<T> &arg1) {
    kgeTestPrint("called operator *=");
    this->x *= arg1.x;
    this->y *= arg1.y;
    return *this;
  }

  point<T> &operator*=(const T &arg1) {
    kgeTestPrint("called operator * for type");
    this->x *= arg1;
    this->y *= arg1;
    return *this;
  }

  point<T> &operator-=(const point<T> &arg1) {
    kgeTestPrint("called operator -=");
    this->x -= arg1.x;
    this->y -= arg1.y;
    return *this;
  }

  point<T> &operator-=(const T &arg1) {
    kgeTestPrint("called operator -= for type");
    this->x -= arg1;
    this->y -= arg1;
    return *this;
  }

  point() { kgeTestPrint("called no-args constructor"); }
  point(T ax, T ay) : x(ax), y(ay) {
    kgeTestPrint("called constructor with arguments");
  }
  point(const point<T> &arg1) : x(arg1.x), y(arg1.y) {
    kgeTestPrint("called copy constructor");
  }
  T x;
  T y;
};

template <typename T>
point<T> operator+(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator +");
  return point<T>((arg1.x + arg2.x), (arg1.y + arg2.y));
}

template <typename T>
point<T> operator-(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator -");
  return point<T>((arg1.x - arg2.x), (arg1.y - arg2.y));
}

template <typename T>
bool operator==(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator ==");
  if constexpr (std::is_floating_point_v<T>) {
    T a = arg1.x - arg2.x;
    T b = arg1.y - arg2.y;
    return ((std::abs(a) < 0.001) && (std::abs(b) < 0.001));
  } else {
    return (arg1.x == arg2.x && arg1.y == arg2.y);
  }
}

template <typename T>
bool operator!=(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator !=");
  if constexpr (std::is_floating_point_v<T>) {
    T a = arg1.x - arg2.x;
    T b = arg1.y - arg2.y;
    return ((std::abs(a) > 0.001) || (std::abs(b) > 0.001));
  } else {
    return (arg1.x != arg2.x || arg1.y != arg2.y);
  }
}

template <typename T>
bool operator>(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator >");
  return (arg1.x > arg2.x && arg1.y > arg2.y);
}

template <typename T>
bool operator<(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator <");
  return (arg1.x < arg2.x && arg1.y < arg2.y);
}

template <typename T>
bool operator>=(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator >=");
  return (arg1.x >= arg2.x && arg1.y >= arg2.y);
}

template <typename T>
bool operator<=(const point<T> &arg1, const point<T> &arg2) {
  kgeTestPrint("called operator <=");
  return (arg1.x <= arg2.x && arg1.y <= arg2.y);
}

} // namespace kge

#endif
