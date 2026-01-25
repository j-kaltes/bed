#pragma once
#include <stdio.h>
#include <stdint.h>
#define _GNU_SOURCE 1
#include <math.h>
static constexpr const uint16_t NRes=0x0800;
union shortfloat {
     uint16_t whole;
     struct {
        int16_t mantissa:12;
        int16_t exponent:4;
        };
     operator float() const {
        return getvalue();
        }
     float getvalue() const {
        switch(whole) {
                case 0x07FE:
                          if constexpr(std::numeric_limits<float>::has_infinity)
                                return std::numeric_limits<float>::infinity();
                case 0x0802:
                        if constexpr(std::numeric_limits<float>::has_infinity)
                                return -std::numeric_limits<float>::infinity();
                case 0x07FF:
                case 0x0801:
                case NRes: return NAN;
                default: return (float) mantissa*powf(10.0f,exponent);
                };

        }
#ifndef exp10f
#define exp10f(exponent) powf(10.0f,exponent)
#endif

static  shortfloat   fromdouble(double value) {
        if(value>20470000000.0)
                return {.whole=NRes};
        if(value<-20480000000.0)
                return {.whole=NRes}; 
        int16_t mantissa,exponent;
        if(!value) {
            exponent=0;
            mantissa=0;
            }
        else  {
            double left;
            if(value>0) {
                int16_t rounded; 
                if(value<=2045 &&(rounded=round(value),rounded==value)) {
                    mantissa=rounded; 
                    exponent=0;
                    }
                else {
                    left=value/2045;
                    exponent=(int16_t)ceil(log10(left));
                    mantissa=(int16_t)round(value/exp10f(exponent));
                    }
                }
            else {
                int16_t rounded; 
                if(value>=-2045 &&(rounded=round(value),rounded==value)) {
                    mantissa=rounded; 
                    exponent=0;
                    }
                  else {
                     left=-value/2045;
                     exponent=(int16_t)ceil(log10(left));
                     mantissa=(int16_t)round(value/exp10f(exponent));
                     }
                }
            }
        return {.mantissa=mantissa,.exponent=exponent};
        };
     };

