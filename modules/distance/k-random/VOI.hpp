/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
Copyright (c) 2019 Panda Team
*/

#ifndef _METRIC_DISTANCE_K_RANDOM_VOI_HPP
#define _METRIC_DISTANCE_K_RANDOM_VOI_HPP

#include "../../../modules/distance/k-related/Standards.hpp"
#include "../../../modules/correlation/entropy.hpp"
#include "../../../modules/utils/type_traits.hpp"

#include <type_traits>
#include <vector>

namespace metric {

/**
 * @brief
 *
 * @tparam RT
 */
template <typename RT>
class VOI {
public:
    /**
     * @brief Construct a new VOI object
     *
     * @param k
     * @param p
     */
    explicit VOI(int k = 3, int p = 25)
        : k(k)
        , p(p)
    {
    }

    /**
     * @brief calculate variation of information between two containers
     *
     * @param a first container
     * @param b second container
     * @return variation of information between a and b
     */

    template <typename Container, typename T = type_traits::underlying_type_t<Container>>
    typename std::enable_if_t<!std::is_integral_v<T>, V>  // only real values are accepted
    operator()(const Container& a, const Container& b) const;

    template <typename Container, typename T = type_traits::underlying_type_t<Container>>
    typename std::enable_if_t<std::is_integral_v<T>, V>  // only integer values are accepted
    operator()(const Container& a, const Container& b) const;
    // TODO add support of 1D random values passed in simple containers

protected:
    int k = 3;
    int p = 25;
};

// deduction guide for VOI
template <typename V>
VOI(int, V) -> VOI<double>;

/**
 * @class VOI_normalized
 *
 * @brief Normalized Variation of Information
 *
 */
template <typename RT>
class VOI_normalized : public VOI<V> {
public:
    using VOI<V>::VOI;

    /**
     * @brief Calculate Variation of Information
     *
     * @param a first container
     * @param b second container
     * @return variation of information between a and b
     */
    template <typename Container, typename T = type_traits::underlying_type_t<Container>>
              typename std::enable_if_t<!std::is_integral_v<T>, V>  // only real values are accepted
    operator()(const Container& a, const Container& b) const;

    // TODO add support of 1D random values passed in simple containers
};

template <typename V>
VOI_normalized(int, V) -> VOI_normalized<double>;


}  // namespace metric

#include "VOI.cpp"
#endif