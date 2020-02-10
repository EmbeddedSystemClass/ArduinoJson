// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <sstream>
#include <string>

TEST_CASE("Filtering") {
  struct TestCase {
    const char* input;
    const char* filter;
    DeserializationError error;
    const char* output;
    size_t memoryUsage;
  };

  // clang-format off
  TestCase testCases[] = {
    {
      "{\"hello\":\"world\"}",   // 1. input
      "null",                    // 2. filter
      DeserializationError::Ok,  // 3. error
      "null",                    // 4. output
      0                          // 5. memoryUsage
    },
    {
      "{\"hello\":\"world\"}",
      "false",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      "{\"abcdefg\":\"hijklmn\"}",
      "true",
      DeserializationError::Ok,
      "{\"abcdefg\":\"hijklmn\"}",
      JSON_OBJECT_SIZE(1) + 16
    },
    {
      "{\"hello\":\"world\"}",
      "{}",
      DeserializationError::Ok,
      "{}",
      JSON_OBJECT_SIZE(0)
    },
    {
      // Input in an object, but filter wants an array
      "{\"hello\":\"world\"}",
      "[]",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // Input is an array, but filter wants an object
      "[\"hello\",\"world\"]",
      "{}",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // Input is a bool, but filter wants an object
      "true",
      "{}",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // Input is a string, but filter wants an object
      "\"hello\"",
      "{}",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // skip an integer
      "{\"an_integer\":666,example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // skip a float
      "{\"a_float\":12.34e-6,example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip a boolean
      "{\"a_bool\":false,example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip a double-quoted string
      "{\"a_double_quoted_string\":\"hello\",example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip a single-quoted string
      "{\"a_single_quoted_string\":'hello',example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an empty array
      "{\"an_empty_array\":[],example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an empty array with spaces in it
      "{\"an_empty_array\":[\t],example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an array
      "{\"an_array\":[1,2,3],example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an array with spaces in it
      "{\"an_array\": [ 1 , 2 , 3 ] ,example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an empty object
      "{\"an_empty_object\":{},example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an empty object with spaces in it
      "{\"an_empty_object\":{    },example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // can skip an object
      "{\"an_object\":{a:1,'b':2,\"c\":3},example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      // skip an object with spaces in it
      "{\"an_object\" : { a : 1 , 'b' : 2 , \"c\" : 3 } ,example:42}",
      "{\"example\":true}",
      DeserializationError::Ok,
      "{\"example\":42}",
      JSON_OBJECT_SIZE(1) + 8
    },
    {
      "{\"an_integer\": 0,\"example\":{\"type\":\"int\",\"outcome\":42}}",
      "{\"example\":{\"outcome\":true}}",
      DeserializationError::Ok,
      "{\"example\":{\"outcome\":42}}",
      2 * JSON_OBJECT_SIZE(1) + 16
    },
    {
      "[1,2,3]",
      "[false,true]",
      DeserializationError::Ok,
      "[2]",
      JSON_ARRAY_SIZE(1)
    },
    {
      "[1,[2.1,2.2,2.3],3]",
      "[false,[false, true]]",
      DeserializationError::Ok,
      "[[2.2]]",
      2*JSON_ARRAY_SIZE(1)
    },
    {
      "[',2,3]",
      "[false,true]",
      DeserializationError::IncompleteInput,
      "[]",
      JSON_ARRAY_SIZE(0)
    },
    {
      "[\",2,3]",
      "[false,true]",
      DeserializationError::IncompleteInput,
      "[]",
      JSON_ARRAY_SIZE(0)
    },
    {
      // ignore errors in skipped value
      "[!,2,\\]",
      "[false,true]",
      DeserializationError::Ok,
      "[2]",
      JSON_ARRAY_SIZE(1)
    },
    {
      // detect incomplete string event if it's skipped
      "\"ABC",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // detect incomplete string event if it's skipped
      "'ABC",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // handle escaped quotes
      "'A\\'BC'",
      "false",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // handle escaped quotes
      "\"A\\\"BC\"",
      "false",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // detect incomplete string in presence of escaped quotes
      "'A\\'BC",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // detect incomplete string in presence of escaped quotes
      "\"A\\\"BC",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // skip empty array
      "[]",
      "false",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // skip empty array with spaces
      " [ ] ",
      "false",
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // bubble up element error even if array is skipped 
      "[1,'2,3]",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // bubble up member error even if object is skipped 
      "{'hello':'worl}",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // bubble up colon error even if object is skipped 
      "{'hello','world'}",
      "false",
      DeserializationError::InvalidInput,
      "null",
      0
    },
    {
      // bubble up key error even if object is skipped 
      "{'hello:1}",
      "false",
      DeserializationError::IncompleteInput,
      "null",
      0
    },
    {
      // ignore invalid value in skipped object
      "{'hello':!}",
      "false", 
      DeserializationError::Ok,
      "null",
      0
    },
    {
      // ignore invalid value in skipped object
      "{'hello':\\}",
      "false", 
      DeserializationError::Ok,
      "null", 
      0
    }
  };  // clang-format on

  for (size_t i = 0; i < sizeof(testCases) / sizeof(testCases[0]); i++) {
    CAPTURE(i);

    DynamicJsonDocument filter(256);
    DynamicJsonDocument doc(256);
    TestCase& tc = testCases[i];

    CAPTURE(tc.filter);
    REQUIRE(deserializeJson(filter, tc.filter) == DeserializationError::Ok);

    CAPTURE(tc.input);
    CHECK(deserializeJson(doc, tc.input,
                          DeserializationOption::Filter(filter)) == tc.error);

    CHECK(doc.as<std::string>() == tc.output);
    CHECK(doc.memoryUsage() == tc.memoryUsage);
  }
}

