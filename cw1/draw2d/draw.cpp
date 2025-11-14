#include "draw.hpp"

#include <algorithm>

#include	<iostream> // for debugging output

#include <cmath>

#include "surface.hpp"


bool clip_line( Rect2F const& aTargetArea, Vec2f& aBegin, Vec2f& aEnd )
{
 // 
	const float xmin = aTargetArea.xmin;
	const float ymin = aTargetArea.ymin;
	const float xmax = aTargetArea.xmin + aTargetArea.width - 1.0f;
	const float ymax = aTargetArea.ymin + aTargetArea.height - 1.0f;

	auto outcode_of = [&](const Vec2f& v) { // lambda function so that I can use local variables
		OutCode	code = INSIDE;
		if (v.x < xmin) code |= LEFT;
		else if (v.x > xmax) code |= RIGHT;
		if (v.y < ymin) code |= BOTTOM;
		else if (v.y > ymax) code |= TOP;
		return code;
	};

	OutCode code0 = outcode_of(aBegin);
	OutCode code1 = outcode_of(aEnd);

	while (true) {
		if ((code0 | code1) == 0) { // both inside
			return true;
		}
		if (code0 & code1) { // doesn't intersect
			return false;
		}
		//choose the vector that is outside
		OutCode codeOut = code0 ? code0 : code1;
		float	x, y;
		// find the intersection point
		if (codeOut & TOP) {
			y = ymax;
			x = aBegin.x + (aEnd.x - aBegin.x) * (y	- aBegin.y) / (aEnd.y - aBegin.y);
		}
		else if (codeOut & BOTTOM) {
			y = ymin;
			x = aBegin.x + (aEnd.x - aBegin.x) * (y - aBegin.y) / (aEnd.y - aBegin.y);
		}
		else if (codeOut & RIGHT) {
			x = xmax;
			y = aBegin.y + (aEnd.y - aBegin.y) * (x - aBegin.x) / (aEnd.x - aBegin.x);
		}
		else { // LEFT
			x = xmin;
			y = aBegin.y + (aEnd.y - aBegin.y) * (x - aBegin.x) / (aEnd.x - aBegin.x);
		}

		if (codeOut == code0) {
			aBegin = {x,	y};
			code0 = outcode_of(aBegin);
		}
		else {
			aEnd = {x, y};
			code1 = outcode_of(aEnd);
		}
	}
}

