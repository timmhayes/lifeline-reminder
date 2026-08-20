/*****************************************************************************
  | File        :   LVGL_Driver.c
  |
  | help        :
    The provided LVGL library file must be installed first
******************************************************************************/
#include "LVGL_Driver.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[ LVGL_BUF_LEN ];
static lv_color_t buf2[ LVGL_BUF_LEN ];

void Lvgl_Display_LCD( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
  LCD_addWindow(area->x1, area->y1, area->x2, area->y2, ( uint16_t *)&color_p->full);
  lv_disp_flush_ready( disp_drv );
}

void Lvgl_Touchpad_Read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
}

void example_increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void Lvgl_Init(void)
{
  lv_init();
  lv_disp_draw_buf_init( &draw_buf, buf1, buf2, LVGL_BUF_LEN);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  disp_drv.hor_res = LVGL_WIDTH;
  disp_drv.ver_res = LVGL_HEIGHT;
  disp_drv.flush_cb = Lvgl_Display_LCD;
  disp_drv.full_refresh = 1;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.sw_rotate = 1;
  disp_drv.rotated = LV_DISP_ROT_90;
  lv_disp_drv_register( &disp_drv );

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = Lvgl_Touchpad_Read;
  lv_indev_drv_register( &indev_drv );

  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);
}
