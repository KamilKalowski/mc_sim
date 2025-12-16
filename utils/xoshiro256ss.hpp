#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <bit>

class xoshiro256ss {
public:
    using result_type = std::uint64_t;

private:
    std::array<result_type, 4> s{};

    static constexpr result_type rotl(result_type x, int k) noexcept {
#if __cpp_lib_bitops >= 201907L
        return std::rotl(x, k);
#else
        return (x << k) | (x >> (64 - k));
#endif
    }

    static constexpr result_type splitmix64(result_type x) noexcept {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        x ^= (x >> 31);
        return x;
    }

public:
    xoshiro256ss() {
        std::random_device rd;
        seed(rd());
    }

    explicit xoshiro256ss(result_type seed_value) {
        seed(seed_value);
    }

    void seed(result_type seed_value) noexcept {
        for (auto &v: s) {
            v = splitmix64(seed_value);
            seed_value = v;
        }
    }

    static constexpr result_type min() noexcept { return 0; }
    static constexpr result_type max() noexcept { 
        return std::numeric_limits<result_type>::max();
    }

    result_type operator()() noexcept {
        const result_type result = rotl(s[1] * 5, 7) * 9;
        const result_type t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;
        s[3] = rotl(s[3], 45);

        return result;
    }
}