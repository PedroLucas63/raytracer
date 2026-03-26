#include <cstdint>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include "Scene/Background/BackgroundColor.hpp"

namespace {
   void requirePixel(
      const raytracer::RGBColor& pixel,
      uint8_t red,
      uint8_t green,
      uint8_t blue
   ) {
      REQUIRE(static_cast<int>(pixel.getRed())   == static_cast<int>(red));
      REQUIRE(static_cast<int>(pixel.getGreen()) == static_cast<int>(green));
      REQUIRE(static_cast<int>(pixel.getBlue())  == static_cast<int>(blue));
   }
}

// ── Corners ─────────────────────────────────────────────────────────────────

TEST_CASE("BackgroundColor sampleUV returns exact corner colors") {
   const raytracer::RGBColor tl {255, 0,   0  };
   const raytracer::RGBColor tr {0,   255, 0  };
   const raytracer::RGBColor bl {0,   0,   255};
   const raytracer::RGBColor br {255, 255, 0  };

   const raytracer::BackgroundColor bkg {tl, tr, bl, br};

   requirePixel(bkg.sampleUV(0.0f, 0.0f), 255, 0,   0  ); // top-left
   requirePixel(bkg.sampleUV(1.0f, 0.0f), 0,   255, 0  ); // top-right
   requirePixel(bkg.sampleUV(0.0f, 1.0f), 0,   0,   255); // bottom-left
   requirePixel(bkg.sampleUV(1.0f, 1.0f), 255, 255, 0  ); // bottom-right
}

// ── Solid ────────────────────────────────────────────────────────────────────

TEST_CASE("BackgroundColor solid factory returns the same color everywhere") {
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::solid({100, 150, 200});

   requirePixel(bkg.sampleUV(0.0f, 0.0f), 100, 150, 200);
   requirePixel(bkg.sampleUV(0.5f, 0.5f), 100, 150, 200);
   requirePixel(bkg.sampleUV(1.0f, 1.0f), 100, 150, 200);
}

TEST_CASE("BackgroundColor horizontalGradient factory creates a left-to-right gradient") {
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::horizontalGradient(
      {10, 20, 30},
      {110, 120, 130}
   );

   requirePixel(bkg.sampleUV(0.0f, 0.0f), 10, 20, 30);
   requirePixel(bkg.sampleUV(0.0f, 1.0f), 10, 20, 30);
   requirePixel(bkg.sampleUV(1.0f, 0.0f), 110, 120, 130);
   requirePixel(bkg.sampleUV(1.0f, 1.0f), 110, 120, 130);
   requirePixel(bkg.sampleUV(0.5f, 0.25f), 60, 70, 80);
}

TEST_CASE("BackgroundColor verticalGradient factory creates a top-to-bottom gradient") {
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::verticalGradient(
      {10, 20, 30},
      {110, 120, 130}
   );

   requirePixel(bkg.sampleUV(0.0f, 0.0f), 10, 20, 30);
   requirePixel(bkg.sampleUV(1.0f, 0.0f), 10, 20, 30);
   requirePixel(bkg.sampleUV(0.0f, 1.0f), 110, 120, 130);
   requirePixel(bkg.sampleUV(1.0f, 1.0f), 110, 120, 130);
   requirePixel(bkg.sampleUV(0.25f, 0.5f), 60, 70, 80);
}

TEST_CASE("BackgroundColor diagonalGradientTLBR factory uses the midpoint color on opposite corners") {
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::diagonalGradientTLBR(
      {0, 0, 0},
      {255, 255, 255}
   );

   requirePixel(bkg.sampleUV(0.0f, 0.0f), 0, 0, 0);
   requirePixel(bkg.sampleUV(1.0f, 0.0f), 128, 128, 128);
   requirePixel(bkg.sampleUV(0.0f, 1.0f), 128, 128, 128);
   requirePixel(bkg.sampleUV(1.0f, 1.0f), 255, 255, 255);
   requirePixel(bkg.sampleUV(0.5f, 0.5f), 128, 128, 128);
}

TEST_CASE("BackgroundColor diagonalGradientTRBL factory uses the midpoint color on opposite corners") {
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::diagonalGradientTRBL(
      {255, 255, 255},
      {0, 0, 0}
   );

   requirePixel(bkg.sampleUV(0.0f, 0.0f), 128, 128, 128);
   requirePixel(bkg.sampleUV(1.0f, 0.0f), 255, 255, 255);
   requirePixel(bkg.sampleUV(0.0f, 1.0f), 0, 0, 0);
   requirePixel(bkg.sampleUV(1.0f, 1.0f), 128, 128, 128);
   requirePixel(bkg.sampleUV(0.5f, 0.5f), 128, 128, 128);
}

// ── Linear interpolation ─────────────────────────────────────────────────────

TEST_CASE("BackgroundColor sampleUV interpolates horizontally at the top edge") {
   // tl=black, tr=white  →  u=0.5 should give gray (128)
   const raytracer::BackgroundColor bkg {
      {0, 0, 0}, {255, 255, 255},   // top:    black → white
      {0, 0, 0}, {0,   0,   0  }    // bottom: black → black
   };

   requirePixel(bkg.sampleUV(0.5f, 0.0f), 128, 128, 128);
}

