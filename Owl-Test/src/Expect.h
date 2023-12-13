#pragma once
#include "Owl/Debug/Log.h"

#define ExpectShouldBe(expected, actual)                                                                \
	if (actual != expected) {																			\
		OWL_ERROR("--> Expected  %lld, but got  %lld. File %s, ligne %d", expected, actual, __FILE__, __LINE__); \
		return false;																					\
	}																									\

#define ExpectShouldNotBe(expected, actual)																				\
	if (actual == expected) {																							\
		OWL_ERROR("--> Expected %lld != %lld, but they are equal. File %s, ligne %d", expected, actual, __FILE__, __LINE__);	\
		return false;																									\
	}																													\

#define ExpectFloatShouldBe(expected, actual)                                                           \
	if (glm::abs(expected - actual) > 0.001f) {															\
		OWL_ERROR("--> Expected %lld, but got %lld. File %s, ligne %d", expected, actual, __FILE__, __LINE__); \
		return false;																					\
	}																									\

#define ExpectToBeTrue(actual)                                                          \
	if (actual != true) {																\
		OWL_ERROR("--> Expected true but got false. File %s, ligne %d", __FILE__, __LINE__); \
		return false;																	\
	}																					\

#define ExpectToBeFalse(actual)                                                         \
	if (actual != false) {																\
		OWL_ERROR("--> Expected false but got true. File %s, ligne %d", __FILE__, __LINE__); \
		return false;																	\
	}	
