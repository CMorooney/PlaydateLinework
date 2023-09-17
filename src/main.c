#include "pd_api.h"
#include <stdbool.h>
#include <stdio.h>

static int update(void *userdata);

PlaydateAPI* pd;

LCDBitmap* tattoo_gun_bmp;
LCDBitmap* stencil_bmp;
LCDBitmap* meter_container_bmp;

LCDSprite* tattoo_gun_sprite;
LCDSprite* stencil_sprite;
LCDSprite* meter_container_sprite;
LCDSprite* meter_value_sprite;

PDButtons buttons_pressed;
PDButtons buttons_held;
PDButtons buttons_released;

int last_time;
float delta_time = 0.0f;

float accelerometer_x, accelerometer_y, accelerometer_z;

void meter_value_sprite_draw(LCDSprite* sprite, PDRect bounds, PDRect drawrect) {
  float width = fabsf(accelerometer_x) * 30;
  float x = 0;
  if(accelerometer_x > 0) { // left
    x = (LCD_COLUMNS/2.0f) - width;
  } else if (accelerometer_x < 0) {
    x = (LCD_COLUMNS/2.0f);
  }
  pd->graphics->fillRect(x, drawrect.y, width, 25, kColorBlack);
}

void init_sprites(void) {
  const char* outerr;
  tattoo_gun_bmp = pd->graphics->loadBitmap("image/tattoogun", &outerr);
  stencil_bmp = pd->graphics->loadBitmap("image/no_regrets", &outerr);
  meter_container_bmp = pd->graphics->loadBitmap("image/meter", &outerr);

  stencil_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(stencil_sprite, stencil_bmp, kBitmapUnflipped);
  pd->sprite->moveTo(stencil_sprite, LCD_COLUMNS/2.0f, LCD_ROWS/2.0f);
  pd->sprite->addSprite(stencil_sprite);
  pd->sprite->setCollisionsEnabled(stencil_sprite, 0);

  tattoo_gun_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(tattoo_gun_sprite, tattoo_gun_bmp, kBitmapUnflipped);
  pd->sprite->moveTo(tattoo_gun_sprite, LCD_COLUMNS/2.0f, LCD_ROWS/2.0f);
  pd->sprite->addSprite(tattoo_gun_sprite);
  pd->sprite->setCollisionsEnabled(tattoo_gun_sprite, 0);

  meter_container_sprite = pd->sprite->newSprite();
  pd->sprite->setImage(meter_container_sprite, meter_container_bmp, kBitmapUnflipped);
  pd->sprite->moveTo(meter_container_sprite, LCD_COLUMNS/2.0f, 30);
  pd->sprite->addSprite(meter_container_sprite);
  pd->sprite->setCollisionsEnabled(meter_container_sprite, 0);

  meter_value_sprite = pd->sprite->newSprite();
  pd->sprite->moveTo(meter_value_sprite, LCD_COLUMNS/2.0f, 30);
  pd->sprite->setSize(meter_value_sprite, 150, 25);
  pd->sprite->setDrawFunction(meter_value_sprite, meter_value_sprite_draw);
  pd->sprite->addSprite(meter_value_sprite);
  pd->sprite->setCollisionsEnabled(meter_value_sprite, 0);
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
  (void)arg; // silence warning

  if (event == kEventInit) {
    pd = playdate;
    init_sprites();
    playdate->display->setRefreshRate(0);
    pd->system->setPeripheralsEnabled(kAccelerometer);
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

void update_accelerometer(void) {
  pd->system->getAccelerometer(&accelerometer_x, &accelerometer_y, &accelerometer_z);
}

static int update(void* userdata) {
  update_delta_time();
  pd->system->getButtonState(&buttons_pressed, &buttons_held, &buttons_released);
  update_accelerometer();
  pd->sprite->markDirty(meter_value_sprite);
  update_tattoo_gun();
  pd->sprite->updateAndDrawSprites();
  pd->system->drawFPS(0, 0);
  return 1;
}
