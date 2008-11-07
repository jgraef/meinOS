#ifndef _CDI_VIDEO_H_
#define _CDI_VIDEO_H_

#include "cdi.h"
#include "cdi/lists.h"

typedef struct {
  unsigned int width;
  unsigned int height;
  unsigned int depth;
  enum {
    CDI_VIDEO_TEXT,
    CDI_VIDEO_GRAPHIC
  } type;
} cdi_video_mode_t;

typedef enum {
  CDI_VIDEO_PRIO_HIGH,
  CDI_VIDEO_PRIO_NORMAL,
  CDI_VIDEO_PRIO_LOW,
} cdi_video_usage_hint_t;

typedef enum {
  CDI_VIDEO_FMT_MONO_1,
  CDI_VIDEO_FMT_MONO_8,
  CDI_VIDEO_FMT_R8G8B8_24,
  CDI_VIDEO_FMT_B8G8R8_24,
  CDI_VIDEO_FMT_R8G8B8_32,
  CDI_VIDEO_FMT_B8G8R8_32,
  CDI_VIDEO_FMT_R5G6B5_16,
  CDI_VIDEO_FMT_B5G6R5_16,
  CDI_VIDEO_FMT_R5G5B5_16,
  CDI_VIDEO_FMT_B5G5R5_16,
  CDI_VIDEO_FMT_GREY_8,
} cdi_video_bitmap_format_t;

struct cdi_video_bitmap {
  int width;
  int height;
  cdi_video_bitmap_format_t format;
  char *pixeldata;
};

typedef enum {
  CDI_VIDEO_ROP_COPY,
  CDI_VIDEO_ROP_OR,
  CDI_VIDEO_ROP_AND,
  CDI_VIDEO_ROP_XOR,
} cdi_video_raster_op_t;

struct cdi_video_device {
  /// CDI-Geraet
  struct cdi_device dev;

  /// Liste der Bildschirme
  cdi_list_t displays;
};

struct cdi_video_display {
  /// Videogeraet, dem dieser Bildschirm angehoert
  struct cdi_video_device *device;

  /// Ob Bildschirm aktiviert ist
  int activated;

  // Derzeitiger ROP
  cdi_video_raster_op_t raster_op;

  /// Momentaner Modus
  cdi_video_mode_t *mode;
};

struct cdi_video_driver {
  /// CDI-Treiber
  struct cdi_driver drv;

  // Basis-Funktionen
  /**
   * Aktiviert einen Bildschirm
   *  @param device Videogeraet
   *  @param display Bildschirm
   *  @return 0 = Erfolg; -1 = Fehlschlag
   *  @note Kann nur angewendet werden wenn der Bildschirm _nicht_ aktiviert ist
   */
  int (*display_enable)(struct cdi_video_display *display);

  /**
   * Deaktiviert einen Bildschirm
   *  @param device Videogeraet
   *  @param display Bildschirm
   *  @return 0 = Erfolg; -1 = Fehlschlag
   *  @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  int (*display_disable)(struct cdi_video_display *display);

  /**
   * Leert einen Bildschirm
   *  @param device Videogeraet
   *  @param display Bildschirm
   *  @return 0 = Erfolg; -1 = Fehlschlag
   *  @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  int (*display_clear)(struct cdi_video_display *display);

  /**
   * Setzt die Aufloesung eines Bildschirms
   *  @param device Videogeraet
   *  @param display Bildschirm
   *  @param mode Modus
   *  @return 0 = Erfolg; -1 = Fehlschlag
   */
  int (*display_set_mode)(struct cdi_video_display *display,cdi_video_mode_t *mode);

  /**
   * Gibt eine Liste aller Modi zurueck
   *  @param device Videogeraet
   *  @param display Bildschirm
   *  @return Liste aller moeglichen Modi
   */
  cdi_list_t (*display_get_mode_list)(struct cdi_video_display *display);

  // Bitmap-Funktionen

