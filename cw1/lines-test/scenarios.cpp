// #include <catch2/catch_amalgamated.hpp>

// //TODO
#include <catch2/catch_amalgamated.hpp>
#include <algorithm>

#include "helpers.hpp"                  // max_row_pixel_count, max_col_pixel_count, count_pixel_neighbours
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

#define COLOR {255,255,255}
#define SURF 640, 480

// additional helper functions for the tests

void reset(Surface& surf) {
    surf.clear();
}

static void neighbourCounts(Surface const& surf) {
       std::array<std::size_t, 9> counts = count_pixel_neighbours(surf);
       int total = 0;
        for (int i = 0; i < 9; ++i) {
            total += counts[i];
        }
        if (total > 1) {
            REQUIRE(counts[0] == 0); // no isolated pixels
            REQUIRE(counts[1] == 2); // two endpoints
            for (int i = 3; i < 9; ++i) REQUIRE(counts[i] == 0);    // no pixels with >2 neighbours
        }
    }

static void max_pixel__neighbour_check (std::string_view string, Vec2f p0, Vec2f p1, Surface& surf) {

    draw_clip_line_solid(surf, p0, p1, COLOR);

    if (string == "horizontal") {
        REQUIRE(max_col_pixel_count(surf) == 1);
        REQUIRE(max_row_pixel_count(surf) > 1);                          
    } else if (string == "45 degree diagonal") {
        REQUIRE(max_row_pixel_count(surf) == 1);
        REQUIRE(max_col_pixel_count(surf) == 1);
    } else if (string == "completely horizontal") {
        REQUIRE(max_row_pixel_count(surf) == static_cast<std::size_t>(std::fabs(p0.x - p1.x)) );
        REQUIRE(max_col_pixel_count(surf) == 1);
    }
    neighbourCounts(surf);
    reset(surf);
}

static void same_buffer (Surface& S0, Surface& S1, Vec2f p0, Vec2f p1) {
    draw_line_solid(S0, p0, p1, COLOR);
    draw_line_solid(S1, p1, p0, COLOR);

    const std::uint8_t* p_0 = S0.get_surface_ptr();
    const std::uint8_t* p_1 = S1.get_surface_ptr();
    const int bytes = (int)S0.get_width() * (int)(S0.get_height()) * 4;
    REQUIRE(std::equal(p_0, p_0 + bytes, p_1));
    reset(S0);
    reset(S1);
}

static void draw_line_chain(Surface& surf, std::vector<Vec2f> const& points) {
    for (std::size_t i=0; i+1<points.size(); ++i) {
        draw_line_solid(surf, points[i], points[i+1], COLOR);
    }
}

//==========================================================================================================

// Scenario 1: one endpoint inside, one outside

TEST_CASE("Scenario1", "[scenario1][!mayfail]") {
    Surface surf(SURF);

    SECTION("horizontal") { // blue
        max_pixel__neighbour_check("horizontal", {320.f,240.f}, {2000.f,280.f}, surf);
    }
    SECTION("vertical") { // red
        max_pixel__neighbour_check("vertical", {320.f,240.f}, {340.f,800.f}, surf);
    }
    SECTION("45 degree diagonal") { // green
        max_pixel__neighbour_check("45 degree diagonal", {320.f,240.f}, {800.f,720.f}, surf);
    }
    SECTION("completely horizontal") { // black
        max_pixel__neighbour_check("completely horizontal", {50.f,120.f}, {-200.f,120.f}, surf);
    }
}

// Scenario 2: both endpoints outside

TEST_CASE("Scenario2", "[scenario2][!mayfail]") {
    Surface surf(SURF);

    SECTION("horizontal across") {
        max_pixel__neighbour_check("horizontal", {-100.f,240.f}, {740.f,280.f}, surf);
    }
    SECTION("vertical across") {
        max_pixel__neighbour_check("vertical", {320.f,-50.f}, {340.f,600.f}, surf);
    }
    SECTION("diagonal across") {
        max_pixel__neighbour_check("45 degree diagonal", {-50.f,-50.f}, {700.f,700.f}, surf);
    }
    SECTION("misses entirely") {
        draw_line_solid(surf, {-200.f,-200.f}, {-10.f,-10.f}, COLOR);
        REQUIRE(max_row_pixel_count(surf) == 0);
        REQUIRE(max_col_pixel_count(surf) == 0);
        reset(surf);
    }
}

// Scenario 3: p0->p1 must equal to p1->p0

TEST_CASE("Scenario3", "[scenario3]") {
    Surface A(SURF), B(SURF);

    SECTION("horizontal") {
    same_buffer(A, B, {50.f, 150.f}, {300.f, 160.f});
    }
    SECTION("vertical") {
       same_buffer(A, B, {150.f, 20.f}, {170.f, 300.f});
    }
    SECTION("45 degree diagonal") {
       same_buffer(A, B, {40.f, 10.f}, {120.f, 90.f});
    }
    SECTION("completely horizontal") {
        same_buffer(A, B, {10.f, 400.f}, {600.f, 400.f});
    }
}

// Scenario 4: connected lines should have no gaps at the junction

TEST_CASE("Scenario4", "[scenario4][!mayfail]") {

    Surface surf(SURF);
    std::vector<Vec2f> points;

    SECTION("two line segments meet exactly") {
        points.push_back({20.f, 200.f});
        points.push_back({200.f, 200.f});
        points.push_back({200.f, 40.f});
        draw_line_chain(surf, points);

        neighbourCounts(surf);
        reset(surf);
        points.clear();
    }

    SECTION("polyline with random short line segments") {

        std::vector<Vec2f> pts = {
            {30.f, 30.f}, {34.f, 33.f}, {38.f, 36.f}, {42.f, 39.f},
            {46.f, 42.f}, {50.f, 45.f}, {54.f, 48.f}, {58.f, 51.f}
        };
        draw_line_chain(surf, pts);
        neighbourCounts(surf);
        reset(surf);
    }

    SECTION("short line segments horizontal chain") {

        for (int x=20; x<=100; x+=4) {
            points.push_back({float(x), 110.f});
            points.push_back({float(x+3), 110.f});
        }
        draw_line_chain(surf, points);
        neighbourCounts(surf);
        reset(surf);
    }

    SECTION("short line segments vertical chain") {

        for (int y=20; y<=100; y+=3) {
            points.push_back({float(140), float(y)});
            points.push_back({float(140), float(y+2)});
        }
        draw_line_chain(surf, points);
        neighbourCounts(surf);
        reset(surf);
    }
}
