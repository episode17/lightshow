#define PIXELS  120  // Number of pixels in the string

// DueMilanove pin 12
#define PIXEL_PORT  PORTB
#define PIXEL_DDR   DDRB
#define PIXEL_BIT   4


// WS2812 timing constraints
#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns
#define T0H  400    // Width of a 0 bit in ns
#define T0L  900    // Width of a 0 bit in ns
#define RES 6000    // Width of the low gap between bits to cause a frame to latch


// Convenience defines for using nanoseconds specs to generate actual CPU delays
#define NS_PER_SEC      (1000000000L)    // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC  (F_CPU)
#define NS_PER_CYCLE    (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)


// Send bit to string
void sendBit(bool bitVal) {
    if (bitVal) {   // 0 bit
      
        asm volatile (
            "sbi %[port], %[bit] \n\t"                      // Set the output bit
            ".rept %[onCycles] \n\t"                        // Execute NOPs to delay exactly the specified number of cycles
            "nop \n\t"
            ".endr \n\t"
            "cbi %[port], %[bit] \n\t"                      // Clear the output bit
            ".rept %[offCycles] \n\t"                       // Execute NOPs to delay exactly the specified number of cycles
            "nop \n\t"
            ".endr \n\t"
            ::
            [port]      "I" (_SFR_IO_ADDR(PIXEL_PORT)),
            [bit]       "I" (PIXEL_BIT),
            [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),        // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
            [offCycles] "I" (NS_TO_CYCLES(T1L) - 2)         // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness
        );
                                  
    } else {    // 1 bit

        asm volatile (
            "sbi %[port], %[bit] \n\t"                      // Set the output bit
            ".rept %[onCycles] \n\t"                        // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
            "nop \n\t"                                      // Execute NOPs to delay exactly the specified number of cycles
            ".endr \n\t"
            "cbi %[port], %[bit] \n\t"                      // Clear the output bit
            ".rept %[offCycles] \n\t"                       // Execute NOPs to delay exactly the specified number of cycles
            "nop \n\t"
            ".endr \n\t"
            ::
            [port]      "I" (_SFR_IO_ADDR(PIXEL_PORT)),
            [bit]       "I" (PIXEL_BIT),
            [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
            [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
        );

    }    
}


void sendByte(unsigned char byte) {
    // Neopixel wants bit in highest-to-lowest order
    for(unsigned char bit = 0; bit < 8; bit++) {
        sendBit(bitRead(byte, 7));
        byte <<= 1;
    }
} 



/**
 * API
 *
 * ledSetup()           - set up the pin that is connected to the string. Call once at the begining of the program.  
 * sendPixel(r, g, b)   - send a single pixel to the string. Call this once for each pixel in a frame.
 * show()               - show the recently sent pixel on the LEDs . Call once per frame. 
 */

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
 
void ledSetup() {
    bitSet(PIXEL_DDR, PIXEL_BIT);    // Set pin to digital out
}


void sendPixel(unsigned char r, unsigned char g, unsigned char b) {
    sendByte(pgm_read_byte(&gamma[g]));    // Neopixel wants colors in green then red then blue order
    sendByte(pgm_read_byte(&gamma[r]));
    sendByte(pgm_read_byte(&gamma[b]));
    
    // sendByte(g);
    // sendByte(r);
    // sendByte(b);
}


void show() {
    _delay_us((RES / 1000UL) + 1);
}



/******************************************************************************
 * TESTS
 */

// Display a single color on the whole string
void showColor( unsigned char r , unsigned char g , unsigned char b ) {
    cli();
    
    for(int p = 0; p < PIXELS; p++) {
        sendPixel(r, g, b);
    }
    
    sei();
    show();
}

// Fill the dots one after the other with a color
// rewrite to lift the compare out of the loop
void colorWipe(unsigned char r , unsigned char g, unsigned char b, unsigned char wait) {
    for (unsigned int i = 0; i < PIXELS; i += (PIXELS/60)) {
        cli();
        unsigned int p = 0;

        while (p++ <= i) {
            sendPixel(r, g, b);
        } 

        while (p++ <= PIXELS) {
            sendPixel(0, 0, 0);  
        }

        sei();
        show();
        delay(wait);
    }
}

// Theatre-style crawling lights.
// Changes spacing to be dynmaic based on string size

#define THEATER_SPACING (PIXELS/6)

void theaterChase( unsigned char r , unsigned char g, unsigned char b, unsigned char wait ) {
  
  for (int j=0; j< 3 ; j++) {  
  
    for (int q=0; q < THEATER_SPACING ; q++) {
      
      unsigned int step=0;
      
      cli();
      
      for (int i=0; i < PIXELS ; i++) {
        
        if (step==q) {
          
          sendPixel( r , g , b );
          
        } else {
          
          sendPixel( 0 , 0 , 0 );
          
        }
        
        step++;
        
        if (step==THEATER_SPACING) step =0;
        
      }
      
      sei();
      
      show();
      delay(wait);
      
    }
    
  }
  
}
        


// I rewrite this one from scrtach to use high resolution for the color wheel to look nicer on a *much* bigger string
                                                                            
void rainbowCycle(unsigned char frames , unsigned int frameAdvance, unsigned int pixelAdvance ) {
  
  // Hue is a number between 0 and 3*256 than defines a mix of r->g->b where
  // hue of 0 = Full red
  // hue of 128 = 1/2 red and 1/2 green
  // hue of 256 = Full Green
  // hue of 384 = 1/2 green and 1/2 blue
  // ...
  
  unsigned int firstPixelHue = 0;     // Color for the first pixel in the string
  
  for(unsigned int j=0; j<frames; j++) {                                  
    
    unsigned int currentPixelHue = firstPixelHue;
       
    cli();    
        
    for(unsigned int i=0; i< PIXELS; i++) {
      
      if (currentPixelHue>=(3*256)) {                  // Normalize back down incase we incremented and overflowed
        currentPixelHue -= (3*256);
      }
            
      unsigned char phase = currentPixelHue >> 8;
      unsigned char step = currentPixelHue & 0xff;
                 
      switch (phase) {
        
        case 0: 
          sendPixel( ~step , step ,  0 );
          break;
          
        case 1: 
          sendPixel( 0 , ~step , step );
          break;

        case 2: 
          sendPixel(  step ,0 , ~step );
          break;
          
      }
      
      currentPixelHue+=pixelAdvance;                                      
      
                          
    } 
    
    sei();
    
    show();
    
    firstPixelHue += frameAdvance;
           
  }
}

  
// I added this one just to demonstrate how quickly you can flash the string.
// Flashes get faster and faster until *boom* and fade to black.

void detonate( unsigned char r , unsigned char g , unsigned char b , unsigned int startdelayms) {
  while (startdelayms) {
    
    showColor( r , g , b );      // Flash the color 
    showColor( 0 , 0 , 0 );
    
    delay( startdelayms );      
    
    startdelayms =  ( startdelayms * 4 ) / 5 ;           // delay between flashes is halved each time until zero
    
  }
  
  // Then we fade to black....
  
  for( int fade=256; fade>0; fade-- ) {
    
    showColor( (r * fade) / 256 ,(g*fade) /256 , (b*fade)/256 );
        
  }
  
  showColor( 0 , 0 , 0 );
  
    
}



/**
 * MAIN
 */
void setup() {
    ledSetup();
}


void loop() {
    
    cli();
    
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(229, 31, 115);
    sendPixel(0, 0, 255);
    sendPixel(0, 0, 0);
    sendPixel(0, 0, 0);
    sendPixel(0, 0, 0);
    sendPixel(0, 0, 0);
    sendPixel(0, 0, 0);
    sendPixel(0, 0, 0);
    
    sei();
    show();
    
    delay(10000000);
    return;
    
    showColor(255, 0, 0);
    showColor(0, 0, 0);
    showColor(255, 0, 0);
    showColor(0, 0, 0);
    
    
    delay(1000);
    showColor(20, 0, 0);
    delay(1000);
    showColor(0, 20, 0);
    delay(1000);
    showColor(0, 0, 20);
    delay(1000);

    rainbowCycle(500, 1, 5);

    // Some example procedures showing how to display to the pixels:
    colorWipe(255, 0, 0, 50);   // Red
    colorWipe(0, 255, 0, 50);   // Green
    colorWipe(0, 0, 255, 50);   // Blue

    // Send a theater pixel chase in...
    theaterChase(127, 127, 127, 100);   // White
    theaterChase(127,   0,   0, 100);   // Red
    theaterChase(  0,   0, 127, 100);   // Blue
    theaterChase(  0, 127,   0, 100);   // Green

    
    //detonate( 255 , 255 , 255 , 1000);

    return;
}
