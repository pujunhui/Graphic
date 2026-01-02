#include "global/base.h"
#include "math/math.h"

using namespace math;

int main() {
    vec2f v2_1(2.0f, 3.0f);
    vec2f v2_2 = v2_1;

    std::cout << "v1 is: " << v2_1 << std::endl;
    std::cout << "v2 is: " << v2_2 << std::endl;

    auto v2_add = v2_1 + v2_2;
    std::cout << "v1 + v2 = " << v2_add << std::endl;

    auto v2_multi = v2_1 * v2_2;
    std::cout << "v1 * v2 = " << v2_multi << std::endl;

    auto v2_dot = dot(v2_1, v2_2);
    std::cout << "v1 dot v2: " << v2_dot << std::endl;

    auto v2_cross = cross(v2_1, v2_2);
    std::cout << "v1 cross v2: " << v2_cross << std::endl;

    auto ss = 2.0 * v2_2;
    std::cout << "2 * v2 = " << ss << std::endl;

    auto v2_div = v2_2 / 2;
    std::cout << "v2 / 2 = " << v2_div << std::endl;

    vec4f v4(5.0f, 1.0f, 3.0f, 1.0f);
    auto v4n = normalize(v4);
    std::cout << "v4 normalize is:" << v4n << std::endl;

    math::mat4f src;
    src.set(
        4.0f, 3.0f, 2.0f, 1.0f,
        3.0f, 2.0f, 1.0f, 0.0f,
        2.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 2.0f
    );

    math::mat4f dst = inverse(src);
    std::cout << "matrix inverse result:" << dst << std::endl;
}
