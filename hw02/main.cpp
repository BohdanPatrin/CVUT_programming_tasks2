#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <span>
#include <algorithm>
#include <memory>
#include <compare>
#include <complex>
#endif /* __PROGTEST__ */

std::ios_base & ( * poly_var ( const std::string & name ) ) ( std::ios_base & x )
{
    return [] ( std::ios_base & ios ) -> std::ios_base & { return ios; };
}

const double PI = acos(-1); // pi constant for complex roots of unity - needed in FFT algorithm

// recursive FFT implementation
void FFT(std::vector<std::complex<double>> & a, bool invert) {
    unsigned long long int n = a.size();
    if (n == 1) return;

    // split elements in odd and even
    std::vector<std::complex<double>> a_even(n / 2), a_odd(n / 2);
    for (unsigned long long int i = 0; i < n / 2; i++) {
        a_even[i] = a[i * 2];
        a_odd[i] = a[i * 2 + 1];
    }

    // recursion
    FFT(a_even, invert);
    FFT(a_odd, invert);

    // сompute FFT results for current stage
    double angle = 2 * PI / (double )n * (invert ? -1 : 1);
    std::complex<double> w(1), wn(cos(angle), sin(angle));

    for (unsigned long long int i = 0; i < n / 2; i++) {
        std::complex<double> temp = w * a_odd[i];
        a[i] = a_even[i] + temp;
        a[i + n / 2] = a_even[i] - temp;
        if (invert) {
            a[i] /= 2;
            a[i + n / 2] /= 2;
        }
        w *= wn;
    }
}

std::vector<double> multiplyBigPol(const std::vector<double> &A, const std::vector<double> &B) {
    unsigned long long int n = 1;
    while (n < A.size() + B.size()) n *= 2; // round up to power of 2

    // extend vectors to power of 2 with zero padding
    std::vector<std::complex<double>> FA(n), FB(n);
    for (size_t i = 0; i < A.size(); i++) FA[i] = A[i];
    for (size_t i = 0; i < B.size(); i++) FB[i] = B[i];

    FFT(FA, false);
    FFT(FB, false);

    // multiply in frequency domain
    for (unsigned long long int i = 0; i < n; i++) FA[i] *= FB[i];

    FFT(FA, true);

    // extract real coefficients
    std::vector<double> result(n);
    for (unsigned long long int i = 0; i < n; i++) result[i] = (std::abs(FA[i].real()) < 1e-9) ? 0.0 : std::round(FA[i].real() * 1e6) / 1e6;

    // remove trailing zeroes
    while (result.size() > 1 && result.back() == 0) result.pop_back();

    return result;
}



std::vector<double> multiplySmallPol(const std::vector<double> &A, const std::vector<double> &B){ //naive
    size_t n = A.size(), m = B.size();
    std::vector<double> result(n + m - 1, 0.0);  // result - polynomial of degree (n+m-2)

    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j) result.at(i + j) += A.at(i) * B.at(j);

    return result;
}



//functions needed for Karatsuba algorithm

std::vector<double> addPolynomials(const std::vector<double>& A, const std::vector<double>& B) {
    size_t n = std::max(A.size(), B.size());
    std::vector<double> res(n, 0.0);

    for (size_t i = 0; i < A.size(); ++i) res[i] += A[i];
    for (size_t i = 0; i < B.size(); ++i) res[i] += B[i];

    return res;
}

std::vector<double> subtractPolynomials(const std::vector<double>& A, const std::vector<double>& B) {
    size_t n = std::max(A.size(), B.size());
    std::vector<double> res(n, 0.0);

    for (size_t i = 0; i < A.size(); ++i) res[i] += A[i];
    for (size_t i = 0; i < B.size(); ++i) res[i] -= B[i];

    return res;
}

