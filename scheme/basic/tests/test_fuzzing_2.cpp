#include <scheme.h>

#include "scheme_test.h"
#include <fuzzer.h>

#include <iostream>
//#define SCHEME_FUZZING_2_PRINT_REQUESTS
static constexpr uint32_t kShotsCount = 100000;

TEST_CASE("Fuzzing-2") {
    Fuzzer fuzzer;
    Interpreter interpreter;

    for (uint32_t i = 0; i < kShotsCount; ++i) {
        try {
            auto req = fuzzer.Next();
#ifdef SCHEME_FUZZING_2_PRINT_REQUESTS
            std::cerr << "[ " << i << " ] " << req << std::endl;
#endif
//            if (i == 5383) {
//                interpreter.Run(req);
////                break;
//            }
            interpreter.Run(req);
        } catch (const SyntaxError&) {
        } catch (const RuntimeError&) {
        } catch (const NameError&) {
        } catch (...) {

        }
    }
}
