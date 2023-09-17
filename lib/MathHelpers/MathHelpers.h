/*
    Math Helpers
    Created by Grant Keller, November 2022
*/

#ifndef MathHelpers_h
#define MathHelpers_h

class MathHelpers {
    public:
    static int degreeToSixteenbit(int degree);
    static float mPower(float a, int b);
    static long map(long x, long in_min, long in_max, long out_min, long out_max);
};

#endif
