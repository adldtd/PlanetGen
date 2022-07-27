//----------------------------------------------------------------------------------------
//
//	siv::PerlinNoise
//	Perlin noise library for modern C++
//
//	Copyright (C) 2013-2021 Ryo Suzuki <reputeless@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//----------------------------------------------------------------------------------------

# pragma once
# include <cstdint>
# include <algorithm>
# include <array>
# include <iterator>
# include <numeric>
# include <random>
# include <type_traits>

# if __has_include(<concepts>) && defined(__cpp_concepts)
#	include <concepts>
# endif


// Library major version
# define SIVPERLIN_VERSION_MAJOR			3

// Library minor version
# define SIVPERLIN_VERSION_MINOR			0

// Library revision version
# define SIVPERLIN_VERSION_REVISION			0

// Library version
# define SIVPERLIN_VERSION			((SIVPERLIN_VERSION_MAJOR * 100 * 100) + (SIVPERLIN_VERSION_MINOR * 100) + (SIVPERLIN_VERSION_REVISION))


// [[nodiscard]] for constructors
# if (201907L <= __has_cpp_attribute(nodiscard))
#	define SIVPERLIN_NODISCARD_CXX20 [[nodiscard]]
# else
#	define SIVPERLIN_NODISCARD_CXX20
# endif


// std::uniform_random_bit_generator concept
# if __cpp_lib_concepts
#	define SIVPERLIN_CONCEPT_URBG  template <std::uniform_random_bit_generator URBG>
#	define SIVPERLIN_CONCEPT_URBG_ template <std::uniform_random_bit_generator URBG>
# else
#	define SIVPERLIN_CONCEPT_URBG  template <class URBG, std::enable_if_t<std::conjunction_v<std::is_invocable<URBG&>, std::is_unsigned<std::invoke_result_t<URBG&>>>>* = nullptr>
#	define SIVPERLIN_CONCEPT_URBG_ template <class URBG, std::enable_if_t<std::conjunction_v<std::is_invocable<URBG&>, std::is_unsigned<std::invoke_result_t<URBG&>>>>*>
# endif


// arbitrary value for increasing entropy
# ifndef SIVPERLIN_DEFAULT_Y
#	define SIVPERLIN_DEFAULT_Y (0.12345)
# endif

// arbitrary value for increasing entropy
# ifndef SIVPERLIN_DEFAULT_Z
#	define SIVPERLIN_DEFAULT_Z (0.34567)
# endif


namespace siv
{
	template <class Float>
	class BasicPerlinNoise
	{
	public:

		static_assert(std::is_floating_point_v<Float>);

		///////////////////////////////////////
		//
		//	Typedefs
		//

		using state_type = std::array<std::uint8_t, 256>;

		using value_type = Float;

		using default_random_engine = std::mt19937;

		using seed_type = typename default_random_engine::result_type;

		///////////////////////////////////////
		//
		//	Constructors
		//

		SIVPERLIN_NODISCARD_CXX20
		constexpr BasicPerlinNoise() noexcept;

		SIVPERLIN_NODISCARD_CXX20
		explicit BasicPerlinNoise(seed_type seed);

		SIVPERLIN_CONCEPT_URBG
		SIVPERLIN_NODISCARD_CXX20
		explicit BasicPerlinNoise(URBG&& urbg);

		///////////////////////////////////////
		//
		//	Reseed
		//

		void reseed(seed_type seed);

		SIVPERLIN_CONCEPT_URBG
		void reseed(URBG&& urbg);

		///////////////////////////////////////
		//
		//	Serialization
		//

		[[nodiscard]]
		constexpr const state_type& serialize() const noexcept;

		constexpr void deserialize(const state_type& state) noexcept;

		///////////////////////////////////////
		//
		//	Noise (The result is in the range [-1, 1])
		//

		[[nodiscard]]
		value_type noise1D(value_type x) const noexcept;

