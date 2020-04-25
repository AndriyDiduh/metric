/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2018 Michael Welsch
*/

#include "standards.hpp"
#include "../../../3rdparty/blaze/Blaze.h" // blaze::norm

#include <cmath> // std::nan, std::sqrt

namespace metric {

// Euclidean

template <typename RT>
template <typename R>
auto Euclidean<RT>::operator()(const R& a, const R& b) const -> DistanceType {
    if (size(a) != size(b)) {
        const DistanceType dnan = std::nan("Input container sizes do not match");
        return dnan;
    }
    DistanceType sum = 0;
    for (auto [a1, b1] : zip(a, b)) {
        sum += (a1 - b1) * (a1 - b1);
    }
    return std::sqrt(sum);
}

// works on Matrices and Vectors (blaze)
//template <typename RT>
//template <typename R>
//isBlazeCompatible<R>
//auto Euclidean<RT>::operator()(const R & a, const R & b) const -> DistanceType {
//     FIXME: matrices
//    if (a.size() != b.size()) { // no .size method in martices, TODO restore this check!
//        DistanceType dnan = std::nan("Input container sizes do not match");
//        return dnan;
//    }
//    return blaze::norm(a - b); // slower than the iterator-based approach above
//}

// NOTE: we can not tell apart Blaze DynamicVector and CompressedMatrix via signature,
// but processing CompressedMatrix via iterators will skip zero elements and result in wring output,
// so we use Blaze norm function for both Blaze types.
// Although, processing DynamicVector via iterators is correct and much faster than via norm function. So
// TODO add DynamicVector type detector and process this type apart

//template <typename RT>
//template <typename R>
//isEigenCompatible<R>
//auto Euclidean<RT>::operator()(const R & a, const R & b) const {
//    if (a.size() != b.size()) {
//        DistanceType dnan = std::nan("Input container sizes do not match");
//        return dnan;
//    }
//    DistanceType sum = 0;
//    for (size_t i = 0; i < a.size(); ++i) { // TODO try Eigen specific norm function instead
//        sum += (a[i] - b[i]) * (a[i] - b[i]);
//    }
//    return std::sqrt(sum);
//}

template <typename RT>
template <typename R>
auto Euclidean_thresholded<RT>::operator()(const R& a, const R& b) const -> DistanceType {
    const auto distance = Euclidean<RT>::operator()(a, b);
    return std::min(thres, factor * distance);
}

template <typename RT>
template <typename R>
auto Manhattan<RT>::operator()(const R& a, const R& b) const -> DistanceType
{
    DistanceType sum = 0;
    for (const auto [a1, b1] : zip(a, b)) {
        sum += std::abs(a1 - b1);
    }
    return sum;
}

template <typename RT>
template <typename R>
auto P_norm<RT>::operator()(const R& a, const R& b) const -> DistanceType
{
    DistanceType sum = 0;
    for (auto [a1, b1] : zip(a, b)) {
        sum += std::pow(std::abs(a1 - b1), p);
    }
    return std::pow(sum, 1 / p);
}

template <typename RT>
template <typename R>
auto Cosine<RT>::operator()(const R& a, const R& b) const -> DistanceType
{
    ValueType dot = 0, denom_a = 0, denom_b = 0;
    for (auto [a1, b1] : zip(a, b)) {
        dot += a1 * b1;
        denom_a += a1 * b1;
        denom_b += a1 * b1;
    }
    return dot / (std::sqrt(denom_a) * std::sqrt(denom_b));
}

template <typename RT>
template <typename R>
auto CosineInverted<RT>::operator()(const R& a, const R& b) const -> DistanceType
{
    const auto distance = Cosine<RT>::operator()(a, b);
    return std::abs(1 - distance);
}

template <typename RT>
template <typename R>
auto Chebyshev<RT>::operator()(const R& a, const R& b) const -> DistanceType
{
    DistanceType res = 0;
    for (auto [a1, b1] : zip(a, b)) {
        res = std::max(res, std::abs(a1 - b1));
    }
    return res;
}

}  // namespace metric