std::vector<double> multiplyMedPol(const std::vector<double> &A, const std::vector<double> &B){ //Karatsuba
    long long n = A.size();

    // base case - if polynomial is small, use naive multiplication
    if (n <= 2) return multiplySmallPol(A,B);

    long long newSize = 1;
    while (newSize < (long long) A.size() || newSize < (long long) B.size())
        newSize *= 2; //it should be power of 2

    std::vector<double> A_padded = A;
    std::vector<double> B_padded = B;
    A_padded.resize(newSize, 0.0);
    B_padded.resize(newSize, 0.0);
    long long half = newSize / 2;

    // split A and B into low and high
    std::vector<double> A_low(A_padded.begin(), A_padded.begin() + half);
    std::vector<double> A_high(A_padded.begin() + half, A_padded.end());

    std::vector<double> B_low(B_padded.begin(), B_padded.begin() + half);
    std::vector<double> B_high(B_padded.begin() + half, B_padded.end());

    // recursion
    std::vector<double> P1 = multiplyMedPol(A_low, B_low);
    std::vector<double> P2 = multiplyMedPol(A_high, B_high);

    std::vector<double> sumA = addPolynomials(A_low, A_high);
    std::vector<double> sumB = addPolynomials(B_low, B_high);
    std::vector<double> P3 = multiplyMedPol(sumA, sumB);

    // P3 - P1 - P2 gives the cross terms
    std::vector<double> P3_sub = subtractPolynomials(P3, P1);
    P3_sub = subtractPolynomials(P3_sub, P2);

    std::vector<double> res(2 * n - 1, 0.0);

    // merge results
    for (size_t i = 0; i < P1.size(); ++i) res[i] += P1[i];
    for (size_t i = 0; i < P3_sub.size(); ++i) res[i + half] += P3_sub[i];
    for (size_t i = 0; i < P2.size(); ++i) res[i + newSize] += P2[i];

    return res;
}

class CPolynomial
{
public:
    [[nodiscard]]long long int degree() const{
        int degree = 0;
        for (int i = (int)coeffs.size(); i > 0; --i)
            if (coeffs.at(i-1) != 0.0) {
                degree = i-1;
                break;
            }

        return degree;
    }

    explicit operator bool() const{
        return degree() > 0 || coeffs.at(0) != 0;
    }

    CPolynomial() : coeffs() {}
    CPolynomial(std::vector<double> c): coeffs(std::move(c)){};
    ~CPolynomial() = default;
    CPolynomial(const CPolynomial&) = default;
    CPolynomial& operator=(const CPolynomial&) = default;

    friend CPolynomial operator*(const CPolynomial& lhs, const CPolynomial& rhs){
        if(!static_cast<bool>(lhs) || !static_cast<bool>(rhs))
            return CPolynomial{std::vector<double>{0} };

        long long int lhsdeg = lhs.degree();
        long long int rhsdeg = rhs.degree();
        if(lhsdeg >= 100000 || rhsdeg >= 100000)
           return multiplyBigPol(lhs.coeffs, rhs.coeffs);
        else if(lhsdeg > 2 || rhsdeg > 2)
            return multiplyMedPol(lhs.coeffs, rhs.coeffs);
        else
            return multiplySmallPol(lhs.coeffs, rhs.coeffs);
    }

    CPolynomial operator*(const double x) const{
        if(x == 0 || !static_cast<bool>(*this))
            return CPolynomial{std::vector<double>{0} };

        std::vector<double> res_coefs = coeffs;
        for (double& coeff : res_coefs) coeff *= x;

        return CPolynomial{res_coefs};
    };

    CPolynomial operator*=(const CPolynomial& other){
        *this = *this * other;
        return *this;
    }

    CPolynomial operator*=(const double x){
        if(x == 0)
            coeffs.clear();

        else if(static_cast<bool> (*this)) //polynomial is not zero
            for (double& coeff : coeffs) coeff *= x;
        return *this;
    }

    bool operator==(const CPolynomial& other) const{
        long long int deg = degree();
        long long int otherdeg = other.degree();
        if(deg != otherdeg) return false; //if their max degree are different - obviously the polynomials are different
        size_t maxDeg = std::max(deg, otherdeg);

        for (size_t i = maxDeg; i > 0; --i) {
            double c1 = (i <= coeffs.size()) ? coeffs.at(i - 1) : 0.0;
            double c2 = (i <= other.coeffs.size()) ? other.coeffs.at(i - 1) : 0.0;
            if (c1 != c2) return false;
        }
        return true;
    }

    bool operator!=(const CPolynomial& other) const{
        return !(*this == other);
    };

    double& operator[](size_t index){
        if (index >= coeffs.size()) coeffs.resize(index + 1, 0.0);
        return coeffs.at(index);
    }

    double operator[] (unsigned int index) const {
        if (index < coeffs.size()) return coeffs.at(index);
        return 0.0;
    }


    double operator()(double x) const{ //Horner's algortihm
        long long int deg = degree();
        if(coeffs.empty() || (deg == 0 && coeffs.at(0) == 0.0)) return 0.0;

        double result = coeffs.at(deg);
        for(long long i = deg; i > 0; i--){
            result*=x;
            result+=coeffs.at(i-1);
        }
        return result;
    };

