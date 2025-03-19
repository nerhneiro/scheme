#include "scheme_test.h"

TEST_CASE_METHOD(SchemeTest, "Quote") {
    Interpreter interpreter;
    interpreter.Run("(quote (1 2))");
    ExpectEq("(quote (1 2))", "(1 2)");
    ExpectEq("'(1 2)", "(1 2)");
}

TEST_CASE_METHOD(SchemeTest, "Be careful") {
    ExpectRuntimeError("(())");
    ExpectRuntimeError("(+ ())");
//    Interpreter interpreter;
//    interpreter.Run("('() ())");
    ExpectRuntimeError("('() ())");
    ExpectEq("'(())", "(())");
}
