#pragma once

#include "Bitmaps/Bitmap.hpp"

namespace acid {
class ACID_EXPORT BitmapJpg : public Bitmap::Registrar<BitmapJpg> {
	inline static const bool Registered = Register(".jpg", ".jpeg");
public:
	static void Load(Bitmap *bitmap, const std::filesystem::path &filename);
	static void Write(const Bitmap *bitmap, const std::filesystem::path &filename);
};
}
