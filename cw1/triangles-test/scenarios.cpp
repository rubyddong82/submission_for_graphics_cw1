#include <catch2/catch_amalgamated.hpp>

#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"
#include "helpers.hpp"
#include <algorithm>
#include <cstdint>

// remember the color of draw_triangle_interp is in linear space (using ColorF)

#define COLOR {1.f, 0.f, 0.f} 
#define SURF 320, 240

void reset(Surface& surf) {
    surf.clear();
}

static void same_buffer (Surface& S0, Surface& S1) {

    const std::uint8_t* p_0 = S0.get_surface_ptr();
    const std::uint8_t* p_1 = S1.get_surface_ptr();
    const int bytes = (int)S0.get_width() * (int)(S0.get_height()) * 4;
    REQUIRE(std::equal(p_0, p_0 + bytes, p_1));
    reset(S0);
    reset(S1);
}


// Draw the same triangle CW vs CCW and compare buffers.

TEST_CASE("Compare CW & CCW", "[scenario1]") {
    
    Surface A( SURF ), B( SURF );

    Vec2f P0{ 60.f, 200.f }, p1{ 280.f, 40.f }, p2{ 300.f, 220.f };

    //cw
    draw_triangle_interp( A, P0, p1, p2, COLOR, COLOR, COLOR );
    //ccw
    draw_triangle_interp( B, P0, p2, p1, COLOR, COLOR, COLOR );
    // compare buffers
    same_buffer(A, B);

}

// one vector inside 2 vectors outside & 2 vectors inside 1 vector outside

TEST_CASE("Insides and outsides", "[scenario2]") {
    Surface surf( SURF );

     // 1 vector inside, 2 outside
    SECTION("one inside, two outside") {
        Vec2f A{ 160.f, 120.f }, B{ -100.f, 200.f }, C{ 400.f, 300.f };
        draw_triangle_interp( surf, A, B, C, COLOR, COLOR, COLOR );
        ColorU8_sRGB max = find_most_red_pixel(surf);
        ColorU8_sRGB min = find_least_red_nonzero_pixel(surf);
        REQUIRE(max.r == 255); REQUIRE(max.g == 0); REQUIRE(max.b == 0);
        REQUIRE(min.r == 255); REQUIRE(min.g == 0); REQUIRE(min.b == 0);
        reset(surf);
    }


    // 2 vectors inside, 1 outside
    SECTION("two inside, one outside") {
        Vec2f A{ 80.f, 60.f }, B{ 240.f, 180.f }, C{ 400.f, 300.f };
        draw_triangle_interp( surf, A, B, C, COLOR, COLOR, COLOR );
        ColorU8_sRGB max = find_most_red_pixel(surf);
        ColorU8_sRGB min = find_least_red_nonzero_pixel(surf);
        REQUIRE(max.r == 255); REQUIRE(max.g == 0); REQUIRE(max.b == 0);
        REQUIRE(min.r == 255); REQUIRE(min.g == 0); REQUIRE(min.b == 0);
        reset(surf);

    }
}

