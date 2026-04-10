#include "exports.hpp"

#include "SketchUpAPI/application/application.h"
#include "SketchUpAPI/application/model.h"
#include "SketchUpAPI/model/image_rep.h"

#include <string>
#include <vector>

namespace {

class Image2dPlugin final {
 public:
  Image2dPlugin();
  ~Image2dPlugin();

  bool Load(const char* image_file_path);
  const char* GetId() const {
    return id_.c_str();
  }

  void BeginFrame(const SUBeginFrameInfo& info);
  void DrawFrame(SUOverlayDrawFrameInfo& info) const;
  void EndFrame() const;

 private:
  struct Image final {
    SUImageRepRef image;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t row_pitch = 0;
    mutable std::vector<SUByte> temp_buffer;
  };

 private:
  std::string id_;
  Image original_;
  Image resized_;
};

}  // namespace

extern "C" OVERLAY_EXPORT void RegisterImageOverlay(const char* image_file_path) {
  auto* plugin = new Image2dPlugin;
  if (!plugin->Load(image_file_path)) {
    delete plugin;
    return;
  }

  SUModelRef model = SU_INVALID;
  if (SUApplicationGetActiveModel(&model) != SU_ERROR_NONE) {
    return;
  }

  SUOverlayCreateInfo info {};  // Set everything to 0/nullptr.
  info.version = SUOVERLAY_CREATE_INFO_VERSION;
  info.user_data = plugin;
  info.id = plugin->GetId();
  info.name = "Example Image Overlay";
  info.desc = "Example demonstrating an image overlay using SketchUp C API";
  info.source = "Example Overlay";
  info.image_orientation = SUOVERLAY_IMAGE_ORIENTATION_TOP_DOWN;

  const auto color_order = SUGetColorOrder();
  info.image_format =
      (color_order.blue_index == 0) ? SUOVERLAY_IMAGE_FORMAT_BGRA : SUOVERLAY_IMAGE_FORMAT_RGBA;

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Callbacks
  info.get_extents = nullptr;

  info.begin_frame = [](SUOverlayRef, const SUBeginFrameInfo* bfi, void* data) {
    if (bfi == nullptr || data == nullptr) {
      // This shouldn't happen but...
      return;
    }

    auto* plugin = static_cast<Image2dPlugin*>(data);
    plugin->BeginFrame(*bfi);
  };

  info.draw_frame = [](SUOverlayRef, SUOverlayDrawFrameInfo* info, void* data) {
    if (info == nullptr || data == nullptr) {
      // This shouldn't happen but...
      return;
    }

    auto* plugin = static_cast<Image2dPlugin*>(data);
    plugin->DrawFrame(*info);
  };

  info.end_frame = [](SUOverlayRef, void* data) {
    auto* plugin = static_cast<Image2dPlugin*>(data);
    plugin->EndFrame();
  };
  //////////////////////////////////////////////////////////////////////////////////////////////////

  SUOverlayRef suoverlay = SU_INVALID;
  if (SUModelCreateOverlay(model, &info, &suoverlay) != SU_ERROR_NONE) {
    delete plugin;
    return;
  }

  if (SUOverlayEnable(suoverlay, true) != SU_ERROR_NONE) {
    SUModelReleaseOverlay(model, &suoverlay);
    delete plugin;
    return;
  }
}

Image2dPlugin::Image2dPlugin() {
  SUSetInvalid(original_.image);
  SUSetInvalid(resized_.image);
  SUImageRepCreate(&original_.image);
  SUImageRepCreate(&resized_.image);

  static int id_count = 0;
  id_ = "sketchup.example_overlay.image2d_" + std::to_string(++id_count);
}

Image2dPlugin::~Image2dPlugin() {
  if (SUIsValid(original_.image)) {
    SUImageRepRelease(&original_.image);
  }
  if (SUIsValid(resized_.image)) {
    SUImageRepRelease(&resized_.image);
  }
}

bool Image2dPlugin::Load(const char* image_file_path) {
  if (SUImageRepLoadFile(original_.image, image_file_path) == SU_ERROR_NONE) {
    SUImageRepConvertTo32BitsPerPixel(original_.image);
    size_t width = 0, height = 0, data_size = 0, bits_per_pixel = 0;
    SUImageRepGetPixelDimensions(original_.image, &width, &height);
    SUImageRepGetDataSize(original_.image, &data_size, &bits_per_pixel);
    original_.width = static_cast<uint32_t>(width);
    original_.height = static_cast<uint32_t>(height);
    original_.row_pitch = static_cast<uint32_t>(data_size / height);
    return true;
  }
  return false;
}

void Image2dPlugin::BeginFrame(const SUBeginFrameInfo& info) {
  if (resized_.width != info.viewport_width || resized_.height != info.viewport_height) {
    if (SUImageRepCopy(resized_.image, original_.image) != SU_ERROR_NONE) {
      return;
    }

    if (SUImageRepResize(resized_.image, info.viewport_width, info.viewport_height) !=
        SU_ERROR_NONE) {
      return;
    }

    size_t width = 0, height = 0, data_size = 0, bits_per_pixel = 0;
    SUImageRepGetPixelDimensions(resized_.image, &width, &height);
    SUImageRepGetDataSize(resized_.image, &data_size, &bits_per_pixel);
    resized_.width = static_cast<uint32_t>(width);
    resized_.height = static_cast<uint32_t>(height);
    resized_.row_pitch = static_cast<uint32_t>(data_size / height);
    resized_.temp_buffer.resize(data_size);
    SUImageRepGetData(resized_.image, data_size, resized_.temp_buffer.data());
  }
}

void Image2dPlugin::DrawFrame(SUOverlayDrawFrameInfo& info) const {
  if (SUIsInvalid(resized_.image)) {
    return;
  }

  info.color.ptr = resized_.temp_buffer.data();
  info.color.row_pitch = resized_.row_pitch;
  info.color.size = static_cast<uint32_t>(resized_.temp_buffer.size());

  info.blending_factor = 1.0f;
  info.depth.ptr = nullptr;
  info.reserved = nullptr;
}

void Image2dPlugin::EndFrame() const {
  // Nothing to do.
}
