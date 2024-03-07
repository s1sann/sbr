#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <assert.h>

class MPoint3
{
private:
    double P[3];

public:
    MPoint3() { P[0] = P[1] = P[2] = 0; }

    MPoint3(double x, double y, double z) {
        P[0] = x;
        P[1] = y;
        P[2] = z;
    }
    MPoint3(const double* _p) {
        P[0] = _p[0];
        P[1] = _p[1];
        P[2] = _p[2];
    }
    MPoint3(const MPoint3& pt) {
        P[0] = pt.P[0];
        P[1] = pt.P[1];
        P[2] = pt.P[2];
    }

    virtual ~MPoint3() {}

    void set(double x, double y, double z) {
        P[0] = x;
        P[1] = y;
        P[2] = z;
    }
    void set(const MPoint3& pt, const MPoint3& dir, const double dist_) {
        P[0] = pt.P[0];
        P[1] = pt.P[1];
        P[2] = pt.P[2];

        MPoint3 tmp(dir);
        tmp *= dist_;
        P[0] += tmp[0];
        P[1] += tmp[1];
        P[2] += tmp[2];
    }

    inline double x(void) const { return P[0]; }
    inline double y(void) const { return P[1]; }
    inline double z(void) const { return P[2]; }

    double& operator[](int);
    double operator[](int) const;

    MPoint3& operator=(const MPoint3& p);
    void operator+=(const MPoint3& p);
    void operator-=(const MPoint3& p);
    void operator*=(double mul);
    void operator/=(double mul);

    MPoint3 operator*(double mul);
    operator double* () { return P; }

    const double* data() const { return P; }
    double* data() { return P; }

    double distance(const MPoint3& p) const;
};

inline double& MPoint3::operator[](int i) { return P[i]; }
inline double MPoint3::operator[](int i) const { return P[i]; }

inline MPoint3& MPoint3::operator=(const MPoint3& p) {
    P[0] = p.P[0];
    P[1] = p.P[1];
    P[2] = p.P[2];
    return *this;
}
inline void MPoint3::operator+=(const MPoint3& p) {
    P[0] += p.P[0];
    P[1] += p.P[1];
    P[2] += p.P[2];
}
inline void MPoint3::operator-=(const MPoint3& p) {
    P[0] -= p.P[0];
    P[1] -= p.P[1];
    P[2] -= p.P[2];
}
inline void MPoint3::operator*=(double mul) {
    P[0] *= mul;
    P[1] *= mul;
    P[2] *= mul;
}
inline void MPoint3::operator/=(double div) {
    P[0] /= div;
    P[1] /= div;
    P[2] /= div;
}

inline MPoint3 MPoint3::operator*(double mul) {
    return MPoint3(P[0] * mul, P[1] * mul, P[2] * mul);
}

inline double MPoint3::distance(const MPoint3& p) const {
    return std::sqrt((P[0] - p.P[0]) * (P[0] - p.P[0]) + (P[1] - p.P[1]) * (P[1] - p.P[1]) + (P[2] - p.P[2]) * (P[2] - p.P[2]));
}
inline double distance(const MPoint3& lhs, const MPoint3& rhs) {
    double x = lhs[0] - rhs[0], y = lhs[1] - rhs[1], z = lhs[2] - rhs[2];
    return std::sqrt(x * x + y * y + z * z);
}

inline MPoint3 operator+(const MPoint3& a, const MPoint3& b) {
    return MPoint3(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}
inline MPoint3 operator*(const MPoint3& a, const double& b) {
    return MPoint3(a.x() * b, a.y() * b, a.z() * b);
}
inline MPoint3 operator/(const MPoint3& a, const double& b) {
    assert(abs(b) > 1e-14);
    return MPoint3(a.x() / b, a.y() / b, a.z() / b);
}

inline std::ostream& operator<<(std::ostream& os, MPoint3& pt) {
    os << pt[0] << " " << pt[1] << " " << pt[2];
    return os;
}