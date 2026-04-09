#include <catch2/catch_test_macros.hpp>
#include "Core/ParamSet.hpp"
#include "Image/RGBColor.hpp"
#include "Parser/ParserScene.hpp"

// ── helper ────────────────────────────────────────────────────────────────────
static raytracer::ParamSet parseElementOrFail(const char* xml, const std::string& element) {
    const auto parsed = raytracer::ParserScene::parseScene(xml, true);
    raytracer::ParamSet elementPs;
    REQUIRE_NOTHROW(elementPs = parsed.getParam(element));
    return elementPs;
}

// ── <film> ────────────────────────────────────────────────────────────────────

TEST_CASE("parseScene stores film filename") {
    const char* xml = R"(<scene><film filename="out.png"/></scene>)";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE(filmPs.retrieve<std::string>("filename") == "out.png");
}

TEST_CASE("parseScene stores film resolution") {
    const char* xml = R"(<scene><film x_res="1920" y_res="1080"/></scene>)";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE(filmPs.retrieve<int>("x_res") == 1920);
    REQUIRE(filmPs.retrieve<int>("y_res") == 1080);
}

TEST_CASE("parseScene stores film gamma_corrected as true") {
    const char* xml = R"(<scene><film gamma_corrected="true"/></scene>)";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE(filmPs.retrieve<bool>("gamma_corrected"));
}

TEST_CASE("parseScene stores film gamma_corrected as false") {
    const char* xml = R"(<scene><film gamma_corrected="false"/></scene>)";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE_FALSE(filmPs.retrieve<bool>("gamma_corrected"));
}

// ── <background> ─────────────────────────────────────────────────────────────

TEST_CASE("parseScene stores background color") {
    const char* xml = R"(<scene><background color="100 149 237"/></scene>)";
    const auto backgroundPs = parseElementOrFail(xml, "background");

    const auto color = backgroundPs.retrieve<raytracer::RGBColor>("color");
    REQUIRE(color.getRed() == 100);
    REQUIRE(color.getGreen() == 149);
    REQUIRE(color.getBlue() == 237);
}

TEST_CASE("parseScene stores background corner colors") {
    const char* xml = R"(
        <scene>
            <background tl="255 0 0" tr="0 255 0" bl="0 0 255" br="255 255 0"/>
        </scene>
    )";
    const auto backgroundPs = parseElementOrFail(xml, "background");

    const auto tl = backgroundPs.retrieve<raytracer::RGBColor>("tl");
    const auto tr = backgroundPs.retrieve<raytracer::RGBColor>("tr");
    const auto bl = backgroundPs.retrieve<raytracer::RGBColor>("bl");
    const auto br = backgroundPs.retrieve<raytracer::RGBColor>("br");

    REQUIRE(tl.getRed() == 255); REQUIRE(tl.getGreen() == 0);   REQUIRE(tl.getBlue() == 0);
    REQUIRE(tr.getRed() == 0);   REQUIRE(tr.getGreen() == 255); REQUIRE(tr.getBlue() == 0);
    REQUIRE(bl.getRed() == 0);   REQUIRE(bl.getGreen() == 0);   REQUIRE(bl.getBlue() == 255);
    REQUIRE(br.getRed() == 255); REQUIRE(br.getGreen() == 255); REQUIRE(br.getBlue() == 0);
}

// ── elementos inválidos ───────────────────────────────────────────────────────

TEST_CASE("parseScene ignores unknown elements") {
    const char* xml = R"(<scene><unknown_tag foo="bar"/></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

TEST_CASE("parseScene ignores unknown attributes") {
    const char* xml = R"(<scene><film invalid_attr="x"/></scene>)";
    const auto parsed = raytracer::ParserScene::parseScene(xml, true);
    raytracer::ParamSet filmPs;

    REQUIRE_NOTHROW(filmPs = parsed.getParam("film"));
    REQUIRE_FALSE(filmPs.has("invalid_attr"));
}

TEST_CASE("parseScene handles empty scene") {
    const char* xml = R"(<scene></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_NOTHROW(parser.parseScene(xml, true));
}

TEST_CASE("parseScene handles malformed XML") {
    const char* xml = R"(<scene><film x_res="abc"/></scene>)";

    raytracer::ParserScene parser;
    REQUIRE_THROWS_AS(parser.parseScene(xml, true), std::invalid_argument);
}

// ── case insensitive ──────────────────────────────────────────────────────────

TEST_CASE("parseScene is case insensitive for element names") {
    const char* xml = R"(<scene><FILM x_res="800"/></scene>)";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE(filmPs.retrieve<int>("x_res") == 800);
}

TEST_CASE("parseScene is case insensitive for attribute names") {
    const char* xml = R"(<scene><film X_RES="800"/></scene>)";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE(filmPs.retrieve<int>("x_res") == 800);
}


TEST_CASE("parseScene stores film attributes from RT3 document") {
    const char* xml = R"(
        <RT3>
            <film type="image" x_res="200" y_res="100"
                  filename="simple_bkg.ppm" img_type="ppm"/>
        </RT3>
    )";
    const auto filmPs = parseElementOrFail(xml, "film");

    REQUIRE(filmPs.retrieve<std::string>("type")     == "image");
    REQUIRE(filmPs.retrieve<int>("x_res")            == 200);
    REQUIRE(filmPs.retrieve<int>("y_res")            == 100);
    REQUIRE(filmPs.retrieve<std::string>("filename") == "simple_bkg.ppm");
    REQUIRE(filmPs.retrieve<std::string>("img_type") == "ppm");
}

TEST_CASE("parseScene stores background color from RT3 document") {
    const char* xml = R"(
        <RT3>
            <world_begin/>
                <background type="single_color" color="153 204 255"/>
            <world_end/>
        </RT3>
    )";
    const auto backgroundPs = parseElementOrFail(xml, "background");

    REQUIRE(backgroundPs.retrieve<std::string>("type") == "single_color");

    const auto color = backgroundPs.retrieve<raytracer::RGBColor>("color");
    REQUIRE(color.getRed() == 153);
    REQUIRE(color.getGreen() == 204);
    REQUIRE(color.getBlue() == 255);
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
    const char* xml = R"(
        <RT3>
            <camera type="orthographic"/>
        </RT3>
    )";
    const auto cameraPs = parseElementOrFail(xml, "camera");

    REQUIRE(cameraPs.retrieve<std::string>("type") == "orthographic");
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