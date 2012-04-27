#ifndef _UTIL_H_
#define _UTIL_H_
//util.h
//Author: Simon Wittenberg


#define Unless(x) if(!bool(x))
#define Return_False_Unless(x) Unless(x) return false
#define Return_True_Unless(x) Unless(x) return true
#define Return_Void_Unless(x) Unless(x) return
#define Return_Zero_Unless(x) Unless(x) return 0
#define Return_NULL_Unless(x) Return_Zero_Unless(x)
#define Return_One_Unless(x) Unless(x) return 1
#define Return_MinusOne_Unless(x) Unless(x) return -1

#endif