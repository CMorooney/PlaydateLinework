#include "pd_api.h"
#include <stdbool.h>

static int update(void *userdata);

PlaydateAPI* pd;

LCDBitmap* tattoo_gun_bmp;
LCDBitmap* stencil_bmp;

LCDSprite* tattoo_gun_sprite;
LCDSprite* stencil_sprite;

PDButtons buttons_pressed;
PDButtons buttons_held;
PDButtons buttons_released;

int last_time;
float delta_time = 0.0f;

void init_sprites(void) {
  const char* outerr;
  tattoo_gun_bmp = pd->graphics->loadBitmap("image/tattoogun", &outerr);
  stencil_bmp = pd->graphics->loadBitmap("image/no_regrets", &outerr);

  stencil_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(stencil_sprite, stencil_bmp, kBitmapUnflipped);
  pd->sprite->moveTo(stencil_sprite, LCD_COLUMNS/2, LCD_ROWS/2);
  pd->sprite->addSprite(stencil_sprite);

  tattoo_gun_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(tattoo_gun_sprite, tattoo_gun_bmp, kBitmapUnflipped);
  pd->sprite->moveTo(tattoo_gun_sprite, LCD_COLUMNS/2, LCD_ROWS/2);
  pd->sprite->addSprite(tattoo_gun_sprite);
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
  (void)arg; // silence warning

  if (event == kEventInit) {
    pd = playdate;
    init_sprites();
    playdate->display->setRefreshRate(0);
    pd->system->setUpdateCallback(update, playdate);
  }

  return 0;
}

void update_delta_time(void) {
  int current_time = pd->system->getCurrentTimeMilliseconds();
  delta_time = (current_time - last_time)/1000.0f;
  last_time = current_time;
}

bool is_button_pressed(PDButtons button) {
  if(button & buttons_pressed) return true;
  return false;
}

bool is_button_held(PDButtons button) {
  if(button & buttons_held) return true;
  return false;
}

bool is_button_released(PDButtons button) {
  if(button & buttons_released) return true;
  return false;
}

void update_tattoo_gun(void) {
  float x = 0;
  float y = 0;

  if(is_button_pressed(kButtonUp)) {
    y = -50;
  } else if(is_button_pressed(kButtonDown)) {
    y = 50;
  }

  if(is_button_pressed(kButtonLeft)) {
    x = -50;
  } else if(is_button_pressed(kButtonRight)) {
    x = 50;
  }

  pd->sprite->moveBy(tattoo_gun_sprite, x * delta_time, y * delta_time);
}

static int update(void* userdata) {
  update_delta_time();
  pd->system->getButtonState(&buttons_pressed, &buttons_held, &buttons_released);
  update_tattoo_gun();
  pd->sprite->updateAndDrawSprites();
  pd->system->drawFPS(0, 0);
  return 1;
}