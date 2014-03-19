#ifndef __MODULE__UTILS_MATH_
#define __MODULE__UTILS_MATH_
#line 6 "Utils.Math.mpp"
namespace Utils {
#line 7 "Utils.Math.mpp"
namespace Math {
#line 8 "Utils.Math.mpp"
extern const double PI;
#line 10 "Utils.Math.mpp"
inline double sqr (double x)
{
return x * x;}
#line 14 "Utils.Math.mpp"
inline double addPercentage (double val, double x)
{
return ( val * ( 1 + x ) );}
#line 18 "Utils.Math.mpp"
inline double removePercentage (double val, double x)
{
return ( val - ( val * x ) );}
} // namespace Math
#line 21 "Utils.Math.mpp"
} // namespace Utils
#line 22 "Utils.Math.mpp"
#endif // module Utils_Math

