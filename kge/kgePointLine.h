/*-------------------------------*\
Copyright 2020 Kiyo Matsui
KiyosGameEngine v1.0 
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_POINTLINE_H
#define KGE_POINTLINE_H

#include <cmath>

#ifdef USED_FOR_KGE_TESTS
#define kgeTestPrint(x) std::cout << x << std::endl
#else
#define kgeTestPrint(x)
#endif

namespace kge {

template <typename T>
class point {
  static_assert(std::is_arithmetic<T>::value, "must be arithmetic type / useful fundamental type");

 public:
  point<T>() { kgeTestPrint("called no-args constructor"); }

  point<T>(T ax, T ay) : x(ax), y(ay) { kgeTestPrint("called constructor with arguments"); }

  point<T>(const point<T>& arg1) = default;
  point<T>& operator=(const point<T>& arg1) = default;
  point<T>(point<T>&& other) noexcept = default;
  point<T>& operator=(point<T>&& other) noexcept = default;
  ~point<T>() = default;

  T x{0};
  T y{0};

  point<T>& operator+=(const point<T>& arg1) {
    kgeTestPrint("called operator +=");
    this->x += arg1.x;
    this->y += arg1.y;
    return *this;
  }

  point<T>& operator+=(const T& arg1) {
    kgeTestPrint("called operator += for type");
    this->x += arg1;
    this->y += arg1;
    return *this;
  }

  point<T>& operator-=(const point<T>& arg1) {
    kgeTestPrint("called operator -=");
    this->x -= arg1.x;
    this->y -= arg1.y;
    return *this;
  }

  point<T>& operator-=(const T& arg1) {
    kgeTestPrint("called operator -= for type");
    this->x -= arg1;
    this->y -= arg1;
    return *this;
  }

  point<T>& operator*=(const point<T>& arg1) {
    kgeTestPrint("called operator *=");
    this->x *= arg1.x;
    this->y *= arg1.y;
    return *this;
  }

  point<T>& operator*=(const T& arg1) {
    kgeTestPrint("called operator *= for type");
    this->x *= arg1;
    this->y *= arg1;
    return *this;
  }

  point<T>& operator/=(const point<T>& arg1) {
    kgeTestPrint("called operator /=");
    this->x /= arg1.x;
    this->y /= arg1.y;
    return *this;
  }

  point<T>& operator/=(const T& arg1) {
    kgeTestPrint("called operator /= for type");
    this->x /= arg1;
    this->y /= arg1;
    return *this;
  }

  T length(const point<T>& arg1) const {
    kgeTestPrint("called length");
    point<T> a = *this - arg1;
    return (std::hypot(a.x, a.y));
  }
};

template <typename T>
point<T> operator+(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator +");
  return point<T>((arg1.x + arg2.x), (arg1.y + arg2.y));
}

template <typename T>
point<T> operator+(const point<T>& arg1, const T& arg2) {
  kgeTestPrint("called operator + with type");
  return point<T>((arg1.x + arg2), (arg1.y + arg2));
}

template <typename T>
point<T> operator-(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator -");
  return point<T>((arg1.x - arg2.x), (arg1.y - arg2.y));
}

template <typename T>
point<T> operator-(const point<T>& arg1, const T& arg2) {
  kgeTestPrint("called operator - with type");
  return point<T>((arg1.x - arg2), (arg1.y - arg2));
}

template <typename T>
point<T> operator*(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator *");
  return point<T>((arg1.x * arg2.x), (arg1.y * arg2.y));
}

template <typename T>
point<T> operator*(const point<T>& arg1, const T& arg2) {
  kgeTestPrint("called operator +  with type");
  return point<T>((arg1.x * arg2), (arg1.y * arg2));
}

template <typename T>
point<T> operator/(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator /");
  return point<T>((arg1.x / arg2.x), (arg1.y / arg2.y));
}

template <typename T>
point<T> operator/(const point<T>& arg1, const T& arg2) {
  kgeTestPrint("called operator / with type");
  return point<T>((arg1.x / arg2), (arg1.y / arg2));
}

template <typename T>
bool operator==(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator ==");
  if constexpr (std::is_floating_point_v<T>) {
    T a = arg1.x - arg2.x;
    T b = arg1.y - arg2.y;
    return ((std::abs(a) < 0.001) && (std::abs(b) < 0.001));
  }
  return (arg1.x == arg2.x && arg1.y == arg2.y);
}

template <typename T>
bool operator!=(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator !=");
  if constexpr (std::is_floating_point_v<T>) {
    T a = arg1.x - arg2.x;
    T b = arg1.y - arg2.y;
    return ((std::abs(a) > 0.001) || (std::abs(b) > 0.001));
  }
  return (arg1.x != arg2.x || arg1.y != arg2.y);
}

template <typename T>
bool operator>(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator >");
  return (arg1.x > arg2.x && arg1.y > arg2.y);
}

template <typename T>
bool operator<(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator <");
  return (arg1.x < arg2.x && arg1.y < arg2.y);
}

template <typename T>
bool operator>=(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator >=");
  return (arg1.x >= arg2.x && arg1.y >= arg2.y);
}

template <typename T>
bool operator<=(const point<T>& arg1, const point<T>& arg2) {
  kgeTestPrint("called operator <=");
  return (arg1.x <= arg2.x && arg1.y <= arg2.y);
}

template <typename T>
class line {
  static_assert(std::is_floating_point<T>::value, "must be floating point type");

 public:
  line<T>() { kgeTestPrint("called line constructor no arguments"); }

  line<T>(point<T> aA, point<T> aB) : A(aA), B(aB) { kgeTestPrint("called line constructor with arguments"); }

  line<T>(const line<T>& arg1) = default;
  line<T>& operator=(const line<T>& arg1) = default;
  line<T>(line&& other) noexcept = default;
  line<T>& operator=(line<T>&& other) noexcept = default;
  ~line<T>() = default;

  T length() const {
    kgeTestPrint("called line length");
    point<T> a = A - B;
    return (std::hypot(a.x, a.y));
  }

  bool intersect(const line<T>& arg1) {
    kgeTestPrint("called line intersect");
    point<T> w = this->A - arg1.A;
    point<T> r = arg1.B - arg1.A;
    point<T> s = this->B - this->A;
    T wxr = ((w.x * r.y) - (w.y * r.x));
    T wxs = ((w.x * s.y) - (w.y * s.x));
    T rxs = ((r.x * s.y) - (r.y * s.x));
    if (rxs < 0.0001 && rxs > -0.0001) {
      return false;
    }
    T u = wxr / rxs;
    T t = wxs / rxs;
    if (t <= 1.0 && t >= 0.0 && u <= 1.0 && u >= 0.0) {
      point<T> temp = s * u;
      recentIntersection = this->A + temp;
      return true;
    }
    return false;
  }

  point<T> A{0, 0};
  point<T> B{0, 0};
  point<T> recentIntersection{0, 0};
};

template <typename T>
line<T> operator+(const line<T>& arg1, const line<T>& arg2) {
  kgeTestPrint("called line operator +");
  point<T> temp = arg1.B + arg2.A;
  temp = temp - arg2.B;
  return line<T>(arg1.A, temp);
}

}  // namespace kge

#endif