		[[nodiscard]]
		value_type noise2D(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		value_type noise3D(value_type x, value_type y, value_type z) const noexcept;

		[[nodiscard]]
		value_type noise4D(value_type x, value_type y, value_type z, value_type w) const noexcept;

		///////////////////////////////////////
		//
		//	Noise (The result is remapped to the range [0, 1])
		//

		[[nodiscard]]
		value_type noise1D_01(value_type x) const noexcept;

		[[nodiscard]]
		value_type noise2D_01(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		value_type noise3D_01(value_type x, value_type y, value_type z) const noexcept;

		///////////////////////////////////////
		//
		//	Octave noise (The result can be out of the range [-1, 1])
		//

		[[nodiscard]]
		value_type octave1D(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave2D(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave3D(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave4D(value_type x, value_type y, value_type z, value_type w, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		///////////////////////////////////////
		//
		//	Octave noise (The result is clamped to the range [-1, 1])
		//

		[[nodiscard]]
		value_type octave1D_11(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave2D_11(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave3D_11(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		///////////////////////////////////////
		//
		//	Octave noise (The result is clamped and remapped to the range [0, 1])
		//

		[[nodiscard]]
		value_type octave1D_01(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave2D_01(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type octave3D_01(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		///////////////////////////////////////
		//
		//	Octave noise (The result is normalized to the range [-1, 1])
		//

		[[nodiscard]]
		value_type normalizedOctave1D(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type normalizedOctave2D(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type normalizedOctave3D(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		///////////////////////////////////////
		//
		//	Octave noise (The result is normalized and remapped to the range [0, 1])
		//

		[[nodiscard]]
		value_type normalizedOctave1D_01(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type normalizedOctave2D_01(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

		[[nodiscard]]
		value_type normalizedOctave3D_01(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

	private:

		state_type m_permutation;
	};

	using PerlinNoise = BasicPerlinNoise<double>;

	namespace perlin_detail
	{
		////////////////////////////////////////////////
		//
		//	These functions are provided for consistency.
		//	You may get different results from std::shuffle() with different standard library implementations.
		//
		SIVPERLIN_CONCEPT_URBG
		[[nodiscard]]
		inline std::uint64_t Random(const std::uint64_t max, URBG&& urbg)
		{
			return (urbg() % (max + 1));
		}

		template <class RandomIt, class URBG>
		inline void Shuffle(RandomIt first, RandomIt last, URBG&& urbg)
		{
			if (first == last)
			{
				return;
			}

			using difference_type = typename std::iterator_traits<RandomIt>::difference_type;

			for (RandomIt it = first + 1; it < last; ++it)
			{
				const std::uint64_t n = static_cast<std::uint64_t>(it - first);
				std::iter_swap(it, first + static_cast<difference_type>(Random(n, std::forward<URBG>(urbg))));
			}
		}
		//
		////////////////////////////////////////////////

		template <class Float>
		[[nodiscard]]
		inline constexpr Float Fade(const Float t) noexcept
		{
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		template <class Float>
		[[nodiscard]]
		inline constexpr Float Lerp(const Float a, const Float b, const Float t) noexcept
		{
			return (a + (b - a) * t);
		}

		template <class Float>
		[[nodiscard]]
		inline constexpr Float Grad(const std::uint8_t hash, const Float x, const Float y, const Float z) noexcept
		{
			const std::uint8_t h = hash & 15;
			const Float u = h < 8 ? x : y;
			const Float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

		//http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
		/*
		(1,1,1,0),(1,-1,1,0),(1,1,-1,0),(1,-1,-1,0),(-1,1,1,0),(-1,-1,1,0),(-1,1,-1,0),(-1,-1,-1,0),
		(1,1,0,1),(1,-1,0,1),(1,1,0,-1),(1,-1,0,-1),(-1,1,0,1),(-1,-1,0,1),(-1,1,0,-1),(-1,-1,0,-1),
		(1,0,1,1),(1,0,-1,1),(1,0,1,-1),(1,0,-1,-1),(-1,0,1,1),(-1,0,-1,1),(-1,0,1,-1),(-1,0,-1,-1),
		(0,1,1,1),(0,1,-1,1),(0,1,1,-1),(0,1,-1,-1),(0,-1,1,1),(0,-1,-1,1),(0,-1,1,-1),(0,-1,-1,-1)
		*/
		template <class Float> //Dot product using the former random vectors
		[[nodiscard]]
		inline constexpr Float Grad4D(const std::uint8_t hash, const Float x, const Float y, const Float z, const Float w) noexcept
		{
			switch (hash & 31)
			{
				case 0: return x + y + z;
				case 1: return x - y + z;
				case 2: return x + y - z;
				case 3: return x - y - z;
				case 4: return -x + y + z;
				case 5: return -x - y + z;
				case 6: return -x + y - z;
				case 7: return -x - y - z;

				case 8: return x + y + w;
				case 9: return x - y + w;
				case 10: return x + y - w;
				case 11: return x - y - w;
				case 12: return -x + y + w;
				case 13: return -x - y + w;
				case 14: return -x + y - w;
				case 15: return -x - y - w;

				case 16: return x + z + w;
				case 17: return x - z + w;
				case 18: return x + z - w;
				case 19: return x - z - w;
				case 20: return -x + z + w;
				case 21: return -x - z + w;
				case 22: return -x + z - w;
				case 23: return -x - z - w;

				case 24: return y + z + w;
				case 25: return y - z + w;
				case 26: return y + z - w;
				case 27: return y - z - w;
				case 28: return -y + z + w;
				case 29: return -y - z + w;
				case 30: return -y + z - w;
				case 31: return -y - z - w;

				default: return 0;
			}
		}

		template <class Float>
		[[nodiscard]]
		inline constexpr Float Remap_01(const Float x) noexcept
		{
			return (x * Float(0.5) + Float(0.5));
		}

		template <class Float>
		[[nodiscard]]
		inline constexpr Float Clamp_11(const Float x) noexcept
		{
			return std::clamp(x, Float(-1.0), Float(1.0));
		}

		template <class Float>
		[[nodiscard]]
		inline constexpr Float RemapClamp_01(const Float x) noexcept
		{
			if (x <= Float(-1.0))
			{
				return Float(0.0);
			}
			else if (Float(1.0) <= x)
			{
				return Float(1.0);
			}

			return (x * Float(0.5) + Float(0.5));
		}

		template <class Noise, class Float>
		[[nodiscard]]
		inline auto Octave1D(const Noise& noise, Float x, const std::int32_t octaves, const Float persistence) noexcept
		{
			using value_type = Float;
			value_type result = 0;
			value_type amplitude = 1;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += (noise.noise1D(x) * amplitude);
				x *= 2;
				amplitude *= persistence;
			}

			return result;
		}

		template <class Noise, class Float>
		[[nodiscard]]
		inline auto Octave2D(const Noise& noise, Float x, Float y, const std::int32_t octaves, const Float persistence) noexcept
		{
			using value_type = Float;
			value_type result = 0;
			value_type amplitude = 1;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += (noise.noise2D(x, y) * amplitude);
				x *= 2;
				y *= 2;
				amplitude *= persistence;
			}

			return result;
		}

		template <class Noise, class Float>
		[[nodiscard]]
		inline auto Octave3D(const Noise& noise, Float x, Float y, Float z, const std::int32_t octaves, const Float persistence) noexcept
		{
			using value_type = Float;
			value_type result = 0;
			value_type amplitude = 1;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += (noise.noise3D(x, y, z) * amplitude);
				x *= 2;
				y *= 2;
				z *= 2;
				amplitude *= persistence;
			}

			return result;
		}

		template <class Noise, class Float>
		[[nodiscard]]
		inline auto Octave4D(const Noise& noise, Float x, Float y, Float z, Float w, const std::int32_t octaves, const Float persistence) noexcept
		{
			using value_type = Float;
			value_type result = 0;
			value_type amplitude = 1;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += (noise.noise4D(x, y, z, w) * amplitude);
				x *= 2;
				y *= 2;
				z *= 2;
				w *= 2;
				amplitude *= persistence;
			}

			return result;
		}

		template <class Float>
		[[nodiscard]]
		inline constexpr Float MaxAmplitude(const std::int32_t octaves, const Float persistence) noexcept
		{
			using value_type = Float;
			value_type result = 0;
			value_type amplitude = 1;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += amplitude;
				amplitude *= persistence;
			}

			return result;
		}
	}

	///////////////////////////////////////

	template <class Float>
	inline constexpr BasicPerlinNoise<Float>::BasicPerlinNoise() noexcept
		: m_permutation{ 151,160,137,91,90,15,
				131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
				190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
				88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
				77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
				102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
				135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
				5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
				223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
				129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
				251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
				49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
				138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 } {}

	template <class Float>
	inline BasicPerlinNoise<Float>::BasicPerlinNoise(const seed_type seed)
	{
		reseed(seed);
	}

	template <class Float>
	SIVPERLIN_CONCEPT_URBG_
	inline BasicPerlinNoise<Float>::BasicPerlinNoise(URBG&& urbg)
	{
		reseed(std::forward<URBG>(urbg));
	}

	///////////////////////////////////////

	template <class Float>
	inline void BasicPerlinNoise<Float>::reseed(const seed_type seed)
	{
		reseed(default_random_engine{ seed });
	}

	template <class Float>
	SIVPERLIN_CONCEPT_URBG_
	inline void BasicPerlinNoise<Float>::reseed(URBG&& urbg)
	{
		std::iota(m_permutation.begin(), m_permutation.end(), uint8_t{ 0 });

		perlin_detail::Shuffle(m_permutation.begin(), m_permutation.end(), std::forward<URBG>(urbg));
	}

	///////////////////////////////////////

	template <class Float>
	inline constexpr const typename BasicPerlinNoise<Float>::state_type& BasicPerlinNoise<Float>::serialize() const noexcept
	{
		return m_permutation;
	}

	template <class Float>
	inline constexpr void BasicPerlinNoise<Float>::deserialize(const state_type& state) noexcept
	{
		m_permutation = state;
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise1D(const value_type x) const noexcept
	{
		return noise3D(x,
			static_cast<value_type>(SIVPERLIN_DEFAULT_Y),
			static_cast<value_type>(SIVPERLIN_DEFAULT_Z));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise2D(const value_type x, const value_type y) const noexcept
	{
		return noise3D(x,
			y,
			static_cast<value_type>(SIVPERLIN_DEFAULT_Z));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise3D(const value_type x, const value_type y, const value_type z) const noexcept
	{
		const value_type _x = std::floor(x);
		const value_type _y = std::floor(y);
		const value_type _z = std::floor(z);

		const std::int32_t ix = static_cast<std::int32_t>(_x) & 255;
		const std::int32_t iy = static_cast<std::int32_t>(_y) & 255;
		const std::int32_t iz = static_cast<std::int32_t>(_z) & 255;

		const value_type fx = (x - _x);
		const value_type fy = (y - _y);
		const value_type fz = (z - _z);

		const value_type u = perlin_detail::Fade(fx);
		const value_type v = perlin_detail::Fade(fy);
		const value_type w = perlin_detail::Fade(fz);

		const std::uint8_t A = (m_permutation[ix & 255] + iy) & 255;
		const std::uint8_t B = (m_permutation[(ix + 1) & 255] + iy) & 255;

		const std::uint8_t AA = (m_permutation[A] + iz) & 255;
		const std::uint8_t AB = (m_permutation[(A + 1) & 255] + iz) & 255;

		const std::uint8_t BA = (m_permutation[B] + iz) & 255;
		const std::uint8_t BB = (m_permutation[(B + 1) & 255] + iz) & 255;

		const value_type p0 = perlin_detail::Grad(m_permutation[AA], fx, fy, fz);
		const value_type p1 = perlin_detail::Grad(m_permutation[BA], fx - 1, fy, fz);
		const value_type p2 = perlin_detail::Grad(m_permutation[AB], fx, fy - 1, fz);
		const value_type p3 = perlin_detail::Grad(m_permutation[BB], fx - 1, fy - 1, fz);
		const value_type p4 = perlin_detail::Grad(m_permutation[(AA + 1) & 255], fx, fy, fz - 1);
		const value_type p5 = perlin_detail::Grad(m_permutation[(BA + 1) & 255], fx - 1, fy, fz - 1);
		const value_type p6 = perlin_detail::Grad(m_permutation[(AB + 1) & 255], fx, fy - 1, fz - 1);
		const value_type p7 = perlin_detail::Grad(m_permutation[(BB + 1) & 255], fx - 1, fy - 1, fz - 1);

		const value_type q0 = perlin_detail::Lerp(p0, p1, u);
		const value_type q1 = perlin_detail::Lerp(p2, p3, u);
		const value_type q2 = perlin_detail::Lerp(p4, p5, u);
		const value_type q3 = perlin_detail::Lerp(p6, p7, u);

		const value_type r0 = perlin_detail::Lerp(q0, q1, v);
		const value_type r1 = perlin_detail::Lerp(q2, q3, v);

		return perlin_detail::Lerp(r0, r1, w);
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise4D(const value_type x, const value_type y, const value_type z, const value_type w) const noexcept
	{
		const value_type _x = std::floor(x);
		const value_type _y = std::floor(y);
		const value_type _z = std::floor(z);
		const value_type _w = std::floor(w);

		const std::int32_t ix = static_cast<std::int32_t>(_x) & 255;
		const std::int32_t iy = static_cast<std::int32_t>(_y) & 255;
		const std::int32_t iz = static_cast<std::int32_t>(_z) & 255;
		const std::int32_t iw = static_cast<std::int32_t>(_w) & 255;

		const value_type fx = (x - _x);
		const value_type fy = (y - _y);
		const value_type fz = (z - _z);
		const value_type fw = (w - _w);

		const value_type s = perlin_detail::Fade(fx);
		const value_type t = perlin_detail::Fade(fy);
		const value_type u = perlin_detail::Fade(fz);
		const value_type v = perlin_detail::Fade(fw);

		int aaaa, aaba, aaab, aabb, abaa, abba, abab, abbb, baaa, baba, baab, babb, bbaa, bbba, bbab, bbbb;
		aaaa = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy) & 255] + iz) & 255] + iw) & 255];
		aaba = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy) & 255] + iz + 1) & 255] + iw) & 255];
		aaab = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy) & 255] + iz) & 255] + iw + 1) & 255];
		aabb = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy) & 255] + iz + 1) & 255] + iw + 1) & 255];
		abaa = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy + 1) & 255] + iz) & 255] + iw) & 255];
		abba = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy + 1) & 255] + iz + 1) & 255] + iw) & 255];
		abab = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy + 1) & 255] + iz) & 255] + iw + 1) & 255];
		abbb = m_permutation[(m_permutation[(m_permutation[(m_permutation[ix] + iy + 1) & 255] + iz + 1) & 255] + iw + 1) & 255];
		baaa = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy) & 255] + iz) & 255] + iw) & 255];
		baba = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy) & 255] + iz + 1) & 255] + iw) & 255];
		baab = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy) & 255] + iz) & 255] + iw + 1) & 255];
		babb = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy) & 255] + iz + 1) & 255] + iw + 1) & 255];
		bbaa = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy + 1) & 255] + iz) & 255] + iw) & 255];
		bbba = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy + 1) & 255] + iz + 1) & 255] + iw) & 255];
		bbab = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy + 1) & 255] + iz) & 255] + iw + 1) & 255];
		bbbb = m_permutation[(m_permutation[(m_permutation[(m_permutation[(ix + 1) & 255] + iy + 1) & 255] + iz + 1) & 255] + iw + 1) & 255];

		const value_type p0 = perlin_detail::Grad4D(aaaa, fx, fy, fz, fw);
		const value_type p1 = perlin_detail::Grad4D(baaa, fx - 1, fy, fz, fw);
		const value_type p2 = perlin_detail::Grad4D(abaa, fx, fy - 1, fz, fw);
		const value_type p3 = perlin_detail::Grad4D(bbaa, fx - 1, fy - 1, fz, fw);
		const value_type p4 = perlin_detail::Grad4D(aaba, fx, fy, fz - 1, fw);
		const value_type p5 = perlin_detail::Grad4D(baba, fx - 1, fy, fz - 1, fw);
		const value_type p6 = perlin_detail::Grad4D(abba, fx, fy - 1, fz - 1, fw);
		const value_type p7 = perlin_detail::Grad4D(bbba, fx - 1, fy - 1, fz - 1, fw);
		const value_type p8 = perlin_detail::Grad4D(aaab, fx, fy, fz, fw - 1);
		const value_type p9 = perlin_detail::Grad4D(baab, fx - 1, fy, fz, fw - 1);
		const value_type pA = perlin_detail::Grad4D(abab, fx, fy - 1, fz, fw - 1);
		const value_type pB = perlin_detail::Grad4D(bbab, fx - 1, fy - 1, fz, fw - 1);
		const value_type pC = perlin_detail::Grad4D(aabb, fx, fy, fz - 1, fw - 1);
		const value_type pD = perlin_detail::Grad4D(babb, fx - 1, fy, fz - 1, fw - 1);
		const value_type pE = perlin_detail::Grad4D(abbb, fx, fy - 1, fz - 1, fw - 1);
		const value_type pF = perlin_detail::Grad4D(bbbb, fx - 1, fy - 1, fz - 1, fw - 1);

		const value_type q0 = perlin_detail::Lerp(p0, p1, s);
		const value_type q1 = perlin_detail::Lerp(p2, p3, s);
		const value_type q2 = perlin_detail::Lerp(p4, p5, s);
		const value_type q3 = perlin_detail::Lerp(p6, p7, s);
		const value_type q4 = perlin_detail::Lerp(p8, p9, s);
		const value_type q5 = perlin_detail::Lerp(pA, pB, s);
		const value_type q6 = perlin_detail::Lerp(pC, pD, s);
		const value_type q7 = perlin_detail::Lerp(pE, pF, s);

		const value_type r0 = perlin_detail::Lerp(q0, q1, t);
		const value_type r1 = perlin_detail::Lerp(q2, q3, t);
		const value_type r2 = perlin_detail::Lerp(q4, q5, t);
		const value_type r3 = perlin_detail::Lerp(q6, q7, t);

		const value_type e0 = perlin_detail::Lerp(r0, r1, u);
		const value_type e1 = perlin_detail::Lerp(r2, r3, u);

		return perlin_detail::Lerp(e0, e1, v);
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise1D_01(const value_type x) const noexcept
	{
		return perlin_detail::Remap_01(noise1D(x));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise2D_01(const value_type x, const value_type y) const noexcept
	{
		return perlin_detail::Remap_01(noise2D(x, y));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise3D_01(const value_type x, const value_type y, const value_type z) const noexcept
	{
		return perlin_detail::Remap_01(noise3D(x, y, z));
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave1D(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Octave1D(*this, x, octaves, persistence);
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave2D(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Octave2D(*this, x, y, octaves, persistence);
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave3D(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Octave3D(*this, x, y, z, octaves, persistence);
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave4D(const value_type x, const value_type y, const value_type z, const value_type w, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Octave4D(*this, x, y, z, w, octaves, persistence);
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave1D_11(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Clamp_11(octave1D(x, octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave2D_11(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Clamp_11(octave2D(x, y, octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave3D_11(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Clamp_11(octave3D(x, y, z, octaves, persistence));
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave1D_01(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::RemapClamp_01(octave1D(x, octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave2D_01(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::RemapClamp_01(octave2D(x, y, octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave3D_01(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::RemapClamp_01(octave3D(x, y, z, octaves, persistence));
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave1D(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return (octave1D(x, octaves, persistence) / perlin_detail::MaxAmplitude(octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave2D(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return (octave2D(x, y, octaves, persistence) / perlin_detail::MaxAmplitude(octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave3D(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return (octave3D(x, y, z, octaves, persistence) / perlin_detail::MaxAmplitude(octaves, persistence));
	}

	///////////////////////////////////////

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave1D_01(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Remap_01(normalizedOctave1D(x, octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave2D_01(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Remap_01(normalizedOctave2D(x, y, octaves, persistence));
	}

	template <class Float>
	inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave3D_01(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
	{
		return perlin_detail::Remap_01(normalizedOctave3D(x, y, z, octaves, persistence));
	}
}

# undef SIVPERLIN_NODISCARD_CXX20
# undef SIVPERLIN_CONCEPT_URBG
# undef SIVPERLIN_CONCEPT_URBG_
