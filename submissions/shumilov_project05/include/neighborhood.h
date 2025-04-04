#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H


template<class T>
struct Neighborhood2D
{
    T north{};
    T south{};
    T east{};
    T west{};

    [[nodiscard]]
    constexpr Neighborhood2D() = default;

    [[nodiscard]]
    constexpr Neighborhood2D(const T& n, const T& s, const T& e, const T& w)
        : north{n}, south{s}, east{e}, west{w} {}

    [[nodiscard]]
    explicit constexpr Neighborhood2D(const int n) : Neighborhood2D {n, n, n, n}
    {}
};

using Padding = Neighborhood2D<int>;

#endif //NEIGHBORHOOD_H
