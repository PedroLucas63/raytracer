#include <catch2/catch_test_macros.hpp>
#include "Core/ParamSet.hpp"
#include "Parser/ParserScene.hpp"

// ── helper ────────────────────────────────────────────────────────────────────
static raytracer::ParamSet parseElementOrFail(const char* xml, const std::string& element) {
    const auto parsed = raytracer::ParserScene::parseScene(xml, true);
    const auto it = parsed.find(element);
    REQUIRE(it != parsed.end());
    return it->second;
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

    const auto color = backgroundPs.retrieve<std::vector<std::uint8_t>>("color");
    REQUIRE(color.size() == 3);
    REQUIRE(color[0] == 100);
    REQUIRE(color[1] == 149);
    REQUIRE(color[2] == 237);
}

TEST_CASE("parseScene stores background corner colors") {
    const char* xml = R"(
        <scene>
            <background tl="255 0 0" tr="0 255 0" bl="0 0 255" br="255 255 0"/>
        </scene>
    )";
    const auto backgroundPs = parseElementOrFail(xml, "background");

    const auto tl = backgroundPs.retrieve<std::vector<std::uint8_t>>("tl");
    const auto tr = backgroundPs.retrieve<std::vector<std::uint8_t>>("tr");
    const auto bl = backgroundPs.retrieve<std::vector<std::uint8_t>>("bl");
    const auto br = backgroundPs.retrieve<std::vector<std::uint8_t>>("br");

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
    const char* xml = R"(<scene><film invalid_attr="x"/></scene>)";
    const auto parsed = raytracer::ParserScene::parseScene(xml, true);

    REQUIRE(parsed.find("film") != parsed.end());
    REQUIRE_FALSE(parsed.at("film").has("invalid_attr"));
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

    const auto color = backgroundPs.retrieve<std::vector<std::uint8_t>>("color");
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