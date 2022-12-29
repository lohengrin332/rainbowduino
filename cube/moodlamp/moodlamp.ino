/*
  Sets pixels on 3D plane (4x4x4 cube) in an oscillating mood lamp configuration
*/

#include "Rainbowduino.h"

const unsigned char R = 0;
const unsigned char G = 1;
const unsigned char B = 2;

const signed int DELAY = 6;

const unsigned char DIMENSIONS = 4;
const unsigned char MAX_DIST_FROM_O = DIMENSIONS * DIMENSIONS - 1;
const unsigned char SEED_SIZE = 113;

boolean watchBit = true;

class PixelMatrix {
  private:
    class Pixel {
      private:
        class PixelColor {
          public:
            boolean isRising = true;
            unsigned char value;
            
            PixelColor() {
              PixelColor(0);
            }
            PixelColor(unsigned char value) {
              this->value = value;
            }
            
            boolean advance() {
              if (this->isRising) {
                if (this->value == 255) {
                  this->isRising = false;
                  return this->advance();
                }
                this->value++;
                return true;
              } else {
                if (this->value <= 25) {
                  this->isRising = true;
                  return false;
                }
                this->value--;
                return true;
              }
            }
        };
    
      public:
        unsigned char z;
        unsigned char x;
        unsigned char y;
        PixelColor r;
        PixelColor g;
        PixelColor b;
        
        
        Pixel() {
          Pixel(0, 0, 0);
        }
        
        Pixel(unsigned char z, unsigned char x, unsigned char y) {
          Pixel(z, x, y, 0, 0, 0);
        }
        
        Pixel(unsigned char z, unsigned char x, unsigned char y, unsigned char r, unsigned char g, unsigned char b) {
          this->z = z;
          this->x = x;
          this->y = y;
          
          this->r = PixelColor(r);
          this->g = PixelColor(g);
          this->b = PixelColor(b);
        }
        
        void apply() {
          Rb.setPixelZXY(this->z, this->x, this->y, this->r.value, this->g.value, this->b.value);
          
          // watchBit ? Rb.setPixelZXY(3, 0, 3, 0x0F0F0F) : Rb.setPixelZXY(3, 0, 3, 0xF0F0F0);
        }
        
        void advance(unsigned char first, unsigned char second, unsigned char third) {
          if (!this->advance(first) && !this->advance(second) && !this->advance(third)) this->advance(first);
        }
        
        void setColor(unsigned char r, unsigned char g, unsigned char b) {
          this->r.value = r;
          this->g.value = g;
          this->b.value = b;
        }
          
      private:
        boolean advance(unsigned char color) {
          if (color == R) return this->r.advance();
          if (color == G) return this->g.advance();
          return this->b.advance();
        }
    };

    Pixel matrix[DIMENSIONS][DIMENSIONS][DIMENSIONS];
    
  public:
    boolean initComplete = false;
    
    PixelMatrix() {
      PixelMatrix(0, 0, 0);
    }
    PixelMatrix(unsigned char r, unsigned char g, unsigned char b) {
      for (unsigned char z=0; z<DIMENSIONS; z++) {
        for (unsigned char x=0; x<DIMENSIONS; x++) {
          for (unsigned char y=0; y<DIMENSIONS; y++) {
            matrix[z][x][y] = Pixel(z, x, y, r, g, b);
          }
        }
      }
    }
    
    void setMatrixColor(unsigned char r, unsigned char g, unsigned char b) {
      for (unsigned char z=0; z<DIMENSIONS; z++) {
        for (unsigned char x=0; x<DIMENSIONS; x++) {
          for (unsigned char y=0; y<DIMENSIONS; y++) {
            matrix[z][x][y].setColor(r, g, b);
          }
        }
      }
      
      apply();
    }
    
    void moodLamp(unsigned char first, unsigned char second, unsigned char third) {
      if (!initComplete) waveInit(first, second, third);
      advanceMatrix(first, second, third);
    }
    
    void advanceMatrix(unsigned char first, unsigned char second, unsigned char third) {
      for (unsigned char z=0; z<DIMENSIONS; z++) {
        for (unsigned char x=0; x<DIMENSIONS; x++) {
          for (unsigned char y=0; y<DIMENSIONS; y++) {
            matrix[z][x][y].advance(first, second, third);
          }
        }
      }
      
      delay(DELAY);
      
      apply();
    }
    
    void waveInit(unsigned char first, unsigned char second, unsigned char third) {
      for (unsigned char distFromOrigin=0; distFromOrigin<MAX_DIST_FROM_O; distFromOrigin++) {
        for (unsigned char seed=0; seed<SEED_SIZE; seed++) {
          for (unsigned char z=0; z<DIMENSIONS && z<distFromOrigin; z++) {
            for (unsigned char x=0; x<DIMENSIONS && z+x < distFromOrigin; x++) {
              for (unsigned char y=0; y<DIMENSIONS && z+x+y < distFromOrigin; y++) {
                matrix[z][x][y].advance(first, second, third);
              }
            }
          }
          
          delay(DELAY);
          
          apply();
        }
      }
      initComplete = true;
    }
     
    void apply() {
      for (unsigned char z=0; z<DIMENSIONS; z++) {
        for (unsigned char x=0; x<DIMENSIONS; x++) {
          for (unsigned char y=0; y<DIMENSIONS; y++) {
            matrix[z][x][y].apply();
          }
        }
      }
    }
};

PixelMatrix pixelMatrix(0, 0, 0);

void setup()
{
  Rb.init(); //initialize Rainbowduino driver
}

void loop()
{
  pixelMatrix.moodLamp(B, R, G);
}

