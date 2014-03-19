#ifndef __MODULE__MATH_
#define __MODULE__MATH_
#line 6 "Math.mpp"
namespace Math {
#line 7 "Math.mpp"
extern const double PI;
#line 9 "Math.mpp"
inline double sqr (double x)
{
return x * x;}
#line 13 "Math.mpp"
inline double addPercentage (double val, double x)
{
return ( val * ( 1 + x ) );}
#line 17 "Math.mpp"
inline double removePercentage (double val, double x)
{
return ( val - ( val * x ) );}
} // namespace Math
#line 20 "Math.mpp"
#endif // module Math