void draw_clip_line_solid( Surface& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{

	using namespace std;
	// 1. check if the line is in the surface area using clip_line()
	if (!clip_line(aSurface.clip_area(), aBegin, aEnd))
		return;

	const int W = (int)aSurface.get_width();
	const int H = (int)aSurface.get_height();

	auto in_bounds = [&](int x, int y) {
		return (x >= 0 && x < W && y >= 0 && y < H);
	};
	// 2. check if 2 vectors are the same	(degenerate case)
	if (aBegin.x == aEnd.x && aBegin.y == aEnd.y) {
		int x = (int)lround(aBegin.x);
		int y = (int)lround(aBegin.y);
		if (in_bounds(x, y))
			aSurface.set_pixel_srgb((Surface::Index)x, (Surface::Index)y, aColor);
		return;
	}
	// 3. draw it by either chooseing x-major or y-major based on the lenth of dx and dy
	const float dx = aEnd.x - aBegin.x;
	const float dy = aEnd.y - aBegin.y;

	// 4. calculate the slope, starting point, number of steps
	const float slope = fabs(dx) >= fabs(dy) ? dy / dx : dx / dy;                 // dy per 1 in x             // dx per 1 in y
	// we get the start point vector
	Vec2f startPoint = fabs(dx) >= fabs(dy) ? aBegin.x == min(aBegin.x, aEnd.x) ? aBegin : aEnd
																																								: aBegin.y == min(aBegin.y, aEnd.y) ? aBegin : aEnd;
	// float a = fabs(dx) > fabs(dy) ? startPoint.y : startPoint.x;
	// float b = a == startPoint.x ? startPoint.y : startPoint.x;
	int numberOfSteps = fabs(dx) >= fabs(dy) ? abs((int)lround(aBegin.x - aEnd.x)) + 1 
																																: abs((int)lround(aBegin.y - aEnd.y)) + 1;
	// 5. make sure each pixels are adjacent
 // 6. loop through and set pixels using set_pixel_srgb()
	if (fabs(dx) >= fabs(dy)) {
		int x = (int)lround(startPoint.x);
		// float y = startPoint.y + (float(x) - startPoint.x) * slope;
		float y = startPoint.y;
		for (int i = 0; i < numberOfSteps; ++i) {
			if (in_bounds(x, (int)lround(y))) 
				aSurface.set_pixel_srgb((Surface::Index)(int)x, (Surface::Index)(int)lround(y), aColor);
			x += 1;
			y += slope;
		}
	} else {
		int y = (int)lround(startPoint.y);
		// float x = startPoint.x + (float(y) - startPoint.y) * (1.0f / slope);
		float x = startPoint.x;
		for (int i = 0; i < numberOfSteps; ++i) {
			if (in_bounds((int)lround(x), y)) 	
					aSurface.set_pixel_srgb((Surface::Index)(int)lround(x), (Surface::Index)y, aColor);
		x += slope; 
		y += 1;
	}
}
}

void draw_line_solid( Surface& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	if( clip_line( aSurface.clip_area(), aBegin, aEnd ) )
		draw_clip_line_solid( aSurface, aBegin, aEnd, aColor );
}
void draw_line_solid( Surface& aSurface, Rect2F const& aClipArea, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	if( clip_line( aClipArea, aBegin, aEnd ) )
		draw_clip_line_solid( aSurface, aBegin, aEnd, aColor );
}

barycentric_coords barycentric_from_cartesian ( Vec2f const& aP, Vec2f const& aA, Vec2f const& aB, Vec2f const& aC )
{
	float area_ABC = cross_product(aB - aA, aC - aA);
	float	area_PBC = cross_product(aB - aP, aC - aP);
	float	area_PCA = cross_product(aC - aP, aA - aP);
	return barycentric_coords{
		area_PBC / area_ABC,
		area_PCA / area_ABC,
		1.0f - (area_PBC + area_PCA) / area_ABC
	};
}

bound_box_for_triangle	bounding_box_for_triangle ( Vec2f const& aP0, Vec2f const& aP1, Vec2f const& aP2 )
{
	float xmin = std::min({ aP0.x, aP1.x, aP2.x });
	float xmax = std::max({ aP0.x, aP1.x, aP2.x });
	float ymin = std::min({ aP0.y, aP1.y, aP2.y });
	float ymax = std::max({ aP0.y, aP1.y, aP2.y });
	return bound_box_for_triangle{ xmin, xmax, ymin, ymax };
}

bound_box_for_triangle clamping_box (bound_box_for_triangle& aBox, Surface const& aSurface)
{

	Rect2F clip = aSurface.clip_area();

	const int clip_xmin = (int)std::floor(clip.xmin);
	const int clip_ymin = (int)std::floor(clip.ymin);
	const int clip_xmax = (int)std::floor(clip.xmin + clip.width)  - 1;
	const int clip_ymax = (int)std::floor(clip.ymin + clip.height) - 1;

	aBox.xmin = std::max(clip_xmin, (int)std::floor(aBox.xmin));
	aBox.xmax = std::min(clip_xmax, (int)std::ceil (aBox.xmax) - 1);
	aBox.ymin = std::max(clip_ymin, (int)std::floor(aBox.ymin));
	aBox.ymax = std::min(clip_ymax, (int)std::ceil (aBox.ymax) - 1);

	return aBox;
}

ColorU8_sRGB interpolation (	barycentric_coords const& aBary, ColorF const& aC0, ColorF const& aC1, ColorF const& aC2 )
{
	ColorF linearColor;
	linearColor.r = aBary.alpha * aC0.r + aBary.beta * aC1.r + aBary.gamma * aC2.r;
	linearColor.g = aBary.alpha * aC0.g + aBary.beta * aC1.g + aBary.gamma * aC2.g;
	linearColor.b = aBary.alpha * aC0.b + aBary.beta * aC1.b + aBary.gamma * aC2.b;

	return linear_to_srgb(linearColor);
}


void draw_triangle_interp( Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorF aC0, ColorF aC1, ColorF aC2 )
{
	// check if the triangle is degenerate (area = 0)
	float area = cross_product(aP1 - aP0, aP2 - aP0);
	if (area == 0.0f) {
		return; // degenerate triangle
	}
	// compute bounding box of the triangle
	bound_box_for_triangle box = bounding_box_for_triangle(aP0, aP1,	aP2);
	// clamp the bounding box to the surface clip area
	box = clamping_box(box, aSurface);

	if (box.xmax < box.xmin || box.ymax < box.ymin) {
		return; // no pixels to draw
	}
 // for each pixel, check if it's in the triangle using barycentric coordinates
	// if it is, calculate the interpolated color and set the pixel using the barycetric weights

	for (int x = box.xmin; x <= box.xmax; ++x) {	
		for (int y = box.ymin; y <= box.ymax; ++y) {
			Vec2f p = Vec2f{ static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f }; // center of the pixel
			barycentric_coords bary = barycentric_from_cartesian(p, aP0, aP1, aP2);
			if (inside_triangle(bary)) {
				ColorU8_sRGB color = interpolation(bary, aC0, aC1, aC2);
				aSurface.set_pixel_srgb((Surface::Index)x, (Surface::Index)y, color);
			}
		}
	}
}

// You are not required to implement the following, but they can be useful for
// debugging.
void draw_triangle_wireframe( Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aP0;   // is properly implemented.
	(void)aP1;
	(void)aP2;
	(void)aColor;
}

void draw_triangle_solid( Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aP0;   // is properly implemented.
	(void)aP1;
	(void)aP2;
	(void)aColor;
}

void draw_rectangle_solid( Surface& aSurface, Vec2f aMinCorner, Vec2f aMaxCorner, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aMinCorner;   // is properly implemented.
	(void)aMaxCorner;
	(void)aColor;
}

void draw_rectangle_outline( Surface& aSurface, Vec2f aMinCorner, Vec2f aMaxCorner, ColorU8_sRGB aColor )
{
	//TODO: your implementation goes here
	//TODO: your implementation goes here
	//TODO: your implementation goes here

	//TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments
	(void)aMinCorner;
	(void)aMaxCorner;
	(void)aColor;
}
