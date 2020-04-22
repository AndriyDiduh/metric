/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2018 Michael Welsch
*/

#include <cmath> // std::nan, std::sqrt
#include "../../../3rdparty/blaze/Blaze.h" // blaze::norm

#include "Standards.hpp"


namespace metric {



// Euclidean

template <typename RT>
template <typename R>
typename std::enable_if <
 std::is_same<R, RT>::value
  && determine_container_type<R>::code != 2  // non-Blaze type
  && determine_container_type<R>::code != 3, // non-Eigen type
 DistanceType
>::type
Euclidean<RT>::operator()(const R & a, const R & b) const {
    if (a.size() != b.size()) {
        DistanceType dnan = std::nan("Input container sizes do not match");
        return dnan;
    }
    DistanceType sum = 0;
    for (auto it1 = a.begin(), it2 = b.begin(); it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
        sum += (*it1 - *it2) * (*it1 - *it2);
    }
    return std::sqrt(sum);
}


template <typename RT>
template <typename R>
typename std::enable_if <
 std::is_same<R, RT>::value && determine_container_type<R>::code == 2, // Blaze vectors and sparse matrices
 DistanceType
>::type
Euclidean<RT>::operator()(const R & a, const R & b) const {
    //if (a.size() != b.size()) { // no .size method in martices, TODO restore this check!
        //DistanceType dnan = std::nan("Input container sizes do not match");
        //return dnan;
    //}
    return blaze::norm(a - b); // slower than the iterator-based approach above
}
// NOTE: we can tell apart Blaze DyncmicVector and CompressedMatrix via signature,
// ans processing CompressedMatrix via iterators will skip zero elements and result in wring output,
// so we use Blaze norm function for both Blaze types.
// Although, processing DynamicVector via iterators is correct and mush faster than via norm function. So
// TODO add DynamicVector type detector and process this type apart


template <typename RT>
template <typename R>
typename std::enable_if <
 std::is_same<R, RT>::value && determine_container_type<R>::code == 3, // Eigen, [] to access elements (or we can use Eigen-specific matrix operations)
 DistanceType
>::type
Euclidean<RT>::operator()(const R & a, const R & b) const {
    if (a.size() != b.size()) {
        DistanceType dnan = std::nan("Input container sizes do not match");
        return dnan;
    }
    DistanceType sum = 0;
    for (size_t i = 0; i < a.size(); ++i) { // TODO try Eigen specific norm function instead
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return std::sqrt(sum);
}




}  // namespace metric
