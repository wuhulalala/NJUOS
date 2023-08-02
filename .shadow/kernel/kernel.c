#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>




#define SIDE 16

#define IMG_WIDTH 512
#define IMG_HEIGHT 512
static int w, h;  // Screen size

#define RGB2COLOR(R, G, B) (((R) << 16) | ((G) << 8) | (B))
#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = { AM_KEYS(KEYNAME) };

static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
    if (event.keycode == 1) halt(0);
  }
}


static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

//static uint32_t get_pixel(int x, int y) {
  //int position = (y * IMG_WIDTH + x) * 3;
  //uint32_t R, G, B;
  //R = (uint32_t)capybara_ppm[position];
  //G = (uint32_t)capybara_ppm[position + 1];
  //B = (uint32_t)capybara_ppm[position + 2];
  //return RGB2COLOR(R, G, B);
//}

//static uint32_t * get_tile(unsigned char img[], int x, int y) {
  //uint32_t tile[SIDE * SIDE];
  //for (int i = 0; i < SIDE; i += 1) {
    //for (int j = 0; j < SIDE; j += 1) {
      //if (x + j >= IMG_WIDTH || y + i >= IMG_HEIGHT)
        //tile[j + i * SIDE] = 0xffffff;
      //else 
        //tile[j + i * SIDE] = get_pixel(img, x + j, y + i);
    //}
  //}
  //return tile;
//}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;

  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}

// Operating system is a C program!
int main(const char *args) {
  ioe_init();

  puts("mainargs = \"");
  puts(args);  // make run mainargs=xxx
  puts("\"\n");

  splash();

  puts("Press any key to see its key code...\n");
  while (1) {
    print_key();
  }
  return 0;
}
