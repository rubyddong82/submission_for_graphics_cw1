#include "draw-ex.hpp"

#include <algorithm>

#include <cstring> // for std::memcpy()

#include "draw.hpp"
#include "image.hpp"
#include "surface-ex.hpp"

void draw_ex_line_solid( SurfaceEx& aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor )
{
	using namespace std;

	if (!clip_line(aSurface.clip_area(), aBegin, aEnd))
		return;

	int x0 = static_cast<int>(lround(aBegin.x));
	int y0 = static_cast<int>(lround(aBegin.y));
	int x1 = static_cast<int>(lround(aEnd.x));
	int y1 = static_cast<int>(lround(aEnd.y));

	const int W = static_cast<int>(aSurface.get_width());
	const int H = static_cast<int>(aSurface.get_height());

	auto in_bounds = [W, H](int x, int y) {
		return (x >= 0 && x < W && y >= 0 && y < H);
	};

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	// implementation of Bresenham's line algorithm

	if (in_bounds(x0, y0)) {
	 aSurface.set_pixel_srgb(static_cast<Surface::Index>(x0), static_cast<Surface::Index>(y0), aColor);
	 if ( dx >= dy ) {
			int err = dx / 2;
			while (x0 != x1) {
				x0 += sx;
				err -= dy;
				if (err < 0) {
					y0 += sy;
					err += dx;
				}
				if (in_bounds(x0, y0))
					aSurface.set_pixel_srgb(static_cast<Surface::Index>(x0), static_cast<Surface::Index>(y0), aColor);
			}		
	 } else	{
			int err = dy / 2;
			while (y0 != y1) {
				y0 += sy;
				err -= dx;
				if (err < 0) {
					x0 += sx;
					err += dy;
				}
				if (in_bounds(x0, y0))
					aSurface.set_pixel_srgb(static_cast<Surface::Index>(x0), static_cast<Surface::Index>(y0), aColor);
			}
		}
	}
}

void blit_ex_solid( SurfaceEx& aSurface, ImageRGBA const& aImage, Vec2f aPosition )
{

	using namespace	std;

	int surfWidth = static_cast<int>(aSurface.get_width());
	int surfHeight = static_cast<int>(aSurface.get_height());
	int imgWidth = static_cast<int>(aImage.get_width());
	int imgHeight = static_cast<int>(aImage.get_height());

	// derive the top-left corner of where to start blitting
	const int startX = static_cast<int>(std::floor(aPosition.x - imgWidth	/ 2.0f));
	const int startY = static_cast<int>(std::floor(aPosition.y - imgHeight / 2.0f));

	// calculate the intersection of 2 rectangles
	const int x0 = max(0, startX);
	const int y0 = max(0, startY);
	const int x1 = min(surfWidth, startX + imgWidth);
	const int y1 = min(surfHeight, startY + imgHeight);
	if (x0 >= x1 || y0 >= y1) {
		return; // nothing to blit
	}

	const int image_x0 = x0 - startX;
	const int image_y0 = y0 - startY;

	for (int y = y0, image_y = image_y0; y < y1; ++y, ++image_y) {

		for (int x = x0, image_x = image_x0; x < x1; ++x, ++image_x) {

			ColorU8_sRGB_Alpha color_of_pixel = aImage.get_pixel((ImageRGBA::Index)image_x, (ImageRGBA::Index)image_y);
			aSurface.set_pixel_srgb((Surface::Index)x, (Surface::Index)y,
			ColorU8_sRGB{ color_of_pixel.r, color_of_pixel.g, color_of_pixel.b });
		}
	}
}

void blit_ex_memcpy( SurfaceEx& aSurface, ImageRGBA const& aImage, Vec2f aPosition )
{
	int surfWidth = static_cast<int>(aSurface.get_width());
	int surfHeight = static_cast<int>(aSurface.get_height());
	int imgWidth = static_cast<int>(aImage.get_width());
	int imgHeight = static_cast<int>(aImage.get_height());

	// derive the top-left corner of where to start blitting
	const int startX = static_cast<int>(std::floor(aPosition.x - imgWidth	/ 2.0f));
	const int startY = static_cast<int>(std::floor(aPosition.y - imgHeight / 2.0f));

	// calculate the intersection of 2 rectangles
	const int x0 = std::max(0, startX);
	const int y0 = std::max(0, startY);
	const int x1 = std::min(surfWidth, startX + imgWidth);
	const int y1 = std::min(surfHeight, startY + imgHeight);
	if (x0 >= x1 || y0 >= y1) {
		return; // nothing to blit
	}

	const int image_x0 = x0 - startX;
	const int image_y0 = y0 - startY;

	std::uint8_t* surfBase = aSurface.get_surface_ptr();
	const std::uint8_t* imgBase = aImage.get_image_ptr();

	// both surface and image are stored in row-major order
	// and each pixel is 4 bytes (RGBA)
	const int surfRowBytes = surfWidth	* 4;
	const int imgRowBytes = imgWidth * 4;
	// the width of the intersection of 2 rectangles in bytes
	const int copyBytes = (x1 - x0) * 4; 
	
 std::uint8_t* surfRowPtr = nullptr;
	const std::uint8_t* imgRowPtr = nullptr;

	for(int y = y0, image_y = image_y0; y < y1; ++y, ++image_y) {

		imgRowPtr = imgBase + (image_y * imgRowBytes) + (image_x0 * 4);
		surfRowPtr = surfBase + (y * surfRowBytes) + (x0 * 4);

		std::memcpy( surfRowPtr, imgRowPtr, copyBytes );
	}

}

void draw_ex_diagonal( SurfaceEx& aSurface, Vec2f aBegin, float aSteps, ColorU8_sRGB aColor )
{
	std::size_t const steps = std::size_t(aSteps);
	std::size_t const stride = 4*aSurface.get_width();

	std::uint8_t* sptr = aSurface.get_surface_ptr();
	// navigating pointer location from the beginning of the surface
	sptr += std::size_t(aBegin.y) * stride + 4*std::size_t(aBegin.x);

	for( std::size_t i = 0; i < steps; ++i )
	{
		sptr[0] = aColor.r;
		sptr[1] = aColor.g;
		sptr[2] = aColor.b;
		sptr[3] = 0;
		// making complete diagonal step
		sptr += stride + 4;
	}
}
