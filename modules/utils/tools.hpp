namespace metric {

template <typename C1, typename C2, typename T=type_traits::underlying_type_t<C1>>
std::vector<std::vector<T>> combine(const C1& X, const C2& Y)
{
    std::size_t N = X.size();
    std::size_t dx = X[0].size();
    std::size_t dy = Y[0].size();
    std::vector<std::vector<T>> XY(N);
    for (std::size_t i = 0; i < N; i++) {
        XY[i].resize(dx + dy);
        std::size_t k = 0;
        for (std::size_t j = 0; j < dx; j++, k++) {
            XY[i][k] = X[i][j];
        }
        for (std::size_t j = 0; j < dy; j++, k++) {
            XY[i][k] = Y[i][j];
        }
    }
    return XY;
}

// needed only for unique without Boost
template <typename ForwardIterator>
ForwardIterator remove_duplicates( ForwardIterator first, ForwardIterator last )
{
    auto new_last = first;

    for ( auto current = first; current != last; ++current )
    {
        if ( std::find( first, new_last, *current ) == new_last )
        {
            if ( new_last != current ) *new_last = *current;
            ++new_last;
        }
    }

    return new_last;
}

// unique without Boost, never called, TODO remove if not needed
template <typename Container, typename T = metric::type_traits::underlying_type_t<Container>>
std::vector<T> unique(const Container & in)
{
    auto out = in;
    out.erase( remove_duplicates( out.begin(), out.end() ), out.end() );
    return out;
}

template <typename RT, typename T = metric::type_traits::underlying_type_t<RT>>
std::vector<vector<T>> to_vector(const RT& a) {
    std::vector<std::vector<T>> result;
    for (const auto& e: Yc) {
        result.push_back(std::vector<T>(std::begin(e), std::end(e)));
    }
    return result;
}