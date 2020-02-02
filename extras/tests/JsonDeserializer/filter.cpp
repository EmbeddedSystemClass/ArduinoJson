// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("Filtering") {
  struct TestCase {
    std::string input;
    std::string filter;
    DeserializationError error;
    std::string output;
  };

  // clang-format off
  TestCase testCases[] = {
    {
      "{\"hello\":\"world\"}",   // 1. input
      "null",                    // 2. filter
      DeserializationError::Ok,  // 3. error
      "null"                     // 4. output
    },
    {
      "{\"hello\":\"world\"}",
      "false",
      DeserializationError::Ok,
      "null"
    },
    {
      "{\"hello\":\"world\"}",
      "true",
      DeserializationError::Ok,
      "{\"hello\":\"world\"}"
    },
    {
      "{\"hello\":\"world\"}",
      "{}",
      DeserializationError::Ok,
      "{}"
    }, 
    {
      // Input in an object, but filter wants an array
      "{\"hello\":\"world\"}",
      "[]",
      DeserializationError::Ok,
      "null"
    }, 
    {
      // Input is an array, but filter wants an object
      "[\"hello\",\"world\"]",
      "{}",
      DeserializationError::Ok,
      "null"
    },
    {
      // Input is a bool, but filter wants an object
      "true",
      "{}",
      DeserializationError::Ok,
      "null"
    },
    {
      // Input is a string, but filter wants an object
      "\"hello\"",
      "{}",
      DeserializationError::Ok,
      "null"
    },
    {
      // skip an integer
      "{\"an_integer\":666,answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    }, 
    {
      // skip a float
      "{\"a_float\":12.34e-6,answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip a boolean
      "{\"a_bool\":false,answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip a double-quoted string
      "{\"a_double_quoted_string\":\"hello\",answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip a single-quoted string
      "{\"a_single_quoted_string\":'hello',answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an empty array
      "{\"an_empty_array\":[],answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an empty array with spaces in it
      "{\"an_empty_array\":[\t],answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an array
      "{\"an_array\":[1,2,3],answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an array with spaces in it
      "{\"an_array\": [ 1 , 2 , 3 ] ,answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an empty object
      "{\"an_empty_object\":{},answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an empty object with spaces in it
      "{\"an_empty_object\":{    },answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // can skip an object
      "{\"an_object\":{a:1,'b':2,\"c\":3},answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      // skip an object with spaces in it
      "{\"an_object\" : { a : 1 , 'b' : 2 , \"c\" : 3 } ,answer:42}",
      "{\"answer\":true}",
      DeserializationError::Ok,
      "{\"answer\":42}"
    },
    {
      "{\"an_integer\": 0,\"answer\":{\"type\":\"int\",\"value\":42}}",
      "{\"answer\":{\"value\":true}}",
      DeserializationError::Ok,
      "{\"answer\":{\"value\":42}}"
    },
    {
      "[1,2,3]",
      "[false,true]",
      DeserializationError::Ok,
      "[2]"
    },
    {
      "[1,[2.1,2.2,2.3],3]",
      "[false,[false, true]]",
      DeserializationError::Ok,
      "[[2.2]]"
    },
    {
      "[',2,3]",
      "[false,true]",
      DeserializationError::IncompleteInput,
      "[]"
    },
    {
      "[\",2,3]",
      "[false,true]",
      DeserializationError::IncompleteInput,
      "[]"      
    },
    {
      // ignore errors in skipped value
      "[!,2,\\]",
      "[false,true]",
      DeserializationError::Ok,
      "[2]"
    },
    {
      // detect incomplete string event if it's skipped
      "\"ABC",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // detect incomplete string event if it's skipped
      "'ABC",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // handle escaped quotes
      "'A\\'BC'",
      "false",
      DeserializationError::Ok,
      "null"
    },
    {
      // handle escaped quotes
      "\"A\\\"BC\"",
      "false",
      DeserializationError::Ok,
      "null"
    },
    {
      // detect incomplete string in presence of escaped quotes
      "'A\\'BC",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // detect incomplete string in presence of escaped quotes
      "\"A\\\"BC",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // skip empty array
      "[]",
      "false",
      DeserializationError::Ok,
      "null"
    },
    {
      // skip empty array with spaces
      " [ ] ",
      "false",
      DeserializationError::Ok,
      "null"
    },
    {
      // bubble up element error even if array is skipped 
      "[1,'2,3]",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // bubble up member error even if object is skipped 
      "{'hello':'worl}",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // bubble up colon error even if object is skipped 
      "{'hello','world'}",
      "false",
      DeserializationError::InvalidInput,
      "null"
    },
    {
      // bubble up key error even if object is skipped 
      "{'hello:1}",
      "false",
      DeserializationError::IncompleteInput,
      "null"
    },
    {
      // ignore invalid value in skipped object
      "{'hello':!}",
      "false", 
      DeserializationError::Ok,
      "null"
    },
    {
      // ignore invalid value in skipped object
      "{'hello':\\}",
      "false", 
      DeserializationError::Ok,
      "null"
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
    CHECK(deserializeJson(doc, tc.input.c_str(),
                          DeserializationOption::Filter(filter)) == tc.error);

    CHECK(doc.as<std::string>() == tc.output);
  }
}