TEST_CASE("BackgroundColor sampleUV interpolates horizontally at the bottom edge") {
   const raytracer::BackgroundColor bkg {
      {0, 0, 0}, {0,   0,   0  },   // top:    black → black
      {0, 0, 0}, {255, 255, 255}    // bottom: black → white
   };

   requirePixel(bkg.sampleUV(0.5f, 1.0f), 128, 128, 128);
}

TEST_CASE("BackgroundColor sampleUV interpolates vertically at the left edge") {
   // tl=black, bl=white  →  v=0.5 should give gray (128)
   const raytracer::BackgroundColor bkg {
      {0,   0,   0  }, {0, 0, 0},   // top:   black → black
      {255, 255, 255}, {0, 0, 0}    // bottom: white → black
   };

   requirePixel(bkg.sampleUV(0.0f, 0.5f), 128, 128, 128);
}

TEST_CASE("BackgroundColor sampleUV returns center average of all four corners") {
   // All corners different, center = average
   const raytracer::BackgroundColor bkg {
      {0,   0,   0  },   // tl
      {255, 0,   0  },   // tr
      {0,   255, 0  },   // bl
      {255, 255, 0  }    // br
   };

   // u=0.5 top  → (0+255)/2=128 red,   0 green
   // u=0.5 bot  → (0+255)/2=128 red, (255+0)/2=128 green... wait, let's compute:
   // top  lerp u=0.5: r=(0+255)/2=128, g=0,   b=0
   // bot  lerp u=0.5: r=(0+255)/2=128, g=(255+255)/2? No: bl={0,255,0}, br={255,255,0}
   // bot  lerp u=0.5: r=128, g=255, b=0
   // v=0.5 lerp: r=128, g=128, b=0
   requirePixel(bkg.sampleUV(0.5f, 0.5f), 128, 128, 0);
}

// ── samplePixel ───────────────────────────────────────────────────────────────

TEST_CASE("BackgroundColor samplePixel maps pixel coordinates to correct UV") {
   // Same solid color — result must be identical regardless of pixel position
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::solid({80, 160, 240});

   requirePixel(bkg.samplePixel(0,   0,   4, 2), 80, 160, 240);
   requirePixel(bkg.samplePixel(3,   1,   4, 2), 80, 160, 240);
   requirePixel(bkg.samplePixel(1,   0,   4, 2), 80, 160, 240);
}

TEST_CASE("BackgroundColor samplePixel corner pixels match sampleUV corner colors") {
   const raytracer::RGBColor tl {10, 20, 30};
   const raytracer::RGBColor tr {40, 50, 60};
   const raytracer::RGBColor bl {70, 80, 90};
   const raytracer::RGBColor br {100, 110, 120};

   const raytracer::BackgroundColor bkg {tl, tr, bl, br};
   const uint16_t W = 4, H = 3;

   requirePixel(bkg.samplePixel(0,     0,     W, H), 10,  20,  30 ); // tl
   requirePixel(bkg.samplePixel(W-1,   0,     W, H), 40,  50,  60 ); // tr
   requirePixel(bkg.samplePixel(0,     H-1,   W, H), 70,  80,  90 ); // bl
   requirePixel(bkg.samplePixel(W-1,   H-1,   W, H), 100, 110, 120); // br
}

// ── Clamp ─────────────────────────────────────────────────────────────────────

TEST_CASE("BackgroundColor sampleUV clamps out-of-range UV to valid colors") {
   const raytracer::BackgroundColor bkg {
      {255, 0, 0}, {255, 0, 0},
      {255, 0, 0}, {255, 0, 0}
   };

   // Should not throw — clamp silently
   REQUIRE_NOTHROW(bkg.sampleUV(-0.1f, 0.5f));
   REQUIRE_NOTHROW(bkg.sampleUV(1.1f,  0.5f));
   REQUIRE_NOTHROW(bkg.sampleUV(0.5f, -0.1f));
   REQUIRE_NOTHROW(bkg.sampleUV(0.5f,  1.1f));
}

// ── toImage ──────────────────────────────────────────────────────────────────

TEST_CASE("BackgroundColor toImage generates an image with sampled pixels") {
   const raytracer::BackgroundColor bkg = raytracer::BackgroundColor::horizontalGradient(
      {0, 0, 0},
      {255, 255, 255}
   );

   const raytracer::Image image = bkg.toImage(3, 2);

   REQUIRE(image.getWidth() == 3);
   REQUIRE(image.getHeight() == 2);
   REQUIRE(image.getChannels() == 3);

   requirePixel(image.getPixel(0, 0), 0, 0, 0);
   requirePixel(image.getPixel(0, 1), 128, 128, 128);
   requirePixel(image.getPixel(0, 2), 255, 255, 255);
   requirePixel(image.getPixel(1, 0), 0, 0, 0);
   requirePixel(image.getPixel(1, 1), 128, 128, 128);
   requirePixel(image.getPixel(1, 2), 255, 255, 255);
}