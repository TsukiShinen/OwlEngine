#pragma once
#include "Owl/Debug/Log.h"

#define ExpectShouldBe(expected, actual)                                                                \
	if (actual != expected) {																			\
		OWL_ERROR("--> Expected {0}, but got {1}. File {2}, ligne {3}", expected, actual, __FILE__, __LINE__); \
		return false;																					\
	}																									\

#define ExpectShouldNotBe(expected, actual)																				\
	if (actual == expected) {																							\
		OWL_ERROR("--> Expected {0} != {1}, but they are equal. File {2}, ligne {3}", expected, actual, __FILE__, __LINE__);	\
		return false;																									\
	}																													\

#define ExpectFloatShouldBe(expected, actual)                                                           \
	if (glm::abs(expected - actual) > 0.001f) {															\
		OWL_ERROR("--> Expected {0}, but got {1}. File {2}, ligne {3}", expected, actual, __FILE__, __LINE__); \
		return false;																					\
	}																									\

#define ExpectToBeTrue(actual)                                                          \
	if (actual != true) {																\
		OWL_ERROR("--> Expected true but got false. File {0}, ligne {1}", __FILE__, __LINE__); \
		return false;																	\
	}																					\

#define ExpectToBeFalse(actual)                                                         \
	if (actual != false) {																\
		OWL_ERROR("--> Expected false but got true. File {0}, ligne {1}", __FILE__, __LINE__); \
		return false;																	\
	}	
