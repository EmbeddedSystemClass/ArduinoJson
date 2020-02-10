// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License

#pragma once

#include <ArduinoJson/Deserialization/DeserializationError.hpp>
#include <ArduinoJson/Deserialization/Filter.hpp>
#include <ArduinoJson/Deserialization/NestingLimit.hpp>
#include <ArduinoJson/Deserialization/Reader.hpp>
#include <ArduinoJson/StringStorage/StringStorage.hpp>

namespace ARDUINOJSON_NAMESPACE {

template <template <typename, typename> class TDeserializer, typename TReader,
          typename TWriter>
TDeserializer<TReader, TWriter> makeDeserializer(MemoryPool &pool,
                                                 TReader reader, TWriter writer,
                                                 uint8_t nestingLimit) {
  return TDeserializer<TReader, TWriter>(pool, reader, writer, nestingLimit);
}

// deserialize(JsonDocument&, const std::string&);
// deserialize(JsonDocument&, const String&);
// deserialize(JsonDocument&, char*);
// deserialize(JsonDocument&, const char*);
// deserialize(JsonDocument&, const __FlashStringHelper*);
template <template <typename, typename> class TDeserializer, typename TString>
typename enable_if<!is_array<TString>::value, DeserializationError>::type
deserialize(JsonDocument &doc, const TString &input,
            NestingLimit nestingLimit) {
  return deserialize<TDeserializer>(doc, input, nestingLimit, AllowAllFilter());
}
//
// deserialize(JsonDocument&, const std::string&, Filter);
// deserialize(JsonDocument&, const String&, Filter);
// deserialize(JsonDocument&, char*, Filter);
// deserialize(JsonDocument&, const char*, Filter);
// deserialize(JsonDocument&, const __FlashStringHelper*, Filter);
template <template <typename, typename> class TDeserializer, typename TString,
          typename TFilter>
typename enable_if<!is_array<TString>::value, DeserializationError>::type
deserialize(JsonDocument &doc, const TString &input, NestingLimit nestingLimit,
            TFilter filter) {
  Reader<TString> reader(input);
  doc.clear();
  return makeDeserializer<TDeserializer>(
             doc.memoryPool(), reader,
             makeStringStorage(doc.memoryPool(), input), nestingLimit.value)
      .parse(doc.data(), filter);
}

// deserialize(JsonDocument&, char*, size_t);
// deserialize(JsonDocument&, const char*, size_t);
// deserialize(JsonDocument&, const __FlashStringHelper*, size_t);
template <template <typename, typename> class TDeserializer, typename TChar>
DeserializationError deserialize(JsonDocument &doc, TChar *input,
                                 size_t inputSize, NestingLimit nestingLimit) {
  return deserialize<TDeserializer>(doc, input, inputSize, nestingLimit,
                                    AllowAllFilter());
}
//
// deserialize(JsonDocument&, char*, size_t, Filter);
// deserialize(JsonDocument&, const char*, size_t, Filter);
// deserialize(JsonDocument&, const __FlashStringHelper*, size_t, Filter);
template <template <typename, typename> class TDeserializer, typename TChar,
          typename TFilter>
DeserializationError deserialize(JsonDocument &doc, TChar *input,
                                 size_t inputSize, NestingLimit nestingLimit,
                                 TFilter filter) {
  BoundedReader<TChar *> reader(input, inputSize);
  doc.clear();
  return makeDeserializer<TDeserializer>(
             doc.memoryPool(), reader,
             makeStringStorage(doc.memoryPool(), input), nestingLimit.value)
      .parse(doc.data(), filter);
}

// deserialize(JsonDocument&, std::istream&);
// deserialize(JsonDocument&, Stream&);
template <template <typename, typename> class TDeserializer, typename TStream>
DeserializationError deserialize(JsonDocument &doc, TStream &input,
                                 NestingLimit nestingLimit) {
  return deserialize<TDeserializer>(doc, input, nestingLimit, AllowAllFilter());
}
//
// deserialize(JsonDocument&, std::istream&, Filter);
// deserialize(JsonDocument&, Stream&, Filter);
template <template <typename, typename> class TDeserializer, typename TStream,
          typename TFilter>
DeserializationError deserialize(JsonDocument &doc, TStream &input,
                                 NestingLimit nestingLimit, TFilter filter) {
  Reader<TStream> reader(input);
  doc.clear();
  return makeDeserializer<TDeserializer>(
             doc.memoryPool(), reader,
             makeStringStorage(doc.memoryPool(), input), nestingLimit.value)
      .parse(doc.data(), filter);
}

}  // namespace ARDUINOJSON_NAMESPACE
