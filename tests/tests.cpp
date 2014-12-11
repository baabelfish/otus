#include "../lib/ytest/ytest.hpp"

yTestPackage pkg([]{
    describe("test", []{
        it("compiles", []{
            Assert().isEqual(true, true);
        });
    });
});

yTestExec();
