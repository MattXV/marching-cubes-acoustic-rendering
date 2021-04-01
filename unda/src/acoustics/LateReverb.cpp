#include "LateReverb.h"

namespace unda {
	void generateTail(float* input, float* output, size_t nSamples, size_t nAllPassFilters)
	{
        {
            float outL, outR, input;
            int i;

            while (nSamples-- > 0)
            {
                outL = outR = 0;
                input = (*inputL + *inputR) * gain;

                // Accumulate comb filters in parallel
                for (i = 0; i < numcombs; i++) {
                    outL += combL[i].process(input);
                    outR += combR[i].process(input);
                }

                // Feed through allpasses in series
                for (i = 0; i < nAllPassFilters; i++) {
                    outL = allpassL[i].process(outL);
                    outR = allpassR[i].process(outR);
                }

                // Calculate output REPLACING anything already there
                output 
                //*outputL = outL * wet1 + outR * wet2 + *inputL * dry;
                //*outputR = outR * wet1 + outL * wet2 + *inputR * dry;

                // Increment sample pointers, allowing for interleave 
                // (if any)
                inputL += skip; // For stereo buffers, skip = 2
                inputR += skip;
                outputL += skip;
                outputR += skip;
            }
        }
	}
}