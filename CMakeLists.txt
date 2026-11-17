# Media key LED indicator for ZMK.
if (CONFIG_ZMK_RGB_MEDIA_INDICATOR)
  zephyr_library()
  zephyr_library_sources(src/rgb_media_indicator.c)
  zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
endif()