TEST_CASE("Overloads") {
  StaticJsonDocument<256> doc;
  StaticJsonDocument<256> filter;

  using namespace DeserializationOption;

  // deserializeJson(..., Filter)

  SECTION("const char*, Filter") {
    deserializeJson(doc, "{}", Filter(filter));
  }

  SECTION("const char*, size_t, Filter") {
    deserializeJson(doc, "{}", 2, Filter(filter));
  }

  SECTION("const std::string&, Filter") {
    deserializeJson(doc, std::string("{}"), Filter(filter));
  }

  SECTION("std::istream&, Filter") {
    std::stringstream s("{}");
    deserializeJson(doc, s, Filter(filter));
  }

#ifdef HAS_VARIABLE_LENGTH_ARRAY
  SECTION("char[n], Filter") {
    int i = 4;
    char vla[i];
    strcpy(vla, "{}");
    deserializeJson(doc, vla, Filter(filter));
  }
#endif

  // deserializeJson(..., Filter, NestingLimit)

  SECTION("const char*, Filter, NestingLimit") {
    deserializeJson(doc, "{}", Filter(filter), NestingLimit(5));
  }

  SECTION("const char*, size_t, Filter, NestingLimit") {
    deserializeJson(doc, "{}", 2, Filter(filter), NestingLimit(5));
  }

  SECTION("const std::string&, Filter, NestingLimit") {
    deserializeJson(doc, std::string("{}"), Filter(filter), NestingLimit(5));
  }

  SECTION("std::istream&, Filter, NestingLimit") {
    std::stringstream s("{}");
    deserializeJson(doc, s, Filter(filter), NestingLimit(5));
  }

#ifdef HAS_VARIABLE_LENGTH_ARRAY
  SECTION("char[n], Filter, NestingLimit") {
    int i = 4;
    char vla[i];
    strcpy(vla, "{}");
    deserializeJson(doc, vla, Filter(filter), NestingLimit(5));
  }
#endif

  // deserializeJson(..., NestingLimit, Filter)

  SECTION("const char*, NestingLimit, Filter") {
    deserializeJson(doc, "{}", NestingLimit(5), Filter(filter));
  }

  SECTION("const char*, size_t, NestingLimit, Filter") {
    deserializeJson(doc, "{}", 2, NestingLimit(5), Filter(filter));
  }

  SECTION("const std::string&, NestingLimit, Filter") {
    deserializeJson(doc, std::string("{}"), NestingLimit(5), Filter(filter));
  }

  SECTION("std::istream&, NestingLimit, Filter") {
    std::stringstream s("{}");
    deserializeJson(doc, s, NestingLimit(5), Filter(filter));
  }

#ifdef HAS_VARIABLE_LENGTH_ARRAY
  SECTION("char[n], NestingLimit, Filter") {
    int i = 4;
    char vla[i];
    strcpy(vla, "{}");
    deserializeJson(doc, vla, NestingLimit(5), Filter(filter));
  }
#endif
}
