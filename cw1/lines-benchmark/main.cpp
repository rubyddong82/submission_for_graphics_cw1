#include <benchmark/benchmark.h>

#include "../draw2d/draw.hpp"
#include "../draw2d/draw-ex.hpp"
#include "../draw2d/surface-ex.hpp"

#define COLOR {255,255,255}
#define SURF 1920, 1080

namespace // this allows encapsulation of helper functions
{
	// This is a placeholder. Replace this with yor own code. Refer to
	// blit-benchmark/main.cpp for a more complete example. 

	void benchmark_line_length(benchmark::State& aState) {

	Surface surf(SURF);
	surf.clear();

	Vec2f p0{ 0.f, 0.f };
	Vec2f p1;

	p1.x = static_cast<float>(aState.range(0));
	p1.y = static_cast<float>(aState.range(1));



	for (auto _ : aState)
	{
		draw_line_solid(surf, p0, p1, COLOR);

		benchmark::ClobberMemory();
	}
	}

	void benchmark_slope(benchmark::State& aState) {

		int slopeType = aState.range(0); 
		Surface surf(SURF);
		surf.clear();

		Vec2f p0{ 0.f, 0.f };
		Vec2f p1;

		if (slopeType == 0) { // shallow slope
			p1 = { 1000.f , 10.f };
		}
		else if (slopeType == 1) { // steep slope
			p1 = { 10.f, 1000.f};
		}
		else { // 45 degree slope
			p1 = { 500.f, 500.f };
		}

		for (auto	_ : aState)
		{
			draw_line_solid(surf, p0, p1, COLOR);

			benchmark::ClobberMemory();
		}

	}

	void benchmark_clipping(benchmark::State& aState) {

		int clipType = aState.range(0); 
		Surface surf(SURF);
		surf.clear();

		Vec2f p0, p1;

		// the clipping scenarios should have the line segements with the same	length

		if (clipType == 0) { // completely outside
			p0 = { -200.f, -200.f };
			p1 = { -10.f, -10.f };
		}
		else if (clipType == 1) { // partially inside
			p0 = { -100.f, 50.f };
			p1 = { 300.f, 150.f };
		}
		else { // completely inside
			p0 = { 100.f, 100.f };	
			p1 = { 500.f, 400.f };
		}

		for (auto	_ : aState)
		{
			draw_line_solid(surf, p0, p1, COLOR);

			benchmark::ClobberMemory();
		}
	}


	void benchmark_draw_line_solid_original(	benchmark::State& aState ) {

			Surface surf(SURF);
			surf.clear();

			Vec2f p0 = {	0.f, 0.f };
			Vec2f	p1;
			p1.x = static_cast<float>(aState.range(0));
			p1.y = static_cast<float>(aState.range(1));


			for ( auto _ : aState )
			{
				draw_line_solid( surf, p0, p1, COLOR ); 

				benchmark::ClobberMemory(); 
			}
	}

	void benchmark_draw_ex_line_solid(	benchmark::State& aState ) {

			SurfaceEx surf(SURF);
			surf.clear();

			Vec2f p0 = {	0.f, 0.f };
			Vec2f	p1;
			p1.x = aState.range(0);
			p1.y = aState.range(1);

			for ( auto _ : aState )
			{
				draw_ex_line_solid( surf, p0, p1, COLOR ); 
				benchmark::ClobberMemory(); 
			}
	}

	void benchmark_draw_ex_diagonal(	benchmark::State& aState ) {
		
			SurfaceEx surf(SURF);
			surf.clear();

			Vec2f	p0 = {	0.f, 0.f };
			Vec2f	p1;
			p1.x = static_cast<float>(aState.range(0));
			p1.y = static_cast<float>(aState.range(1));

			for ( auto _ : aState )
			{
				draw_ex_line_solid( surf, p0, p1, COLOR ); 
				benchmark::ClobberMemory(); 
			}
	}

	//


// 	void placeholder_( benchmark::State& aState )
// 	{
// 		auto const width = std::uint32_t(aState.range(0));
// 		auto const height = std::uint32_t(aState.range(1));

// 		SurfaceEx surface( width, height );
// 		surface.clear();

// 		for( auto _ : aState )
// 		{
// 			// Placeholder that just does something:
// 			surface.clear(); // PLACEHOLDER! EXCLUDE FROM REAL BENCHMARKS!

// 			// ClobberMemory() ensures that the compiler won't optimize away
// 			// our blit operation. (Unlikely, but technically poossible.)
// 			benchmark::ClobberMemory(); 
// 		}
// 	}
// }

}



BENCHMARK( benchmark_line_length ) // convert with static_cast<float>
		->Args( { 50, 50} )
		->Args( { 750, 750 } )
		->Args( { 1000, 1000 } )
;

BENCHMARK( benchmark_slope )
		->Arg(0) // shallow slope
		->Arg(1) // steep slope
		->Arg(2) // 45 degree slope
;	

BENCHMARK( benchmark_clipping )
		->Arg(0) // completely outside
		->Arg(1) // partially inside
		->Arg(2) // completely inside
;

BENCHMARK( benchmark_draw_line_solid_original ) // convert with static_cast<float>
		->Args( { 500, 500 } )
		->Args( { 1000, 1000 } )
		->Args( { 1500, 1500 } )
;

BENCHMARK( benchmark_draw_ex_line_solid ) // convert with static_cast<float>
		->Args( { 500, 500 } )
		->Args( { 1000, 1000 } )
		->Args( { 1500, 1500 } )
;

BENCHMARK( benchmark_draw_ex_diagonal ) // convert with static_cast<float>
		->Args( { 500, 500 } )
		->Args( { 1000, 1000 } )
		->Args( { 1500, 1500 } )
;	


// // these are 2 resolutions

// BENCHMARK( placeholder_ )
// 	->Args( { 1920, 1080 } )
// 	->Args( { 7680, 4320 } )
// ;


BENCHMARK_MAIN();
