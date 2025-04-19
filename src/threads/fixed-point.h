//
// Created by ziadh on 19/04/2025.
//

#ifndef FIXED_POINT_H
#define FIXED_POINT_H

typedef int fixed_t;

#define FP_SUB_POINT_BITS 14
#define FP_SUR_POINT_BITS 17

#define INT_TO_FP(x) ((fixed_t) (x << (FP_SUB_POINT_BITS+1)))
#define FP_TO_INT(x) ((fixed_t) (x >> (FP_SUR_POINT_BITS+1)))

#define FP_TO_INT_ROUND(x) (x>=0? x+(1<<FP_SUB_POINT_BITS)>> (FP_SUB_POINT_BITS+1): x-(1<<FP_SUB_POINT_BITS) >> (FP_SUR_POINT_BITS+1))

#define FP_FP_ADD(x,y) (x+y)
#define FP_FP_SUB(x,y) (x-y)
#define FP_FP_MUL(x,y) ((fixed_t)(((int64_t) x) *  y >> (FP_SUB_POINT_BITS+1)))
#define FP_FP_DIV(x,y) ((fixed_t)((((int64_t) x) << (FP_SUB_POINT_BITS+1)) / y))
#define FP_INT_MUL(x,y) (x*y)
#define FP_INT_DIV(x,y) (x/y)



#endif //FIXED_POINT_H
