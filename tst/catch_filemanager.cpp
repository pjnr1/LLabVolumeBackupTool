//
// Created by Jens Lindahl on 04/06/2018.
//

#include "catch.hpp"

#include "../lib/filemanager.h"


TEST_CASE("basic member tests","[full],[filemanager]") {
    FileManager fm;

    SECTION("isFolder") {
        REQUIRE(fm.isFolder("./"));
        REQUIRE(!fm.isFolder("./Makefile"));
    }

    SECTION("fileExists") {
        REQUIRE(fm.fileExists("./Makefile"));
        REQUIRE(fm.fileExists("./CmakeCache.txt"));
        REQUIRE(!fm.fileExists("./CMakeFiles"));
        REQUIRE(!fm.fileExists("./ekasdfafile"));
    }

    SECTION("copyFile") {
        REQUIRE(fm.copyFile("./Makefile","./Makefile_copy", true));
        REQUIRE(!fm.copyFile("./Makefile","./Makefile_copy", false));
    }

    SECTION("clean up after test case") {
        REQUIRE(fm.re)
    }
}