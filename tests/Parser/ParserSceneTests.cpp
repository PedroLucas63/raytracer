#include <catch2/catch_test_macros.hpp>
#include "Core/ParamSet.hpp"
#include "Parser/ParserScene.hpp"
#include "tinyxml2.h"

// ── helper ────────────────────────────────────────────────────────────────────
//  Query the ParamSet passed to the API function for inspection in tests.
static raytracer::ParamSet capturedPs;

static void setupSpy(const std::string& element) {
    raytracer::apiFunctions[element] = [](const raytracer::ParamSet& ps) {
        capturedPs = ps;
    };
}

// ── <film> ────────────────────────────────────────────────────────────────────

TEST_CASE("parseScene stores film filename") {
    setupSpy("film");
    const char* xml = R"(<scene><film filename="out.png"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<std::string>("filename") == "out.png");
}

TEST_CASE("parseScene stores film resolution") {
    setupSpy("film");
    const char* xml = R"(<scene><film x_res="1920" y_res="1080"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<int>("x_res") == 1920);
    REQUIRE(capturedPs.retrieve<int>("y_res") == 1080);
}

TEST_CASE("parseScene stores film gamma_corrected as true") {
    setupSpy("film");
    const char* xml = R"(<scene><film gamma_corrected="true"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<bool>("gamma_corrected"));
}

TEST_CASE("parseScene stores film gamma_corrected as false") {
    setupSpy("film");
    const char* xml = R"(<scene><film gamma_corrected="false"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE_FALSE(capturedPs.retrieve<bool>("gamma_corrected"));
}

// ── <background> ─────────────────────────────────────────────────────────────

TEST_CASE("parseScene stores background color") {
    setupSpy("background");
    const char* xml = R"(<scene><background color="100 149 237"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    const auto color = capturedPs.retrieve<std::vector<std::uint8_t>>("color");
    REQUIRE(color.size() == 3);
    REQUIRE(color[0] == 100);
    REQUIRE(color[1] == 149);
    REQUIRE(color[2] == 237);
}

TEST_CASE("parseScene stores background corner colors") {
    setupSpy("background");
    const char* xml = R"(
        <scene>
            <background tl="255 0 0" tr="0 255 0" bl="0 0 255" br="255 255 0"/>
        </scene>
    )";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    const auto tl = capturedPs.retrieve<std::vector<std::uint8_t>>("tl");
    const auto tr = capturedPs.retrieve<std::vector<std::uint8_t>>("tr");
    const auto bl = capturedPs.retrieve<std::vector<std::uint8_t>>("bl");
    const auto br = capturedPs.retrieve<std::vector<std::uint8_t>>("br");

    REQUIRE(tl[0] == 255); REQUIRE(tl[1] == 0);   REQUIRE(tl[2] == 0);
    REQUIRE(tr[0] == 0);   REQUIRE(tr[1] == 255);  REQUIRE(tr[2] == 0);
    REQUIRE(bl[0] == 0);   REQUIRE(bl[1] == 0);    REQUIRE(bl[2] == 255);
    REQUIRE(br[0] == 255); REQUIRE(br[1] == 255);  REQUIRE(br[2] == 0);
}

// ── elementos inválidos ───────────────────────────────────────────────────────

TEST_CASE("parseScene ignores unknown elements") {
    const char* xml = R"(<scene><unknown_tag foo="bar"/></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

TEST_CASE("parseScene ignores unknown attributes") {
    setupSpy("film");
    const char* xml = R"(<scene><film invalid_attr="x"/></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

TEST_CASE("parseScene handles empty scene") {
    const char* xml = R"(<scene></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

TEST_CASE("parseScene handles malformed XML") {
    const char* xml = R"(<scene><film x_res="abc"/></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

// ── case insensitive ──────────────────────────────────────────────────────────

TEST_CASE("parseScene is case insensitive for element names") {
    setupSpy("film");
    const char* xml = R"(<scene><FILM x_res="800"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<int>("x_res") == 800);
}

TEST_CASE("parseScene is case insensitive for attribute names") {
    setupSpy("film");
    const char* xml = R"(<scene><film X_RES="800"/></scene>)";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<int>("x_res") == 800);
}


TEST_CASE("parseScene stores film attributes from RT3 document") {
    setupSpy("film");
    const char* xml = R"(
        <RT3>
            <film type="image" x_res="200" y_res="100"
                  filename="simple_bkg.ppm" img_type="ppm"/>
        </RT3>
    )";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<std::string>("type")     == "image");
    REQUIRE(capturedPs.retrieve<int>("x_res")            == 200);
    REQUIRE(capturedPs.retrieve<int>("y_res")            == 100);
    REQUIRE(capturedPs.retrieve<std::string>("filename") == "simple_bkg.ppm");
    REQUIRE(capturedPs.retrieve<std::string>("img_type") == "ppm");
}

TEST_CASE("parseScene stores background color from RT3 document") {
    setupSpy("background");
    const char* xml = R"(
        <RT3>
            <world_begin/>
                <background type="single_color" color="153 204 255"/>
            <world_end/>
        </RT3>
    )";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<std::string>("type") == "single_color");

    const auto color = capturedPs.retrieve<std::vector<std::uint8_t>>("color");
    REQUIRE(color.size() == 3);
    REQUIRE(color[0] == 153);
    REQUIRE(color[1] == 204);
    REQUIRE(color[2] == 255);
}

TEST_CASE("parseScene handles world_begin and world_end without throwing") {
    const char* xml = R"(
        <RT3>
            <world_begin/>
            <world_end/>
        </RT3>
    )";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

TEST_CASE("parseScene handles camera element") {
    setupSpy("camera");
    const char* xml = R"(
        <RT3>
            <camera type="orthographic"/>
        </RT3>
    )";

    raytracer::ParserScene parser;
    parser.parseScene(xml, true);

    REQUIRE(capturedPs.retrieve<std::string>("type") == "orthographic");
}

TEST_CASE("parseScene handles full RT3 document without throwing") {
    const char* xml = R"(
        <RT3>
            <camera type="orthographic"/>
            <film type="image" x_res="200" y_res="100"
                  filename="simple_bkg.ppm" img_type="ppm"/>
            <world_begin/>
                <background type="single_color" color="153 204 255"/>
            <world_end/>
        </RT3>
    )";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}