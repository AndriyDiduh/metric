/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2020 Michael Welsch
*/

#ifndef _METRIC_DISTANCE_K_RELATED_STANDARDS_HPP
#define _METRIC_DISTANCE_K_RELATED_STANDARDS_HPP

#include "../../utils.hpp"
#include "../../utils/type_traits.hpp"

namespace metric {

template <typename RT>
class BaseMetric {
public:
    using RecordType = RT;
    using ValueType = contained_value_t<RecordType>;
};


template <typename RT>
class Euclidean : public BaseMetric<RT> {
public:
    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;

// Blaze vectors and sparse matrices
//    template <typename R>
//    auto operator()(const R & a, const R & b) const -> DistanceType;
//
//     Eigen, [] to access elements (or we can use Eigen-specific matrix operations)
//    template <typename R>
//    auto operator()(const R & a, const R & b) const -> DistanceType;

};


template <typename RT>
class Euclidean_thresholded : public Euclidean<RT> {
public:
    Euclidean_thresholded(DistanceType thres, double factor)
        : thres(thres)
        , factor(factor)
    {
    }

    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;

private:
    DistanceType thres = 1000.0;
    double factor = 3000.0;
};


/**
 * @class Manhattan
 *
 * @brief Manhattan/Cityblock (L1) Metric
 *
 */
template <typename RT>
class Manhattan: public BaseMetric<RT> {
public:
    using typename BaseMetric<RT>::ValueType;
    static_assert(std::is_floating_point<ValueType>::value, "T must be a float type");
    /**
     * @brief Calculate Manhattan distance in R^n
     *
     * @param a first vector
     * @param b second vector
     * @return Manhattan distance between a and b
     */
    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;
};

/**
 * @class P_norm
 *
 * @brief Minkowski (L general) Metric
 *
 */
template <typename RT>
class P_norm: public BaseMetric<RT> {
public:
    using typename BaseMetric<RT>::ValueType;
    static_assert(std::is_floating_point<ValueType>::value, "T must be a float type");
    /**
     * @brief Construct a new P_norm object
     *
     * @param p_
     */
    P_norm(const ValueType& p)
        : p(p)
    {
    }

    /**
     * @brief calculate Minkowski distance
     *
     * @param a first vector
     * @param b second vector
     * @return Minkowski distance between a and b
     */
    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;

private:
    ValueType p = 1;
};

/**
 * @class Cosine
 *
 * @brief Cosine similarity
 *
 */
template <typename RT>
class Cosine: public BaseMetric<RT> {
public:
    using typename BaseMetric<RT>::ValueType;
    static_assert(std::is_floating_point<ValueType>::value, "T must be a float type");
    /**
     * @brief calculate cosine similarity between two non-zero vector
     *
     * @param a first vector
     * @param b second vector
     * @return cosine similarity between a and b
     */
    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;
};

/**
 * @class CosineInverted
 *
 * @brief CosineInverted similarity. Means 1 - Cosine.
 *
 */
template <typename RT>
class CosineInverted: public Cosine<RT> {
public:
    using typename BaseMetric<RT>::ValueType;
    static_assert(std::is_floating_point<ValueType>::value, "T must be a float type");
    /**
     * @brief calculate cosine similarity between two non-zero vector
     *
     * @param a first vector
     * @param b second vector
     * @return cosine similarity between a and b
     */
    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;
};

/**
 * @class Chebyshev
 *
 * @brief P. Chebyshev's metric
 *
 */
template <typename RT>
class Chebyshev: public BaseMetric<RT> {
public:
    using typename BaseMetric<RT>::ValueType;
    static_assert(std::is_floating_point<ValueType>::value, "T must be a float type");
    /**
     * @brief calculate chebyshev metric
     *
     * @param lhs first container
     * @param rhs second container
     * @return Chebyshev distance between lhs and rhs
     */
    template <typename R>
    auto operator()(const R& a, const R& b) const -> DistanceType;
};

}  // namespace metric

#include "standards.cpp"

#endif  // Header Guard
