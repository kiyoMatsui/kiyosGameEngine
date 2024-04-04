/*-------------------------------*\
Copyright 2020 Kiyo Matsui
KiyosGameEngine v2.1 
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_POINTLINE_H
#define KGE_POINTLINE_H
#include <cmath>
#include <iostream>

#ifdef USED_FOR_KGE_TESTS
#define kgeTestPrint(x) std::cout << x << std::endl
#else
#define kgeTestPrint(x)
#endif

namespace kge {

static constexpr float smalln = 0.001f;
static constexpr float smallern = 0.0001f;

template <typename Type>
class point {
  static_assert(std::is_arithmetic_v<Type>, "must be arithmetic type / useful fundamental type");

 public:
  point<Type>() { kgeTestPrint("called no-args constructor"); }

  point<Type>(Type ax, Type ay) : x(ax), y(ay) { kgeTestPrint("called constructor with arguments"); }

  // point<Type>(const point<Type>& arg1) = default;
  // point<Type>& operator=(const point<Type>& arg1) = default;
  // point<Type>(point<Type>&& other) noexcept = default;
  // point<Type>& operator=(point<Type>&& other) noexcept = default;

  Type x{0};
  Type y{0};

  point<Type>& operator+=(const point<Type>& arg1) {
    kgeTestPrint("called operator +=");
    this->x += arg1.x;
    this->y += arg1.y;
    return *this;
  }

  point<Type>& operator+=(const Type& arg1) {
    kgeTestPrint("called operator += for type");
    this->x += arg1;
    this->y += arg1;
    return *this;
  }

  point<Type>& operator-=(const point<Type>& arg1) {
    kgeTestPrint("called operator -=");
    this->x -= arg1.x;
    this->y -= arg1.y;
    return *this;
  }

  point<Type>& operator-=(const Type& arg1) {
    kgeTestPrint("called operator -= for type");
    this->x -= arg1;
    this->y -= arg1;
    return *this;
  }

  point<Type>& operator*=(const point<Type>& arg1) {
    kgeTestPrint("called operator *=");
    this->x *= arg1.x;
    this->y *= arg1.y;
    return *this;
  }

  point<Type>& operator*=(const Type& arg1) {
    kgeTestPrint("called operator *= for type");
    this->x *= arg1;
    this->y *= arg1;
    return *this;
  }

  point<Type>& operator/=(const point<Type>& arg1) {
    kgeTestPrint("called operator /=");
    this->x /= arg1.x;
    this->y /= arg1.y;
    return *this;
  }

  point<Type>& operator/=(const Type& arg1) {
    kgeTestPrint("called operator /= for type");
    this->x /= arg1;
    this->y /= arg1;
    return *this;
  }

  [[nodiscard]] Type length(const point<Type>& arg1) const {
    kgeTestPrint("called length");
    point<Type> a = *this - arg1;
    return (std::hypot(a.x, a.y));
  }
  [[nodiscard]] Type length() const {
    kgeTestPrint("called length");
    return (std::hypot(x, y));
  }
  [[nodiscard]] point<Type> unitVector() const {
    kgeTestPrint("called unitVector");
    return {x / std::hypot(x, y), y / std::hypot(x, y)};
  }
};

template <typename Type>
[[nodiscard]] Type area(const point<Type>& arg1, const point<Type>& arg2, const point<Type>& arg3) {
  kgeTestPrint("called area");
  return 0.5f*fabsf(arg1.x*(arg2.y-arg3.y)+arg2.x*(arg3.y-arg1.y)+arg3.x*(arg1.y-arg2.y));
}

template <typename Type>
point<Type> operator+(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator +");
  return point<Type>((arg1.x + arg2.x), (arg1.y + arg2.y));
}

template <typename Type>
point<Type> operator+(const point<Type>& arg1, const Type& arg2) {
  kgeTestPrint("called operator + with type");
  return point<Type>((arg1.x + arg2), (arg1.y + arg2));
}

template <typename Type>
point<Type> operator-(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator -");
  return point<Type>((arg1.x - arg2.x), (arg1.y - arg2.y));
}

template <typename Type>
point<Type> operator-(const point<Type>& arg1, const Type& arg2) {
  kgeTestPrint("called operator - with type");
  return point<Type>((arg1.x - arg2), (arg1.y - arg2));
}

template <typename Type>
point<Type> operator*(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator *");
  return point<Type>((arg1.x * arg2.x), (arg1.y * arg2.y));
}

template <typename Type>
point<Type> operator*(const point<Type>& arg1, const Type& arg2) {
  kgeTestPrint("called operator +  with type");
  return point<Type>((arg1.x * arg2), (arg1.y * arg2));
}

template <typename Type>
point<Type> operator/(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator /");
  return point<Type>((arg1.x / arg2.x), (arg1.y / arg2.y));
}

template <typename Type>
point<Type> operator/(const point<Type>& arg1, const Type& arg2) {
  kgeTestPrint("called operator / with type");
  return point<Type>((arg1.x / arg2), (arg1.y / arg2));
}

template <typename Type>
bool operator==(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator ==");
  if constexpr (std::is_floating_point_v<Type>) {
    Type a = arg1.x - arg2.x;
    Type b = arg1.y - arg2.y;
    return ((std::abs(a) < smalln) && (std::abs(b) < smalln));
  }
  return (arg1.x == arg2.x && arg1.y == arg2.y);
}

template <typename Type>
bool operator!=(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator !=");
  if constexpr (std::is_floating_point_v<Type>) {
    Type a = arg1.x - arg2.x;
    Type b = arg1.y - arg2.y;
    return ((std::abs(a) > smalln) || (std::abs(b) > smalln));
  }
  return (arg1.x != arg2.x || arg1.y != arg2.y);
}

template <typename Type>
bool operator>(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator >");
  return (arg1.x > arg2.x && arg1.y > arg2.y);
}

template <typename Type>
bool operator<(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator <");
  return (arg1.x < arg2.x && arg1.y < arg2.y);
}

template <typename Type>
bool operator>=(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator >=");
  return (arg1.x >= arg2.x && arg1.y >= arg2.y);
}

template <typename Type>
bool operator<=(const point<Type>& arg1, const point<Type>& arg2) {
  kgeTestPrint("called operator <=");
  return (arg1.x <= arg2.x && arg1.y <= arg2.y);
}

template <typename Type>
class intersectionResult {
 public:
  bool flag{false};
  point<Type> intersection{0, 0};
};

template <typename Type>
class line {
  static_assert(std::is_floating_point_v<Type>, "must be floating point type");

 public:
  line<Type>() { kgeTestPrint("called line constructor no arguments"); }

  line<Type>(point<Type> aA, point<Type> aB) : A(aA), B(aB) { kgeTestPrint("called line constructor with arguments"); }

  // line<Type>(const line<Type>& arg1) = default;
  // line<Type>& operator=(const line<Type>& arg1) = default;
  // line<Type>(line&& other) noexcept = default;
  // line<Type>& operator=(line<Type>&& other) noexcept = default;

  [[nodiscard]] Type length() const {
    kgeTestPrint("called line length");
    point<Type> a = A - B;
    return (std::hypot(a.x, a.y));
  }

  intersectionResult<Type> intersects(const line<Type>& arg1) {
    kgeTestPrint("called line intersects");
    point<Type> w = this->A - arg1.A;
    point<Type> r = arg1.B - arg1.A;
    point<Type> s = this->B - this->A;
    Type wxr = ((w.x * r.y) - (w.y * r.x));
    Type wxs = ((w.x * s.y) - (w.y * s.x));
    Type rxs = ((r.x * s.y) - (r.y * s.x));
    if ((rxs < smallern && rxs > (-1.0f * smallern)) || rxs == 0.0f) {
      return {false, {0.0f, 0.0f}};
    }
    Type u = wxr / rxs;
    Type t = wxs / rxs;
    if (t <= 1.0f && t >= 0.0f && u <= 1.0f && u >= 0.0f) {
      point<Type> temp = s * u;
      // recentIntersection = this->A + temp;
      return {true, this->A + temp};
    }
    return {false, {0.0f, 0.0f}};
  }

  intersectionResult<Type> intersects(const point<Type>& arg1, Type distance) {
    kgeTestPrint("called line intersects with point at distance");
    Type dx = this->A.x - this->B.x;
    Type dy = this->A.y - this->B.y;
    Type scale = std::hypot(dx, dy) / distance;
    dx /= scale;
    dy /= scale;
    point<Type> pointDx((arg1.x + dy), (arg1.y - dx));
    point<Type> pointDy((arg1.x - dy), (arg1.y + dx));
    return intersects(line<Type>(pointDx, pointDy));
  }

  point<Type> A{0, 0};
  point<Type> B{0, 0};
};

template <typename Type>
line<Type> operator+(const line<Type>& arg1, const line<Type>& arg2) {
  kgeTestPrint("called line operator +");
  point<Type> temp = arg1.B + arg2.A;
  temp = temp - arg2.B;
  return line<Type>(arg1.A, temp);
}

}  // namespace kge

#endif
