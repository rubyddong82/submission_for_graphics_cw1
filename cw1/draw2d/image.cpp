#include "image.hpp"

#include <memory>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cassert>

#include <stb_image.h>

#include "surface.hpp"

#include "../support/error.hpp"

namespace
{
	struct STBImageRGBA_ : public ImageRGBA
	{
		STBImageRGBA_( Index, Index, std::uint8_t* ); // this object contains data of width and height and pointer to pixel data
		virtual ~STBImageRGBA_();
	};
}

ImageRGBA::ImageRGBA()
	: mWidth( 0 )
	, mHeight( 0 )
	, mData( nullptr )
{}

ImageRGBA::~ImageRGBA() = default;


std::unique_ptr<ImageRGBA> load_image( char const* aPath )
{
	assert( aPath );

	stbi_set_flip_vertically_on_load( true );

	int w, h, channels;
	stbi_uc* ptr = stbi_load( aPath, &w, &h, &channels, 4 );
	if( !ptr )
		throw Error( "Unable to load image \"{}\"", aPath );

	return std::make_unique<STBImageRGBA_>(
		ImageRGBA::Index(w),
		ImageRGBA::Index(h),
		ptr
	);
}

void blit_masked( Surface& aSurface, ImageRGBA const& aImage, Vec2f aPosition )
{

	int surfWidth = static_cast<int>(aSurface.get_width());
	int surfHeight = static_cast<int>(aSurface.get_height());
	int imgWidth = static_cast<int>(aImage.get_width());
	int imgHeight = static_cast<int>(aImage.get_height());

	// derive the bottom left corner of where to start blitting
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

	for (int y = y0, image_y = image_y0; y < y1; ++y, ++image_y) {

		for (int x = x0, image_x = image_x0; x < x1; ++x, ++image_x) {

			ColorU8_sRGB_Alpha color_of_pixel = aImage.get_pixel((ImageRGBA::Index)image_x, (ImageRGBA::Index)image_y);
			if (color_of_pixel.a >= 128) { 
				aSurface.set_pixel_srgb((Surface::Index)x, (Surface::Index)y,
					ColorU8_sRGB{ color_of_pixel.r, color_of_pixel.g, color_of_pixel.b });
			}
		}
	}
}

namespace
{
	STBImageRGBA_::STBImageRGBA_( Index aWidth, Index aHeight, std::uint8_t* aPtr )
	{
		mWidth = aWidth;
		mHeight = aHeight;
		mData = aPtr;
	}

	STBImageRGBA_::~STBImageRGBA_()
	{
		if( mData )
			stbi_image_free( mData );
	}
}
