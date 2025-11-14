#ifndef DRAW_HPP_BA97BA20_4B0E_45D8_97D4_65267FFA2EA6
#define DRAW_HPP_BA97BA20_4B0E_45D8_97D4_65267FFA2EA6

// IMPORTANT: DO NOT CHANGE THIS FILE WHEN YOU ARE SOLVING COURSEWORK 1!
//
// For CW1, the draw.hpp file must remain exactly as it is. In particular, you
// must not change any of the function prototypes in this header.

#include "forward.hpp"

#include "rect.hpp"
#include "color.hpp"

#include "../vmlib/vec2.hpp"

typedef int OutCode;
const int INSIDE = 0b0000;
const int LEFT   = 0b0001;
const int RIGHT  = 0b0010;
const int BOTTOM = 0b0100;
const int TOP    = 0b1000;

struct barycentric_coords {
	float alpha;
	float beta;
	float gamma;
};

struct bound_box_for_triangle {
	float xmin;
	float xmax;
	float ymin;
	float ymax;
};

barycentric_coords barycentric_from_cartesian(
	Vec2f const& aP,
	Vec2f const& aA,
	Vec2f const& aB,
	Vec2f const& aC
);

inline float cross_product (const Vec2f& a, const Vec2f& b) {
	return a.x * b.y - a.y * b.x;
}

inline bool inside_triangle (barycentric_coords const& aBary) {
	return (aBary.alpha >= 0.0f) && (aBary.beta >= 0.0f) && (aBary.gamma >= 0.0f);
}

ColorU8_sRGB interpolation (barycentric_coords const& aBary, ColorF const& aC0, ColorF const& aC1, ColorF const& aC2);

bound_box_for_triangle bounding_box_for_triangle (Vec2f const& aP0, Vec2f const& aP1, Vec2f const& aP2);

bound_box_for_triangle	clamping_box (bound_box_for_triangle& aBox, Surface const& aSurface);

// Coursework 1:
// clip_line() returns true if the line is visible after clipping.
bool clip_line( Rect2F const&, Vec2f& aBegin, Vec2f& aEnd );

void draw_clip_line_solid( 
	Surface&,
	Vec2f aBegin, Vec2f aEnd,
	ColorU8_sRGB
);

void draw_line_solid(
	Surface&,
	Vec2f aBegin, Vec2f aEnd,
	ColorU8_sRGB
);
void draw_line_solid(
	Surface&,
	Rect2F const&,
	Vec2f aBegin, Vec2f aEnd,
	ColorU8_sRGB
);


void draw_triangle_interp(
	Surface&,
	Vec2f aP0, Vec2f aP1, Vec2f aP2,
	ColorF aC0, ColorF aC1, ColorF aC2
);


// Utilities:
void draw_triangle_wireframe(
	Surface&,
	Vec2f aP0, Vec2f aP1, Vec2f aP2,
	ColorU8_sRGB
);

void draw_triangle_solid(
	Surface&,
	Vec2f aP0, Vec2f aP1, Vec2f aP2,
	ColorU8_sRGB
);

// From Exercise G.1
// You can ignore these in Coursework 1
void draw_rectangle_solid(
	Surface&,
	Vec2f aMinCorner, Vec2f aMaxCorner,
	ColorU8_sRGB
);

void draw_rectangle_outline(
	Surface&,
	Vec2f aMinCorner, Vec2f aMaxCorner,
	ColorU8_sRGB
);

#endif // DRAW_HPP_BA97BA20_4B0E_45D8_97D4_65267FFA2EA6
