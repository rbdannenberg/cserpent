#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "any.h"
const int N = 100;

std::mt19937_64 rnd(std::chrono::steady_clock::now().time_since_epoch().count());

int main() {
	#ifdef DEBUG
    printf("Debug mode:\n");
    #endif
	{
		printf("================================\n");
		printf("Testing integer: \n");
		printf("Testing valid integer: ");
		for (int i = 0; i < N; i++) {
			int64_t rngnum = rnd() & (0x1FFFFFFFFFFFF);
			Any T = constructor(rngnum);
			if (!is_int(T)) {
				printf("Test failed on TC %d: input %lld\n", i, rngnum);
				return 0;
			}
			if (is_real(T) || is_ptr(T)) {
				printf("Test failed on TC %d: input %lld\n", i, rngnum);
				return 0;
			}
			int64_t result = to_int(T);
			if (result != rngnum) {
				printf("Test failed on TC %d: input %lld, output %lld\n", i, rngnum, result);
				return 0;
			}

			T = constructor(-rngnum);
			if (!is_int(T)) {
				printf("Test failed on TC %d: input %lld\n", i, -rngnum);
				return 0;
			}
			if (is_real(T) || is_ptr(T)) {
				printf("Test failed on TC %d: input %lld\n", i, -rngnum);
				return 0;
			}
			result = to_int(T);
			if (result != -rngnum) {
				printf("Test failed on TC %d: input %lld, output %lld\n", i, -rngnum, result);
				return 0;
			}
		}
		printf("All tests passed!\n");
//		printf("Testing invalid integer: ");
//		for (int i = 0; i < N; i++) {
//			int64_t rngnum = rnd() & (0xFFF);
//			rngnum += 0x1FFFFFFFFFFFF;
//			if (rngnum & 1) rngnum = -rngnum;
//			try {
//				constructor(rngnum);
//			} catch (std::runtime_error& e) {
//				continue;
//			}
//			printf("Test failed on TC %d: input %lld\n", i, rngnum);
//			return 0;
//		}
//		printf("All tests passed!\n");
//		printf("================================\n");
	}

	{
		printf("Testing double: ");
		for (int i = 0; i < N; i++) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<double> dis(-1.0, 1.0);
			double randomDouble = dis(gen);
			Any T = constructor(randomDouble);
			if (!is_real(T)) {
				printf("Test failed on TC %d: input %f\n", i, randomDouble);
				return 0;
			}
			if (is_int(T) || is_ptr(T)) {
				printf("Test failed on TC %d: input %f\n", i, randomDouble);
				return 0;
			}
			double result = to_real(T);
			if (result != randomDouble) {
				printf("Test failed on TC %d: input %f, output %f\n", i, randomDouble, result);
				return 0;
			}
		}
		printf("All tests passed!\n");
		printf("================================\n");
	}

	// {
	// 	printf("Testing pointer: ");
	// 	u_int64_t** a = new uint64_t*;
	// 	for (int i = 0; i < N; i++) {
	// 		uint64_t rngnum = rnd();
	// 		uint64_t* b = new uint64_t(rngnum);
	// 		Any T = constructor(b);
	// 		if (!is_ptr(T)) {
	// 			printf("Test failed on TC %d: input %p\n", i, b);
	// 			return 0;
	// 		}
	// 		if (is_int(T) || is_real(T)) {
	// 			printf("Test failed on TC %d: input %f\n", i, b);
	// 			return 0;
	// 		}
	// 		void* result = to_ptr(T);
	// 	}
	// 	printf("All tests passed!\n");
	// 	printf("================================\n");
    // }
}
