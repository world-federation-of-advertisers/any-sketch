//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Copyright 2022 The Cross-Media Measurement Authors

#include <cmath>
#include <cstdlib>
#include <limits>
#include <type_traits>

#ifndef LLVM_EXPINT_H_
#define LLVM_EXPINT_H_

namespace llvm {

namespace {

constexpr double EULER_GAMMA_VALUE =
    0.57721566490153286060651209008240243104215933593992l;

// Returns the Euler-Mascheroni constant.
//
// The value is taken from:
// Sweeney, Dura W. "On the computation of Euler’s constant."
// Mathematics of Computation 17.82 (1963): 170-178.
//
// returns the Euler-Mascheroni constant gamma = 0.5772156649
template <class Real>
inline Real euler_gamma() {
  return Real(EULER_GAMMA_VALUE);
}

// Evaluates expint(x) for x < -1.
//
// This function evaluates the following expression:
// \f[
// \mathrm{Ei} (x) = -e^{x} \left(
// \frac{1}{x+1-}\frac{1}{x+3-}\frac{4}{x+5-} \cdots\right)
// \f]
//
// See Press, William H., et al. Numerical recipes 3rd edition: The art of
// scientific computing. Cambridge university press, 2007, p. 267., eq. (6.3.5)
//
// The implementation uses two iterations of the form
// \f[
// s^{(i)} = 1 - x + 2i - \frac{i^2}{s^{(i-1)}}
// \f]
// with different start values \f$ s_0^{(1)} = 1 -x + 2 \f$ and
// \f$ s_1^{(1)} = 1 -x + 2 - \frac{1}{1-x} \f$.
//
// Both of them are bound by
// \f$ 1 - x +i < s^{(i)} \leq 1 - x + 2i \f$.
//
// Therefore, the difference monotonically decreasing
// \f[
// s_0^{(i)} - s_1^{(i)} = \frac{i^2}{s_0^{(i-1)} s_1^{(i-1)}}
// \left( s_0^{(i)} - s_1^{(i)} \right).
// \f]
//
// param: [in] x Argument \f$ x < -1\f$ of \f$\mathrm{Ei} (x) \f$.
// return: continued fraction representation of \f$ \mathrm{Ei} (x) \f$
// for \f$ x < -1 \f$
template <class Real>
inline Real expint_continued_fraction(Real x) {
  const int max_iter = 1000;
  const Real mxp1 = -x + Real(1);
  if (std::isinf(mxp1 + Real(2 * max_iter))) return Real(0);

  // See algorithm expint.h on p. 268 of
  // Press, William H., et al. Numerical recipes 3rd edition: The art of
  // scientific computing. Cambridge university press, 2007.
  // a -> -t0
  // b -> t1
  // c -> s0
  // d -> 1/s1

  Real h = Real(1) / mxp1;
  Real s0 = mxp1 + Real(2);
  Real s1 = s0 - h;
  h *= s0 / s1;

  for (int i = 2; i < max_iter; ++i) {
    const Real t0 = Real(i * i);
    const Real t1 = mxp1 + Real(2 * i);
    s0 = t1 - t0 / s0;
    s1 = t1 - t0 / s1;
    if (s0 == s1) return -h * exp(x);

    h *= s0 / s1;
  }

  // (See the estimates in the comment above.)
  // The closer x is to 0, the slower the convergence.
  // We use this function only for x < -1, which makes
  // x == -1 the worst case.

  // never reach here
  __builtin_unreachable();
}

// Evaluates expint(x) for x != 0.
//
// This function evaluates the following expression:
//
// \f[
// \mathrm{Ei} (x) = \gamma + \ln \left|x\right| + \sum \limits_{n=1}^{\infty}
// \frac{x^n}{n\cdotn!}
// \f]
//
// See Press, William H., et al. Numerical recipes 3rd edition: The art of
// scientific computing. Cambridge university press, 2007, p. 267., eq. (6.3.6)
// or
// Harris, Frank E. "Tables of the exponential integral Ei (x)."
// Mathematical Tables and Other Aids to Computation 11.57 (1957): 9-16.
//
// attention: This representation converges slowly for large
// \f$ \left|x\right| \f$!

// param: [in] x Argument \f$ x \neq 0 \f$ of \f$\mathrm{Ei} (x) \f$.
// return: Evaluation of a series representation of \f$ \mathrm{Ei} (x) \f$
// for \f$ -1 < x < 1 \f$.
template <class Real>
inline Real expint_power_series(Real x) {
  Real sum = Real(0);
  Real k = Real(1);
  Real factor = Real(1);
  for (;;) {
    factor *= x / k;
    const Real prev = sum;
    sum += factor / k;
    if (prev == sum) return sum + euler_gamma<Real>() + std::log(std::abs(x));

    k += Real(1);
  }
}

// Evaluates expint(x) for x > -std::log(
// std::numeric_limits<T>::epsilon()).
//
// This function evaluates the following expression:
// \f[
// \mathrm{Ei} (x) \approx \frac{\exp (x)}{x} \left(
// \sum \limits_{k=0}^{n-1} \frac{k!}{x^k} + R_{n} (x) \right).
// \f]
// where \f$ n = \lceil x \rceil - 1\f$.
// The remainder \f$ R_{n} (x) \f$ is bound by
// \f$ 0 \leq R_{n} (x) \leq \frac{n!}{n^n} \f$.
//
// Since all the summands are positive and the sum is greater than 1,
// one can introduce a cutoff condition at \f$ R_{n} (x) < \epsilon \f$.
// Using Stirling's approximation, one can see that this condition fulfilled
// for \f$ n > - \log \epsilon \f$
// \f[
// \frac{ R_{n} (x)}{\mathrm{Ei} (x)} \geq .
// \f]
//
// See eqs (2.3) and (2.4) from:
// Cody, W., & Thacher, H. (1969).
// Chebyshev Approximations for the Exponential Integral Ei(x).
// Mathematics of Computation, 23(106), 289-303. doi:10.2307/2004423
// or eq. (6.3.11) from:
// Press, William H., et al. Numerical recipes 3rd edition: The art of
// scientific computing. Cambridge university press, 2007, p. 269.
//
// param: Real float, double, or long double
// param: x Argument \f$ x \gg 1 \f$ of \f$\mathrm{Ei} (x) \f$.
// return: Evaluation of an asymptotic series representation of
// \f$ \mathrm{Ei} (x) \f$ for \f$ x \gg 1 \f$.
template <class Real>
inline Real expint_asymptotic(Real x) {
  Real sum = Real(0);
  Real t = Real(1);
  Real k = Real(1);
  do {
    t *= k / x;
    sum += t;
    k += Real(1);
  } while (k < x && t > std::numeric_limits<Real>::epsilon());

  const Real overflow_limit = (Real(1) - std::numeric_limits<Real>::epsilon()) *
                              std::log(std::numeric_limits<Real>::max());

  if (x >= overflow_limit) {
    if (std::isinf(x)) {
      return std::numeric_limits<Real>::infinity();
    }
    // The first term is actually exp(x)/x in disguise.
    // It is less precise but isn't subject to premature overflow.
    return std::exp(x - std::log(x)) * (Real(1) + sum);
  }

  return std::exp(x) / x * (Real(1) + sum);
}

// Internal implementation of std::expint.
//
// The implementation is based on chapter 6.3, p. 266 et seqq. of
// Press, William H., et al. Numerical recipes 3rd edition: The art of
// scientific computing. Cambridge university press, 2007
// param Real float, double, or long double
// param: x Argument \f$ x \gg 1 \f$ of \f$\mathrm{Ei} (x) \f$.
// return: expint(x).
template <class Real>
inline Real expint_impl(Real x) {
  if (x < Real(-1)) return expint_continued_fraction(x);

  const Real asymptotic = -std::log(std::numeric_limits<Real>::epsilon());
  if (x < asymptotic) return expint_power_series(x);

  return expint_asymptotic(x);
}

inline double expint(double x) { return expint_impl<double>(x); }

inline float expintf(float x) { return expint_impl<float>(x); }

inline long double expintl(long double x) {
  return expint_impl<long double>(x);
}

}  // namespace
}  // namespace llvm

#endif  // LLVM_EXPINT_H_
