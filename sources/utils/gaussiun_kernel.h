#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <functional>

namespace misc
{
	std::vector<float> GaussKernel(float sigma, int kernelSize, int sampleCount = 1000);

	namespace detail
	{
		inline float gaussian(float x, float mu, float sigma)
		{
			float d = x - mu;
			float n = 1.0f / (sqrtf(2.0f * (float) M_PI) * sigma);
			return expf(-d * d / (2 * sigma * sigma)) * n;
		}

		typedef std::function<float(float)> Func;
		typedef std::pair<float, float> Sample;

		inline std::vector<Sample> SampleInterval(const Func& f, float minInclusive, float maxInclusive, int sampleCount)
		{
			std::vector<Sample> result;
			float stepSize = (maxInclusive - minInclusive) / (sampleCount - 1);

			for (int s = 0; s < sampleCount; ++s)
			{
				float x = minInclusive + s * stepSize;
				result.emplace_back(x, f(x));
			}

			return result;
		}

		inline float IntegrateSimphson(std::vector<Sample> samples)
		{
			auto result = samples[0].second + samples[samples.size() - 1].second;

			for (size_t s = 1; s < samples.size() - 1; ++s)
			{
				float sampleWeight = (s % 2 == 0) ? 2.0f : 4.0f;
				result += sampleWeight * samples[s].second;
			}

			float h = (samples[samples.size() - 1].first - samples[0].first) / (samples.size() - 1);
			return result * h / 3.0f;
		}
	}

	inline std::vector<float> GaussKernel(float sigma, int kernelSize, int sampleCount)
	{
		if (kernelSize == 1)
		{
			return { 1.0f };
		}
		int samplesPerBin = sampleCount / kernelSize;
		if (samplesPerBin % 2 == 0)
		{
			++samplesPerBin;
		}

		int kernelLeft = -kernelSize / 2;

		auto calcSamplesForRange = [sigma, samplesPerBin](float minInclusive, float maxInclusive)
		{
			return detail::SampleInterval(
					[sigma](float x){ return detail::gaussian(x, 0, sigma); },
					minInclusive,
			        maxInclusive,
			        samplesPerBin
			);
		};

		auto outsideSamplesLeft = calcSamplesForRange(-5.0f * sigma, kernelLeft - 0.5f);
		auto outsideSamplesRight = calcSamplesForRange(-kernelLeft + 0.5f, 5.0f * sigma);

		std::vector<std::pair<std::vector<detail::Sample>, float> > allSamples = { { outsideSamplesLeft, 0.0f }, };

		float weightSum = 0;
		for (int tap = 0; tap < kernelSize; ++tap)
		{
			float left = kernelLeft - 0.5f + (float) tap;

			auto tapSamples = calcSamplesForRange(left, left + 1.0f);
			float tapWeight = detail::IntegrateSimphson(tapSamples);

			allSamples.emplace_back(tapSamples, tapWeight);
			weightSum += tapWeight;
		}

		allSamples.emplace_back(outsideSamplesRight, 0.0f);

		std::vector<float> weights;
		for (size_t i = 1; i < allSamples.size() - 1; ++i)
		{
			weights.push_back(allSamples[i].second / weightSum);
		}
		return weights;
	}
}
