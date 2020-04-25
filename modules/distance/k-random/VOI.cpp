/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
Copyright (c) 2019 Panda Team
*/
#include "VOI.hpp"

#include "../../space/tree.hpp"
#include "../../utils/"

#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

namespace metric {

/**
 * @brief
 *
 * @param Xc
 * @param Yc
 * @param k
 * @param metric
 * @param version
 * @return
 */
template <typename Container, typename Metric>
auto mutualInformation(const Container & Xc, const Container & Yc, int k, const Metric & metric, int version)
    -> typename std::enable_if_t<!type_traits::is_container_of_integrals_v<Container>,
        type_traits::underlying_type_t<Container>>
{
    const auto N = Xc.size();

    if (N < k + 1 || Yc.size() < k + 1)
        throw std::invalid_argument("number of points in dataset must be larger than k");

    auto X = metric::to_vector(Xc);
    auto Y = metric::to_vector(Yc);
    auto XY = combine(X, Y);
    auto tree = metric::Tree(XY, -1, metric);
    auto entropyEstimate = entropy_details::digamma(k) + entropy_details::digamma(N);
    if (version == 2) {
        entropyEstimate -= 1 / static_cast<double>(k);
    }

    auto xTree = metric::Tree(X, -1, metric);

    for (std::size_t i = 0; i < N; i++) {
        auto res = tree.knn(XY[i], k + 1);
        auto neighbor = res.back().first;
        const auto dist = res.back().second;
        std::size_t nx = 0;
        if (version == 1) {
            // this is instead of replacing < with <= in Tree
            // added by Max F in order to match Julia code logic
            // without updating Tree
            auto dist_eps = std::nextafter(dist, std::numeric_limits<decltype(dist)>::max());
            nx = xTree.rnn(X[i], dist_eps).size();  // we include points that lay on the sphere
        } else if (version == 2) {
            auto ex = metric(X[neighbor->ID], X[i]);
            // this it to include the most distant point into the
            // sphere // added by Max F in order to match Julia code
            // logic without updating Tree
            const auto ex_eps = std::nextafter(ex, std::numeric_limits<decltype(ex)>::max());
            auto rnn_set = xTree.rnn(X[i], ex_eps);
            nx = rnn_set.size();  // replaced ex by ex_eps by Max F
        } else {
            throw std::runtime_error("this version is not allowed");
        }
        entropyEstimate -= 1.0 / N * entropy_details::digamma(static_cast<double>(nx));
    }
    return entropyEstimate;
}

template <typename V>
template <typename C, typename El>
typename std::enable_if_t<!std::is_integral_v<El>, V>  // only real values are accepted
VOI<V>::operator()(const C& a, const C& b) const
{
    const auto mi = mutualInformation(a, b, k);
    const auto e = Entropy(Chebyshev<El>(), k, p);
    const auto result = e(a) + e(b) - 2 * mi;
    return std::max(result, 0);
}

template <typename V>
template <typename C, typename El>
typename std::enable_if_t<std::is_integral_v<El>, V>
VOI<V>::operator()(const C& a, const C& b) const
{
    const auto mi = mutualInformation(a, b, k);
    const auto e = Entropy(Chebyshev<El>(), k, p);
    return e(a) + e(b) - 2 * mi;
}

template <typename V>
template <typename C, typename El>
typename std::enable_if_t<!std::is_integral_v<El>, V>  // only real values are accepted
VOI_normalized<V>::operator()(const C& a, const C& b) const
{
    const auto mi = mutualInformation(a, b, this->k);
    const auto e = Entropy(Chebyshev<El>(), this->k, this->p);
    return 1 - (mi / (e(a) + e(b) - mi));
}


}  // namespace metric