    friend std::ostream& operator<<(std::ostream& os, const CPolynomial& poly){
        bool firstTerm = true;

        if (poly.coeffs.empty()) {
            os << "0";
            return os;
        }

        for (int i = poly.coeffs.size()-1; i >= 0; i--) {
           double coef = poly.coeffs.at(i);
           if(coef == 0) continue;

           if(!firstTerm)
               os << (coef > 0 ? " + " : " - ");
           else
               if(coef < 0) os << "- ";

           coef = std::abs(coef);
           if(i == 0 && coef != 1) os << coef;
           else if(i != 0 && coef == 1) os << "x^" << i;
           else if(i != 0 && coef != 1) os << coef << "*x^" << i;

           firstTerm = false;
        }


        return os;
    }

private:
    // todo
    std::vector<double> coeffs;
};

#ifndef __PROGTEST__
bool smallDiff(double a, double b) {
    const double epsilon = 1e-4;
    return std::abs(a - b) <= epsilon * std::max(1.0, std::max(std::abs(a), std::abs(b)));
}

bool dumpMatch(CPolynomial &x, const std::vector<double> &ref) {
    if (x.degree() != static_cast<int>(ref.size()) - 1)
        return false;

    for (size_t i = 0; i < ref.size(); ++i) {
        if (!smallDiff(x[i], ref[i])) {
            return false;
        }
    }
    return true;
}

int main ()
{
    CPolynomial a, b, c;
    std::ostringstream out, tmp;

    a[0] = -10;
    a[1] = 3.5;
    a[3] = 1;
    assert ( smallDiff ( a ( 2 ), 5 ) );


    out . str ("");
    out << a;
    assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
    c = a * -2;
    assert ( c . degree () == 3
             && dumpMatch ( c, std::vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

    out . str ("");
    out << c;
    assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
    out . str ("");
    out << b;
    assert ( out . str () == "0" );
    b[5] = -1;
    b[2] = 3;
    out . str ("");
    out << b;
    assert ( out . str () == "- x^5 + 3*x^2" );
    c = a * b;
    assert ( c . degree () == 8
             && dumpMatch ( c, std::vector<double>{ -0.0, -0.0, -30.0, 10.5, -0.0, 13.0, -3.5, 0.0, -1.0 } ) );

    out . str ("");
    out << c;
    assert ( out . str () == "- x^8 - 3.5*x^6 + 13*x^5 + 10.5*x^3 - 30*x^2" );
    a *= 5;
    assert ( a . degree () == 3
             && dumpMatch ( a, std::vector<double>{ -50.0, 17.5, 0.0, 5.0 } ) );

    a *= b;
    assert ( a . degree () == 8
             && dumpMatch ( a, std::vector<double>{ 0.0, 0.0, -150.0, 52.5, -0.0, 65.0, -17.5, -0.0, -5.0 } ) );

    assert ( a != b );
    b[5] = 0;
    assert ( static_cast<bool> ( b ) );
    assert ( ! ! b );
    b[2] = 0;
    assert ( !(a == b) );
    a *= 0;
    assert ( a . degree () == 0
             && dumpMatch ( a, std::vector<double>{ 0.0 } ) );

    assert ( a == b );
    assert ( ! static_cast<bool> ( b ) );
    assert ( ! b );

    // bonus - manipulators
/*
    out . str ("");
    out << poly_var ( "y" ) << c;
    assert ( out . str () == "- y^8 - 3.5*y^6 + 13*y^5 + 10.5*y^3 - 30*y^2" );
    out . str ("");
    tmp << poly_var ( "abc" );
    out . copyfmt ( tmp );
    out << c;
    assert ( out . str () == "- abc^8 - 3.5*abc^6 + 13*abc^5 + 10.5*abc^3 - 30*abc^2" );

*/


    CPolynomial p1;
    p1[3] = 5;
    p1 = p1; // Self-assignment
    assert(p1(1) == 5);
    assert(p1(2) == 5 * 8); // 5 * 2^3
    p1[3] = 0;

    CPolynomial p2;
    p1[2] = 3;
    p2 = p1;
    p2[2] = 10;

    assert(p1(2) == 3 * 4); // p1 must remain unchanged
    assert(p2(2) == 10 * 4); // p2 should be different from p1

    CPolynomial p;
    p[0] = 5; // Only constant term
    out . str ("");
    out << p;
    assert(out.str() == "5");

    p[1] = -3; // Single variable term
    std::ostringstream out2;
    out2 << p;
    assert(out2.str() == "- 3*x^1 + 5");

    CPolynomial p4, p5, p6;
    p4[1] = 2;
    p5[2] = 3;
    p6 = p5 = p4;

    assert(p4(1) == 2);
    assert(p5(1) == 2);
    assert(p6(1) == 2);

    CPolynomial p3, p7;
    p3[4] = 7;
    p7 = p3;
    p3 = CPolynomial(); // Assign empty polynomial

    assert(p3(3) == 0);
    assert(p7(3) == 7 * 81); // p2 should still be correct

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