  /**
   * Erstellt ein Bitmap
   * @param width Breite des Bitmaps
   * @param height Hoehe des Bitmaps
   * @param format Format der Pixeldaten
   * @param data Pixel, oder 0, wenn das Bitmap vom Treiber beschrieben wird
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  struct cdi_video_bitmap *(*bitmap_create)(struct cdi_video_display *display,unsigned int width, unsigned int height, cdi_video_bitmap_format_t format, void *data);

  /**
   * Loescht ein Bitmap
   * @param bitmap Zu loeschendes Bitmap
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*bitmap_destroy)(struct cdi_video_bitmap *bitmap);

 /**
   * Setzt den Gebrauchsmodus eines Bitmaps (bestimmt zB, ob ein Bitmap gecacht wird)
   * @param bitmap Betroffenes Bitmap
   * @param hint Hinweis auf die Priorität des Bitmaps
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*bitmap_set_usage_hint)(struct cdi_video_bitmap *bitmap, cdi_video_usage_hint_t hint);

  // Zeichen-Funktionen

  /**
   * Setzt den Raster-Op für die folgenden Zeichenfunktionen.
   * Der Raster-Op bestimmt, auf welche Art Grafik gezeichnet wird.
   * @param display Display, auf das gezeichnet wird
   * @param rop Raster-Op
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*set_raster_op)(struct cdi_video_display *display, cdi_video_raster_op_t rop);

  /**
   * Setzt das Ziel für alle Zeichenfunktionen. Kann entweder ein Bitmap oder der Bildschirm sein.
   * @param display Betroffenes Display
   * @param bitmap Zielbitmap fuer alle Zeichenfunktionen. Wenn 0, dann wird auf den Bildschirm gerendert.
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*set_target)(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap);

  /**
   * Zeichnet eine Linie
   * @param display Display, auf das gezeichnet wird
   * @param x1 X-Koordinate des Anfangs
   * @param y1 Y-Koordinate des Anfangs
   * @param x1 X-Koordinate des Endpunktes
   * @param y1 Y-Koordinate des Endpunktes
   * @param color Farbe der Linie
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*draw_line)(struct cdi_video_display *display, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int color);
  /**
   * Zeichnet ein Rechteck
   * @param display Display, auf das gezeichnet wird
   * @param x X-Koordinate
   * @param y Y-Koordinate
   * @param width Breite
   * @param height Hoehe
   * @param color Farbe des Rechtecks
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*draw_rectangle)(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int color);
  /**
   * Zeichnet eine Ellipse
   * @param display Display, auf das gezeichnet wird
   * @param x X-Koordinate
   * @param y Y-Koordinate
   * @param width Breite
   * @param height Hoehe
   * @param color Farbe der Ellipse
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*draw_ellipse)(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int color);

  /**
   * Kopiert einen Bereich des Bildschirminhalts
   * @param display Display, auf das gezeichnet wird
   * @param x X-Koordinate (Ziel)
   * @param y Y-Koordinate (Ziel)
   * @param srcx X-Koordinate (Quelle)
   * @param srcy Y-Koordinate (Quelle)
   * @param width Breite
   * @param height Hoehe
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*copy_screen)(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int srcx, unsigned int srcy, unsigned int width, unsigned int height);

  /**
   * Zeichnet ein Bitmap
   * @param display Display, auf das gezeichnet wird
   * @param bitmap Zu zeichnendes Bitmap
   * @param x X-Koordinate
   * @param y Y-Koordinate
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*draw_bitmap)(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap, unsigned int x, unsigned int y);

  /**
   * Zeichnet ein Bitmap
   * @param display Display, auf das gezeichnet wird
   * @param bitmap Zu zeichnendes Bitmap
   * @param x X-Koordinate (Ziel)
   * @param y Y-Koordinate (Ziel)
   * @param srcx X-Koordinate (Quelle)
   * @param srcy Y-Koordinate (Quelle)
   * @param width Breite
   * @param height Hoehe
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
  void (*draw_bitmap_part)(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap, unsigned int x, unsigned int y, unsigned int srcx, unsigned int srcy, unsigned int width, unsigned int height);

  /**
   * Zeichnet einen Punkt
   *  @param display Display, auf dem gezeichnet wird
   * @param x X-Koordinate (Ziel)
   * @param y Y-Koordinate (Ziel)
   * @note Kann nur angewendet werden wenn der Bildschirm aktiviert ist
   */
   void (*draw_dot)(struct cdi_video_display *display, unsigned int x, unsigned int y);
};

void cdi_video_driver_init(struct cdi_video_driver* driver);
void cdi_video_driver_destroy(struct cdi_video_driver* driver);
void cdi_video_driver_register(struct cdi_video_driver* driver);

#endif
